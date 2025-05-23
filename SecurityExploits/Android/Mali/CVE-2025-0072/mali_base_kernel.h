/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
/*
 *
 * (C) COPYRIGHT 2010-2022 ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you can access it online at
 * http://www.gnu.org/licenses/gpl-2.0.html.
 *
 */

/*
 * Base structures shared with the kernel.
 */

#ifndef _UAPI_BASE_KERNEL_H_
#define _UAPI_BASE_KERNEL_H_

#include <linux/types.h>

#define BASE_MAX_COHERENT_GROUPS 16

/* Physical memory group ID for normal usage.
 */
#define BASE_MEM_GROUP_DEFAULT (0)

/* Physical memory group ID for explicit SLC allocations.
 */
#define BASE_MEM_GROUP_PIXEL_SLC_EXPLICIT (2)

/* Number of physical memory groups.
 */
#define BASE_MEM_GROUP_COUNT (16)

/**
 * typedef base_mem_alloc_flags - Memory allocation, access/hint flags.
 *
 * A combination of MEM_PROT/MEM_HINT flags must be passed to each allocator
 * in order to determine the best cache policy. Some combinations are
 * of course invalid (e.g. MEM_PROT_CPU_WR | MEM_HINT_CPU_RD),
 * which defines a write-only region on the CPU side, which is
 * heavily read by the CPU...
 * Other flags are only meaningful to a particular allocator.
 * More flags can be added to this list, as long as they don't clash
 * (see BASE_MEM_FLAGS_NR_BITS for the number of the first free bit).
 */
typedef __u32 base_mem_alloc_flags;


struct base_mem_handle {
	struct {
		__u64 handle;
	} basep;
};

/**
 * enum base_mem_import_type - Memory types supported by @a base_mem_import
 *
 * @BASE_MEM_IMPORT_TYPE_INVALID: Invalid type
 * @BASE_MEM_IMPORT_TYPE_UMM: UMM import. Handle type is a file descriptor (int)
 * @BASE_MEM_IMPORT_TYPE_USER_BUFFER: User buffer import. Handle is a
 * base_mem_import_user_buffer
 *
 * Each type defines what the supported handle type is.
 *
 * If any new type is added here ARM must be contacted
 * to allocate a numeric value for it.
 * Do not just add a new type without synchronizing with ARM
 * as future releases from ARM might include other new types
 * which could clash with your custom types.
 */
enum base_mem_import_type {
	BASE_MEM_IMPORT_TYPE_INVALID = 0,
	/*
	 * Import type with value 1 is deprecated.
	 */
	BASE_MEM_IMPORT_TYPE_UMM = 2,
	BASE_MEM_IMPORT_TYPE_USER_BUFFER = 3
};

/**
 * struct base_mem_import_user_buffer - Handle of an imported user buffer
 *
 * @ptr:	address of imported user buffer
 * @length:	length of imported user buffer in bytes
 *
 * This structure is used to represent a handle of an imported user buffer.
 */

struct base_mem_import_user_buffer {
	__u64 ptr;
	__u64 length;
};

/*
 * struct base_fence - Cross-device synchronisation fence.
 *
 * A fence is used to signal when the GPU has finished accessing a resource that
 * may be shared with other devices, and also to delay work done asynchronously
 * by the GPU until other devices have finished accessing a shared resource.
 */
struct base_fence {
	struct {
		int fd;
		int stream_fd;
	} basep;
};

/**
 * struct base_mem_aliasing_info - Memory aliasing info
 *
 * @handle: Handle to alias, can be BASE_MEM_WRITE_ALLOC_PAGES_HANDLE
 * @offset: Offset within the handle to start aliasing from, in pages.
 *          Not used with BASE_MEM_WRITE_ALLOC_PAGES_HANDLE.
 * @length: Length to alias, in pages. For BASE_MEM_WRITE_ALLOC_PAGES_HANDLE
 *          specifies the number of times the special page is needed.
 *
 * Describes a memory handle to be aliased.
 * A subset of the handle can be chosen for aliasing, given an offset and a
 * length.
 * A special handle BASE_MEM_WRITE_ALLOC_PAGES_HANDLE is used to represent a
 * region where a special page is mapped with a write-alloc cache setup,
 * typically used when the write result of the GPU isn't needed, but the GPU
 * must write anyway.
 *
 * Offset and length are specified in pages.
 * Offset must be within the size of the handle.
 * Offset+length must not overrun the size of the handle.
 */
struct base_mem_aliasing_info {
	struct base_mem_handle handle;
	__u64 offset;
	__u64 length;
};

/* Maximum percentage of just-in-time memory allocation trimming to perform
 * on free.
 */
#define BASE_JIT_MAX_TRIM_LEVEL (100)

/* Maximum number of concurrent just-in-time memory allocations.
 */
#define BASE_JIT_ALLOC_COUNT (255)

/* base_jit_alloc_info in use for kernel driver versions 10.2 to early 11.5
 *
 * jit_version is 1
 *
 * Due to the lack of padding specified, user clients between 32 and 64-bit
 * may have assumed a different size of the struct
 *
 * An array of structures was not supported
 */
struct base_jit_alloc_info_10_2 {
	__u64 gpu_alloc_addr;
	__u64 va_pages;
	__u64 commit_pages;
	__u64 extension;
	__u8 id;
};

/* base_jit_alloc_info introduced by kernel driver version 11.5, and in use up
 * to 11.19
 *
 * This structure had a number of modifications during and after kernel driver
 * version 11.5, but remains size-compatible throughout its version history, and
 * with earlier variants compatible with future variants by requiring
 * zero-initialization to the unused space in the structure.
 *
 * jit_version is 2
 *
 * Kernel driver version history:
 * 11.5: Initial introduction with 'usage_id' and padding[5]. All padding bytes
 *       must be zero. Kbase minor version was not incremented, so some
 *       versions of 11.5 do not have this change.
 * 11.5: Added 'bin_id' and 'max_allocations', replacing 2 padding bytes (Kbase
 *       minor version not incremented)
 * 11.6: Added 'flags', replacing 1 padding byte
 * 11.10: Arrays of this structure are supported
 */
struct base_jit_alloc_info_11_5 {
	__u64 gpu_alloc_addr;
	__u64 va_pages;
	__u64 commit_pages;
	__u64 extension;
	__u8 id;
	__u8 bin_id;
	__u8 max_allocations;
	__u8 flags;
	__u8 padding[2];
	__u16 usage_id;
};

/**
 * struct base_jit_alloc_info - Structure which describes a JIT allocation
 *                              request.
 * @gpu_alloc_addr:             The GPU virtual address to write the JIT
 *                              allocated GPU virtual address to.
 * @va_pages:                   The minimum number of virtual pages required.
 * @commit_pages:               The minimum number of physical pages which
 *                              should back the allocation.
 * @extension:                     Granularity of physical pages to grow the
 *                              allocation by during a fault.
 * @id:                         Unique ID provided by the caller, this is used
 *                              to pair allocation and free requests.
 *                              Zero is not a valid value.
 * @bin_id:                     The JIT allocation bin, used in conjunction with
 *                              @max_allocations to limit the number of each
 *                              type of JIT allocation.
 * @max_allocations:            The maximum number of allocations allowed within
 *                              the bin specified by @bin_id. Should be the same
 *                              for all allocations within the same bin.
 * @flags:                      flags specifying the special requirements for
 *                              the JIT allocation, see
 *                              %BASE_JIT_ALLOC_VALID_FLAGS
 * @padding:                    Expansion space - should be initialised to zero
 * @usage_id:                   A hint about which allocation should be reused.
 *                              The kernel should attempt to use a previous
 *                              allocation with the same usage_id
 * @heap_info_gpu_addr:         Pointer to an object in GPU memory describing
 *                              the actual usage of the region.
 *
 * jit_version is 3.
 *
 * When modifications are made to this structure, it is still compatible with
 * jit_version 3 when: a) the size is unchanged, and b) new members only
 * replace the padding bytes.
 *
 * Previous jit_version history:
 * jit_version == 1, refer to &base_jit_alloc_info_10_2
 * jit_version == 2, refer to &base_jit_alloc_info_11_5
 *
 * Kbase version history:
 * 11.20: added @heap_info_gpu_addr
 */
struct base_jit_alloc_info {
	__u64 gpu_alloc_addr;
	__u64 va_pages;
	__u64 commit_pages;
	__u64 extension;
	__u8 id;
	__u8 bin_id;
	__u8 max_allocations;
	__u8 flags;
	__u8 padding[2];
	__u16 usage_id;
	__u64 heap_info_gpu_addr;
};

enum base_external_resource_access {
	BASE_EXT_RES_ACCESS_SHARED,
	BASE_EXT_RES_ACCESS_EXCLUSIVE
};

struct base_external_resource {
	__u64 ext_resource;
};

/**
 * BASE_EXT_RES_COUNT_MAX - The maximum number of external resources
 * which can be mapped/unmapped in a single request.
 */
#define BASE_EXT_RES_COUNT_MAX 10

/**
 * struct base_external_resource_list - Structure which describes a list of
 *                                      external resources.
 * @count:                              The number of resources.
 * @ext_res:                            Array of external resources which is
 *                                      sized at allocation time.
 */
struct base_external_resource_list {
	__u64 count;
	struct base_external_resource ext_res[1];
};

#endif /* _UAPI_BASE_KERNEL_H_ */
