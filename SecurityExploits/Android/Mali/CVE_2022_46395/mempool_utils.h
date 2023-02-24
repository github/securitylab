#ifndef MEMPOOL_UTILS_H
#define MEMPOOL_UTILS_H

#include <unistd.h>
#include "mali.h"
#include "mali_base_jm_kernel.h"
#include "log_utils.h"

void mem_alloc(int fd, union kbase_ioctl_mem_alloc* alloc);

void reserve_pages(int mali_fd, int pages, int nents, uint64_t* reserved_va);

void map_reserved(int mali_fd, int pages, int nents, uint64_t* reserved_va);

uint64_t drain_mem_pool(int mali_fd);

void release_mem_pool(int mali_fd, uint64_t drain);

#endif
