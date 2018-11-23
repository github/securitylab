#include "utils.h"

int send_packet(
  const int sock,
  const uint32_t src, const uint16_t src_port,  // In network byte order
  const uint32_t dst, const uint16_t dst_port,  // In network byte order
  const uint32_t seq, const uint32_t ack_seq,
  const uint16_t syn, const uint16_t ack
);
