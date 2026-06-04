#include "netstats.h"
#include <cmath>


void netstats::updateStats() {
  calcPktLoss();
  calcUpSpd();
  calcDownSpd();
  calcLatency();
  calcJitter();
}


void netstats::calcPktLoss() {
  if (getStats().sentPackets == 0) return;
  getStats().lostPacketPcnt = (float)lostPackets / (float)getStats().sentPackets * 100.0f;
}

void netstats::calcUpSpd() {
  if (uploadSpeeds.size() == 0) return;
  float total = 0.0f;
  for (int i = 0; i < uploadSpeeds.size(); i++) {
    total += uploadSpeeds.at(i).value;
  }
  getStats().uploadSpeed = Mbps(total / uploadSpeeds.size());
}

void netstats::calcDownSpd() {
  if (downloadSpeeds.size() == 0) return;
  float total = 0.0f;
  for (int i = 0; i < downloadSpeeds.size(); i++) {
    total += downloadSpeeds.at(i).value;
  }
  getStats().downloadSpeed = Mbps(total / downloadSpeeds.size());
}

void netstats::calcLatency() {
  if (latencies.size() == 0) return;
  float sum = 0.0f;
  for (float lt : latencies) {
    sum += lt;
  }
  getStats().latency = sum / latencies.size();
}

void netstats::calcJitter() {
  if (latencies.size() == 0) return;
  float differences = 0.0f;
  for (int i = 0; i < latencies.size() - 1; i += 2) {
    differences += std::abs(latencies.at(i) - latencies.at(i + 1));
  }
  getStats().jitter = differences / (latencies.size() - 1);
}

