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

  static void addLatency(float lt)       { latencies.push_back(lt); }
  static void addSentPacket()            { getStats().sentPackets++; }
  static void addLostPacket()            { lostPackets++; }
  static void addDownloadTime(Mbps dt)   { downloadSpeeds.push_back(dt); }
  static void addUploadTime(Mbps ut)     { uploadSpeeds.push_back(ut); }

private:
  inline static std::vector<float> latencies;
  inline static int lostPackets;
  inline static std::vector<Mbps> downloadSpeeds;
  inline static std::vector<Mbps> uploadSpeeds;

  static void calcPktLoss();
  static void calcUpSpd();
  static void calcDownSpd();
  static void calcJitter();
  static void calcLatency();

};
