#include "utils.h"

// Compute the Internet Checksum (https://tools.ietf.org/html/rfc1071). One
// of the particularly clever features of the Internet Checksum is its byte
// order independence. In other words, it is not a bug that this function
// does not contain any calls to `ntohs` or `htons`.
uint16_t checksum(const uint16_t* buf, size_t n, const uint16_t start) {
  // The tot_len field of an IP header is a uint16_t, so the number of
  // bytes will never exceed the capacity of a uint16_t.
  assert(n == (uint16_t)n);

  uint32_t sum = start;

  while (n > 1) {
    sum += *buf;
    buf++;
    n -= 2;
  }

  if (n > 0) {
    // Read the final byte as though there was an extra zero byte
    // at the end of the buffer.
    uint16_t x = 0;
    memcpy(&x, buf, 1);
    sum += x;
  }

  // The code below is an efficent way to compute sum `mod` 0xFFFF.
  // It is based on this insight:
  //
  // 1. 0x10000 `mod` 0xFFFF == 1.
  // 2. Therefore, (a * 0x10000 + b) `mod` 0xFFFF == (a + b) `mod` 0xFFFF.
  sum = (sum>>16) + (sum & 0xFFFF);
  assert(sum <= 0x1FFFE);
  sum = sum + (sum>>16); // Add carry bit.

  // Truncate to 16 bits.
  // Note: this might return the answer 0xFFFF. 0xFFFF `mod` 0xFFFF == 0,
  // so this means that in this case the code returns a different answer
  // than if we had just used the % operator. But the value 0xFFFF is
  // allowed (in fact, preferred) as an internet checksum, so this is ok.
  return sum;
}

// Finalize the checksum by computing its complement (except if
// the complement is zero).
static uint16_t wrapsum(uint16_t c) {
  // In 1's complement arithmetic, the numbers 0 and 0xFFFF are equivalent,
  // but the number 0xFFFF is preferred over 0. So before we compute the
  // complement, we do some bit twiddling to map 0xFFFF to 0.
  uint32_t x = c;
  ++x;
  c += x >> 16;  // x >> 16 == 0, unless c == 0xFFFF.

  return ~c;
}

// Used to calculate the TCP checksum.
struct pseudo_header {
  uint32_t source_address;
  uint32_t dest_address;
  uint8_t placeholder;
  uint8_t protocol;
  uint16_t tcp_length;
};

// Set the checksum fields in an IP header.
void ip_checksum(
  struct iphdr* ip_hdr,
  const size_t ip_hdrlen
) {
  assert(ip_hdrlen == (uint16_t)ip_hdrlen);
  assert((ip_hdrlen & 3) == 0);
  assert(ip_hdr->check == 0);

  // Calculate IP header checksum.
  ip_hdr->check = wrapsum(checksum((uint16_t*)ip_hdr, ip_hdrlen, 0));
}

// Set the IP and TCP checksum fields in a TCP packet.
void tcp_checksum(
  struct iphdr* ip_hdr,
  size_t ip_hdrlen,
  struct tcphdr* tcp_hdr,
  size_t tcp_hdrlen,
  const char* payload,
  size_t payload_len
) {
  assert(ip_hdrlen == (uint16_t)ip_hdrlen);
  assert((ip_hdrlen & 3) == 0);
  assert(tcp_hdrlen == (uint16_t)tcp_hdrlen);
  assert(payload_len == (uint16_t)payload_len);
  assert(ip_hdr->check == 0);
  assert(tcp_hdr->check == 0);

  const size_t ip_len = ip_hdrlen + tcp_hdrlen + payload_len;
  // Note: the assertion below also checks that ip_len will not
  // overflow a uint16_t.
  assert(ip_len == ip_hdr->tot_len);

  // Calculate IP header checksum.
  ip_checksum(ip_hdr, ip_hdrlen);

  // Create pseudo-header for TCP checksum calculation.
  struct pseudo_header psh;
  memset(&psh, 0, sizeof (psh));
  psh.source_address = ip_hdr->saddr;
  psh.dest_address = ip_hdr->daddr;
  psh.placeholder = 0;
  psh.protocol = IPPROTO_TCP;
  psh.tcp_length = htons(tcp_hdrlen + payload_len);

  // Calculate TCP checksum.
  uint16_t sum = 0;
  sum = checksum((uint16_t*)&psh, sizeof(psh), sum);
  sum = checksum((uint16_t*)tcp_hdr, tcp_hdrlen + payload_len, sum);
  tcp_hdr->check = wrapsum(sum);
}

int create_raw_socket() {
  const int sock = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
  if (sock == -1) {
    printf("Failed to create socket. Try running with sudo.\n");
    return -1;
  }

  // IP_HDRINCL to tell the kernel that headers are included in the packet
  int one = 1;
  if (setsockopt (sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof (one)) < 0) {
    printf("Error setting IP_HDRINCL\n");
    return -1;
  }

  return sock;
}

// Create a TCP socket and start listening on the specified port.
int create_socket_and_listen(uint16_t port) {
  // Create a socket for listening on port 80.
  const int sock = socket(PF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    printf("Failed to create socket. Try running with sudo.\n");
    return -1;
  }

  // Allow the port to be reused as soon as the program terminates.
  int one = 1;
  const int r0 =
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
  if (r0 < 0) {
    printf("Failed to set SO_REUSEADDR\n.");
    return -1;
  }

  // Bind the port.
  struct sockaddr_in http_addr;
  memset(&http_addr, 0, sizeof(http_addr));
  http_addr.sin_port = htons(port);
  http_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock, (struct sockaddr *)&http_addr, sizeof(http_addr)) < 0) {
    int err = errno;
    printf(
      "Error binding HTTP socket. Try running with sudo.\nerrno = %d %s\n",
      err, strerror(err)
    );
    return -1;
  }

  // Start listening.
  const int r1 = listen(sock, SOMAXCONN);
  if (r1 < 0) {
    printf("listen failed.\n");
    return -1;
  }

  return sock;
}
