#ifndef ADDR_UTILS
#define ADDR_UTILS

#define PHYS_TO_VIRT_OFF 0x8080000000ul

#define VMEMMAP 0xfffffffefde00000ul

#define KERNEL_PBASE 0xa0080000

#define KERNEL_VBASE 0xffffffc010080000ul

//_text - kernel physical base
#define KERNEL_PHYS_OFF (KERNEL_VBASE - KERNEL_PBASE)

static inline uint64_t page_align(uint64_t x) {
  return (x >> 12) << 12;
}

static inline uint64_t phys_to_virt(uint64_t x) {
  return (uint64_t)(x) - PHYS_TO_VIRT_OFF;
}

static inline uint64_t virt_to_phys_lm(uint64_t x) {
  if (x & (1ul << 38)) err(1, "address is not in low mem range.\n");
  return x + PHYS_TO_VIRT_OFF;
}

static inline uint64_t virt_to_phys(uint64_t x) {
  if (x & (1ul << 38)) return x - (KERNEL_VBASE - KERNEL_PBASE);
  return x + PHYS_TO_VIRT_OFF;
}

static inline uint64_t phys_to_page(uint64_t phys_addr) {
  //VMEMMAP interpreted as page pointer, so pfn needs to multiply by sizeof(struct page)
  return (phys_addr >> 12) * 64 + VMEMMAP;
}

#endif
