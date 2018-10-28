#include "send_packet.h"

int main(int argc, char* argv[])
{
  if (argc <= 1) {
    const char* progname = "a.out"; // Default program name
    if (argc > 0) {
      progname = argv[0];
    }
    printf("Usage: sudo %s <dest ip> <dest ip> ...\n", progname);
    printf("Example:\n");
    printf("  sudo %s 192.168.0.8 192.168.0.12\n", progname);
    return 1;
  }

  const uint32_t src = 0; // 0net_addr(argv[1]);
  const uint16_t dst_port = ntohs(22);
  const uint16_t src_port = ntohs(1234);

  const int sock = create_raw_socket();
  if (sock < 0) {
    printf("Failed to create socket. Try running with sudo.\n");
    return 1;
  }

  int i;
  for (i = 1; i < argc; i++) {
    const uint32_t dst = inet_addr(argv[i]);
    const int r0 = send_packet(sock, src, src_port, dst, dst_port, 0, 0, 1, 0);
    if (r0 < 0) {
      printf("send to %s failed\n", argv[i]);
      return 1;
    }
  }

  const int r1 = close(sock);
  if (r1 < 0) {
    printf("could not close socket.\n");
    return -1;
  }

  // Data sent successfully
  printf("Packets sent successfully\n");
  return 0;
}
