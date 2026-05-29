#include <cstdint>
#include <chrono>
#include <netinet/in.h>

struct ICMPHeader {
  uint8_t type;     // 8 = echo request, 0 = echo reply
  uint8_t code;     // 0
  uint8_t checksum; 
  uint8_t id;       // process ID to identify packets
  uint8_t sequence; // increments per packet
};

class ICMP {
public:
  // uint16_t checksum(void* data, int len);
  // double ping(int sock, const sockaddr_in& dest, uint16_t seq);
  void startPings();
};

