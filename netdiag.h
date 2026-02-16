#include <cstdint>
#include <set>

// label corresponds to NTP packet section.
// Value corresponds to starting location of section
enum TimeSeq { reference = 16, origin = 24, receive = 32, transmit = 40 };

struct NTPPacket {
  uint8_t data[48];

  void init(uint64_t);
  uint32_t getSecondSeq(TimeSeq seq);
  int getMiliSeq(TimeSeq seq);
private:
  uint32_t get32BitSeq(uint8_t startingByte);
};

class NetDiag {
private:
  uint8_t data[48];
  std::set<NTPPacket *> sentPackets;
  std::set<NTPPacket *> receivedPackets;

  void listenForNTPPackets(int sock);

public:
  void runPacketLoss();
};
