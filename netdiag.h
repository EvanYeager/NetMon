#include <cstdint>

struct NTPPacket {
  uint8_t data[48];

  void init(uint32_t);
  uint32_t get_sequence();

};

class NetDiag {
private:
  uint8_t data[48];

public:
  void runPacketLoss();
};
