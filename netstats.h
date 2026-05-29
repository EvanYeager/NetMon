#pragma once

#include <ncurses.h>
#include <vector>

struct Mbps {
  float value;
  explicit Mbps(float v) : value(v) {}
};

struct Stats {
  Mbps downloadSpeed {0.0f};
  Mbps uploadSpeed {0.0f};
  float latency = 0.0f;
  float jitter = 0.0f;
  int sentPackets = 0;
  float lostPacketPcnt = 0.0f;
};

class netstats {
public:
  static void updateStats();

  static Stats& getStats() {
    static Stats stats{};
    return stats;
  }

  static void addLatency(float lt) { latencies.push_back(lt); }
  static void addSentPacket()      { getStats().sentPackets++; }
  static void addLostPacket()      { lostPackets++; }

private:
  inline static std::vector<float> latencies;
  inline static int lostPackets;

  static void calcPktLoss();
  static void calcUpSpd();
  static void calcDownSpd();
  static void calcJitter();
  static void calcLatency();

  static void setDownloadSpeed(Mbps value) { getStats().downloadSpeed = value; }
  static void setUploadSpeed(Mbps value)   { getStats().uploadSpeed = value; }
};
