#include "netdiag.h"
#include <arpa/inet.h>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <set>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

// TODO: make a the return type something to indicate a possible error?
void NetDiag::runPacketLoss() {
  // register socket with OS
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("socket");
    return;
  }

  // set timeout
  struct timeval timeout;
  timeout.tv_sec = 2;
  timeout.tv_usec = 0;
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

  // set up destination
  struct sockaddr_in dest;
  memset(&dest, 0, sizeof(dest));
  dest.sin_family = AF_INET;  // ipv4
  dest.sin_port = htons(123); // ntp port
  inet_pton(AF_INET, "216.239.35.0", &dest.sin_addr);

  std::set<uint32_t> sentSeq;
  std::set<uint32_t> receivedSeq;

  auto sendStart = std::chrono::steady_clock::now();
  ssize_t sent = 0;

  for (uint32_t seq = 0; seq < 30; seq++) {
    NTPPacket packet;
    packet.init(seq);

    sent = sendto(sock, packet.data, sizeof(packet.data), 0,
                  (struct sockaddr *)&dest, sizeof(dest));

    if (sent < 0) {
      perror("sendto");
    } else {
      sentSeq.insert(seq);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  auto sendEnd = std::chrono::steady_clock::now();
  auto sendDuration =
      std::chrono::duration_cast<std::chrono::milliseconds>(sendEnd - sendStart)
          .count();

  auto recvStart = std::chrono::steady_clock::now();
  int maxWaitSeconds = 3;
  ssize_t received = 0;
  NTPPacket response;

  // receive
  while (true) {
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);

    received = recvfrom(sock, response.data, sizeof(response.data), 0,
                        (struct sockaddr *)&from_addr, &from_len);

    if (received > 0) {
      uint32_t seq = response.get_sequence();
      receivedSeq.insert(seq);
    }

    // Check if we've waited long enough
    auto now = std::chrono::steady_clock::now();
    auto elapsed =
        std::chrono::duration_cast<std::chrono::seconds>(now - recvStart)
            .count();

    if (elapsed >= maxWaitSeconds) {
      break;
    }
  }

  std::cout << "Received " << receivedSeq.size() << " responses out of "
            << sentSeq.size() << "\n";


  int lost = sentSeq.size() - receivedSeq.size();
  if (lost > 0) {
    std::cout << "\nLost packet sequences: ";
    int count = 0;
    for (uint32_t seq : sentSeq) {
      if (receivedSeq.find(seq) == receivedSeq.end()) {
        std::cout << seq << " ";
        if (++count >= 20) {
          if (lost > 20) {
            std::cout << "... (and " << (lost - 20) << " more)";
          }
          break;
        }
      }
    }
    std::cout << "\n";
  }



  
  close(sock);
}

void NTPPacket::init(uint32_t sequence) {
  memset(data, 0, sizeof(data));
  data[0] = 0x1B; // NTP version 3, client mode

  // Store sequence number in transmit timestamp field (bytes 40-43)
  uint32_t seq_net = htonl(sequence);
  memcpy(&data[40], &seq_net, sizeof(seq_net));
}

uint32_t NTPPacket::get_sequence() {
  uint32_t seq_net;
  memcpy(&seq_net, &data[40], sizeof(seq_net));
  return ntohl(seq_net);
}
