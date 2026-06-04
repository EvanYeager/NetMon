#include <cstdint>
#include <chrono>
#include <netinet/in.h>
#include <thread>
#include <atomic>

struct ICMPHeader {
  uint8_t type;     // 8 = echo request, 0 = echo reply
  uint8_t code;     // 0
  uint8_t checksum; 
  uint8_t id;       // process ID to identify packets
  uint8_t sequence; // increments per packet
};

class ICMP {
public:
  void startPings(std::atomic<bool>& quitFlag);

};

