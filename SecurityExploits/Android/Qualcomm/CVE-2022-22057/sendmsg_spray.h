#ifndef SENDMSG_SPRAY_H
#define SENDMSG_SPRAY_H
#include <sched.h>
#include <sys/un.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_SENDMSG_BATCH 6

struct realloc_thread_arg
{
  pthread_t tid;
  int recv_fd;
  int send_fd;
  struct sockaddr_un addr;
  char* realloc_data;
  size_t object_size;
  int spray_cpu;
  int level;
  int type;
  int batch_num;
};

int init_reallocation(struct realloc_thread_arg *rta, size_t nb_reallocs);

void reset();

void realloc_NOW(int);

void cleanup(struct realloc_thread_arg* rta);

#endif
