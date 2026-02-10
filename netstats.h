#include <ncurses.h>
#include <chrono>

struct Mbps {
  float value;
  explicit Mbps(float v) : value(v) {}
};

struct Stats {
  float packetLoss = 0.0f;     // percent
  std::chrono::milliseconds latency {0};        // ms
  std::chrono::milliseconds jitter {0};         // ms
  Mbps downloadSpeed {0.0f};  // mbps
  Mbps uploadSpeed {0.0f};    // mbps
};

namespace netstats {
  inline Stats& getStats() {
    static Stats stats{};
    return stats;
  }
  
  inline const float& getPacketLoss() { return getStats().packetLoss; }
  inline std::chrono::milliseconds getLatency() { return getStats().latency; }
  inline std::chrono::milliseconds getJitter() { return getStats().jitter; }
  inline Mbps getDownloadSpeed() { return getStats().downloadSpeed; }
  inline Mbps getUploadSpeed() { return getStats().uploadSpeed; }

  inline void setPacketLoss(float value) { getStats().packetLoss = value; }
  inline void setLatency(std::chrono::milliseconds value) { getStats().latency = value; }
  inline void setJitter(std::chrono::milliseconds value) { getStats().jitter = value; }
  inline void setDownloadSpeed(Mbps value) { getStats().downloadSpeed = value; }
  inline void setUploadSpeed(Mbps value) { getStats().uploadSpeed = value; }
};
