#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

uint16_t checksum(const uint16_t* buf, size_t n, const uint16_t start);

void ip_checksum(
  struct iphdr* ip_hdr,
  const size_t ip_hdrlen
);

void tcp_checksum(
  struct iphdr* ip_hdr,
  size_t ip_hdrlen,
  struct tcphdr* tcp_hdr,
  size_t tcp_hdrlen,
  const char* payload,
  size_t payload_len
);

int create_raw_socket();

int create_socket_and_listen(uint16_t port);
