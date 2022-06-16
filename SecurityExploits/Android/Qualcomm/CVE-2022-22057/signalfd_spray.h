#ifndef SIGNALFD_SPRAY_H
#define SIGNALFD_SPRAY_H

#define CPU_RANGE 7

void spray_signalfd(uint64_t* mask, int num, int cpu, int* fds);

uint64_t read_signalfd_mask(int fd);

void spray_with_intervals(uint64_t interval, int count, int exclude_cpu_mask, uint64_t* mask, int* fds, int spray_size);

int search_changed_mask(uint64_t expected, int* fds, uint64_t fd_size, uint64_t* new_mask);

void change_signalfd_mask(uint64_t* mask, int fd);
#endif
