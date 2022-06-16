#ifndef FAKE_OBJ_UTIL
#define FAKE_OBJ_UTIL

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define ZERO_FILL_SZ 128

#define STACK_OFFSET 0x50

//offset of node in kgsl_timeline_fence
#define NODE_OFF 0x48

struct list_head {
	uint64_t next, prev;
};

typedef struct {
	int counter;
} atomic_t;

typedef struct refcount_struct {
	atomic_t refs;
} refcount_t;

struct kref {
	refcount_t refcount;
};

struct dma_fence {
	void *lock;
	uint64_t ops;
	union {
		struct list_head cb_list;
		int64_t timestamp;
	};
	uint64_t context;
	uint64_t seqno;
	unsigned long flags;
	uint32_t refcount;
	int error;
};

struct kgsl_timeline_fence {
	struct dma_fence base;
	void *timeline;
	struct list_head node;
};

struct scatterlist {
	unsigned long	page_link;
	unsigned int	offset;
	unsigned int	length;
	uint64_t	dma_address;
	unsigned int	dma_length;
};

struct sg_table {
	struct scatterlist *sgl;	/* the list */
	unsigned int nents;		/* number of mapped entries */
	unsigned int orig_nents;	/* original size of list */
};

struct ion_buffer {
	struct list_head list;
	void *heap;
	unsigned long flags;
	unsigned long private_flags;
	size_t size;
	void *priv_virt;
	uint8_t lock[32];
	int kmap_cnt;
	void *vaddr;
	struct sg_table *sg_table;
	struct list_head attachments;
};

uint64_t fill_ion_heap(void* region, size_t chain_size, size_t region_size, uint64_t region_vaddr);

uint64_t poll_list_addr(void* fence_start, size_t chain_size, uint64_t region_vaddr);

void fake_ion_heap(void* region);

void patch_ion_buffer(struct ion_buffer* buffer, uint64_t table_vaddr, uint8_t* table_region, uint64_t phys_addr, size_t size);

#endif
