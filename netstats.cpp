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
  
}

void netstats::calcDownSpd() {
  
}

void netstats::calcLatency() {
  
  float sum = 0.0f;
  for (float lt : latencies) {
    sum += lt;
  }
  getStats().latency = sum / latencies.size();
}

void netstats::calcJitter() {
  float differences = 0.0f;
  for (int i = 0; i < latencies.size() - 1; i += 2) {
    differences += std::abs(latencies.at(i) - latencies.at(i + 1));
  }
  getStats().jitter = differences / (latencies.size() - 1);
}

