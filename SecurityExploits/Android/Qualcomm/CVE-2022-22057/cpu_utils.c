#include <sys/syscall.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>

#include "cpu_utils.h"

#define CPU_SETSIZE 1024
#define __NCPUBITS  (8 * sizeof (unsigned long))
typedef struct
{
   unsigned long __bits[CPU_SETSIZE / __NCPUBITS];
} cpu_set_t;

#define CPU_SET(cpu, cpusetp) \
  ((cpusetp)->__bits[(cpu)/__NCPUBITS] |= (1UL << ((cpu) % __NCPUBITS)))
#define CPU_ZERO(cpusetp) \
  memset((cpusetp), 0, sizeof(cpu_set_t))

int migrate_to_cpu(int i)
{
    int syscallres;
    pid_t pid = gettid();
    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    CPU_SET(i, &cpu);

    syscallres = syscall(__NR_sched_setaffinity, pid, sizeof(cpu), &cpu);
    if (syscallres)
    {
        return -1;
    }
    return 0;
}

int check_cpu_affinity() {
  if (migrate_to_cpu(4) == -1) return 4;
  if (migrate_to_cpu(5) == -1) return 5;
  return -1;
}

