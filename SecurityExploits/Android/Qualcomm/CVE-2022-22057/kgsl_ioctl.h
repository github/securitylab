#ifndef KGSL_IOCTL
#define KGSL_IOCTL

#define VM_MAYWRITE	0x00000020

/* ioctls */
#define KGSL_IOC_TYPE 0x09

#define IOCTL_KGSL_GPUOBJ_IMPORT \
	_IOWR(KGSL_IOC_TYPE, 0x48, struct kgsl_gpuobj_import)

#define ION_IOC_MAGIC		'I'

#define ION_IOC_ALLOC		_IOWR(ION_IOC_MAGIC, 0, \
				      struct ion_allocation_data)

#define ION_IOC_FREE		_IOWR(ION_IOC_MAGIC, 1, struct ion_handle_data)

#define ION_IOC_MAP		_IOWR(ION_IOC_MAGIC, 2, struct ion_fd_data)

#define ION_IOC_SHARE		_IOWR(ION_IOC_MAGIC, 4, struct ion_fd_data)

#define ION_IOC_IMPORT		_IOWR(ION_IOC_MAGIC, 5, struct ion_fd_data)

#define ION_IOC_SYNC		_IOWR(ION_IOC_MAGIC, 7, struct ion_fd_data)

#define ION_IOC_CUSTOM		_IOWR(ION_IOC_MAGIC, 6, struct ion_custom_data)

#define ION_BIT(nr) (1UL << (nr))

#define ION_HEAP(bit)			ION_BIT(bit)

#define KGSL_MEMFLAGS_USE_CPU_MAP 0x10000000ULL

#define KGSL_MEMFLAGS_SECURE      0x00000008ULL

enum kgsl_user_mem_type {
	KGSL_USER_MEM_TYPE_PMEM		= 0x00000000,
	KGSL_USER_MEM_TYPE_ASHMEM	= 0x00000001,
	KGSL_USER_MEM_TYPE_ADDR		= 0x00000002,
	KGSL_USER_MEM_TYPE_ION		= 0x00000003,
	/*
	 * ION type is retained for backwards compatibility but Ion buffers are
	 * dma-bufs so try to use that naming if we can
	 */
	KGSL_USER_MEM_TYPE_DMABUF       = 0x00000003,
	KGSL_USER_MEM_TYPE_MAX		= 0x00000007,
};

struct kgsl_gpuobj_import {
	uint64_t __user priv;
	uint64_t priv_len;
	uint64_t flags;
	unsigned int type;
	unsigned int id;
};

struct kgsl_gpuobj_import_dma_buf {
	int fd;
};

struct kgsl_gpuobj_import_useraddr {
	uint64_t virtaddr;
};

struct kgsl_gpuobj_free {
	uint64_t flags;
	uint64_t __user priv;
	unsigned int id;
	unsigned int type;
	unsigned int len;
};

#define KGSL_GPUOBJ_FREE_ON_EVENT 1

#define KGSL_GPU_EVENT_TIMESTAMP 1
#define KGSL_GPU_EVENT_FENCE     2

struct kgsl_gpu_event_timestamp {
	unsigned int context_id;
	unsigned int timestamp;
};

struct kgsl_gpu_event_fence {
	int fd;
};

#define IOCTL_KGSL_GPUOBJ_FREE \
	_IOW(KGSL_IOC_TYPE, 0x46, struct kgsl_gpuobj_free)

struct dma_buf_sync {
	__u64 flags;
};

#define DMA_BUF_SYNC_READ      (1 << 0)
#define DMA_BUF_SYNC_WRITE     (2 << 0)
#define DMA_BUF_SYNC_RW        (DMA_BUF_SYNC_READ | DMA_BUF_SYNC_WRITE)
#define DMA_BUF_SYNC_START     (0 << 2)
#define DMA_BUF_SYNC_END       (1 << 2)
#define DMA_BUF_SYNC_VALID_FLAGS_MASK \
	(DMA_BUF_SYNC_RW | DMA_BUF_SYNC_END)

#define DMA_BUF_BASE		'b'
#define DMA_BUF_IOCTL_SYNC	_IOW(DMA_BUF_BASE, 0, struct dma_buf_sync)

#define KGSL_MEMFLAGS_FORCE_32BIT 0x100000000ULL

struct kgsl_timeline_create {
	__u64 seqno;
	__u32 id;
/* private: padding for 64 bit compatibility */
	__u32 padding;
};

#define IOCTL_KGSL_TIMELINE_CREATE \
	_IOWR(KGSL_IOC_TYPE, 0x58, struct kgsl_timeline_create)

/**
 * struct kgsl_timeline_val - A container to store a timeline/sequence number
 * pair.
 * @seqno: Sequence number to signal/query
 * @timeline: The timeline identifier to signal/query
 *
 * A container to store a timeline/seqno pair used by the query and signal
 * ioctls.
 */
struct kgsl_timeline_val {
	__u64 seqno;
	__u32 timeline;
/* private: padding for 64 bit compatibility */
	__u32 padding;
};

#define KGSL_TIMELINE_WAIT_ALL 1
#define KGSL_TIMELINE_WAIT_ANY 2

/**
 * struct kgsl_timeline_wait - Argument for IOCTL_KGSL_TIMELINE_WAIT
 * @tv_sec: Number of seconds to wait for the signal
 * @tv_nsec: Number of nanoseconds to wait for the signal
 * @timelines: Address of an array of &struct kgsl_timeline_val entries
 * @count: Number of entries in @timeline
 * @timelines_size: Size of each entry in @timelines
 * @flags: One of KGSL_TIMELINE_WAIT_ALL or KGSL_TIMELINE_WAIT_ANY
 *
 * Wait for the timelines listed in @timelines to be signaled. If @flags is
 * equal to KGSL_TIMELINE_WAIT_ALL then wait for all timelines or if
 * KGSL_TIMELINE_WAIT_ANY is specified then wait for any of the timelines to
 * signal. @tv_sec and @tv_nsec indicates the number of seconds and nanoseconds
 * that the process should be blocked waiting for the signal.
 */
struct kgsl_timeline_wait {
	__s64 tv_sec;
	__s64 tv_nsec;
	__u64 timelines;
	__u32 count;
	__u32 timelines_size;
	__u32 flags;
/* private: padding for 64 bit compatibility */
	__u32 padding;
};

#define IOCTL_KGSL_TIMELINE_WAIT \
	_IOW(KGSL_IOC_TYPE, 0x59, struct kgsl_timeline_wait)

#define IOCTL_KGSL_TIMELINE_QUERY \
	_IOWR(KGSL_IOC_TYPE, 0x5A, struct kgsl_timeline_val)

/**
 * struct kgsl_timeline_signal - argument for IOCTL_KGSL_TIMELINE_SIGNAL
 * @timelines: Address of an array of &struct kgsl_timeline_val entries
 * @count: Number of entries in @timelines
 * @timelines_size: Size of each entry in @timelines
 *
 * Signal an array of timelines of type @struct kgsl_timeline_val.
 */
struct kgsl_timeline_signal {
	__u64 timelines;
	__u32 count;
	__u32 timelines_size;
};

#define IOCTL_KGSL_TIMELINE_SIGNAL \
	_IOW(KGSL_IOC_TYPE, 0x5B, struct kgsl_timeline_signal)

/**
 * struct kgsl_timeline_fence_get - argument for IOCTL_KGSL_TIMELINE_FENCE_GET
 * @seqno: Sequence number for the fence
 * @timeline: Timeline to create the fence on
 * @handle: Contains the fence fd for a successful operation [out]
 *
 * Create a sync file descriptor for the seqnum on the timeline and return it in
 * @handle.  Can be polled and queried just like any other sync file descriptor
 */
struct kgsl_timeline_fence_get {
	__u64 seqno;
	__u32 timeline;
	int handle;
};

#define IOCTL_KGSL_TIMELINE_FENCE_GET \
	_IOWR(KGSL_IOC_TYPE, 0x5C, struct kgsl_timeline_fence_get)
/**
 * IOCTL_KGSL_TIMELINE_DESTROY takes a u32 identifier for the timeline to
 * destroy
 */
#define IOCTL_KGSL_TIMELINE_DESTROY _IOW(KGSL_IOC_TYPE, 0x5D, __u32)

#endif
