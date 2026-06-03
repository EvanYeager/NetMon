#include "icmp.h"
#include "netstats.h"
#include <array>
#include <cstdio>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <unistd.h>

void ICMP::startPings() {
  worker = std::jthread([this] { ping(); });
}

void ICMP::ping() {  
  std::array<char, 128> buffer;
  std::string result;

  std::unique_ptr<FILE, void (*)(FILE *)> pipe(
      popen("ping 8.8.8.8 -c 100 -i .2 -v 2>/dev/null", "r"),
      [](FILE *f) -> void {
        // wrapper to ignore the return value from pclose() is needed with newer
        // versions of gnu g++
        std::ignore = pclose(f);
      });

  int lastSeq = 0;
  while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) !=
         nullptr) {
    netstats::addSentPacket();
    std::string line = buffer.data();
    size_t start = line.find("time=");
    size_t end = line.find(" ms");

    // npos means not found
    if (start == std::string::npos || end == std::string::npos) {
      if (line.find("ping statistics") != std::string::npos) {
        // end of ping output
      }
      continue;
    }

    // sequence number -- for finding lost packets
    size_t seqStart = line.find("icmp_seq=");
    size_t seqEnd = line.find(" ttl=");
    if (seqStart != std::string::npos && seqEnd != std::string::npos) {
      int seq = stoi(line.substr(seqStart + 9, seqEnd - (seqStart + 9)));
      int numLost = seq - (lastSeq + 1); // if this seq is more than one greater
                                         // than previous, it lost one
      for (int i = 0; i < numLost; i++) {
        netstats::addLostPacket();
      }
      lastSeq = seq;
    }

    float timeInMs = std::stof(line.substr(start + 5, end - (start + 5)));
    netstats::addLatency(timeInMs);
  }
}

void ICMP::stopPings() {
  worker.detach();
}
