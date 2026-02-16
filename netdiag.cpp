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


/* Structure used to swap the bytes in a 64-bit unsigned long long. */
union byteswap_64_u {
    unsigned long long a;
    uint32_t b[2];
};
/* Function to byteswap big endian 64bit unsigned integers
 * back to little endian host order on little endian machines. 
 * As above, on big endian machines this will be a null macro.
 * The macro ntohll() is defined in byteorder64.h, and if needed,
 * refers to _ntohll() here.
 */
unsigned long long ntohll(unsigned long long x)
{
    union byteswap_64_u u1;
    union byteswap_64_u u2;

    u1.a = x;

    u2.b[1] = ntohl(u1.b[0]);
    u2.b[0] = ntohl(u1.b[1]);

    return u2.a;
}
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

  // start listening thread now
  std::thread listenThr{&NetDiag::listenForNTPPackets, this, sock};
  auto sendStart = std::chrono::steady_clock::now();
  ssize_t sent = 0;

  for (uint32_t seq = 0; seq < 30; seq++) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    NTPPacket packet;
    packet.init(seq);

    sent = sendto(sock, packet.data, sizeof(packet.data), 0,
                  (struct sockaddr *)&dest, sizeof(dest));

    if (sent < 0) {
      perror("sendto");
    } else {
      sentPackets.insert(&packet);
    }
  }

  auto sendEnd = std::chrono::steady_clock::now();
  auto sendDuration =
      std::chrono::duration_cast<std::chrono::milliseconds>(sendEnd - sendStart)
          .count();

  listenThr.join();

  close(sock);
}

void NetDiag::listenForNTPPackets(int sock) {
  auto recvStart = std::chrono::steady_clock::now();
  int maxWaitSeconds = 3;
  ssize_t received = 0;
  NTPPacket response;

  std::cout << "listening for packets now\n";

  int count = 0;
  while (true) {
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);

    received = recvfrom(sock, response.data, sizeof(response.data), 0,
                        (struct sockaddr *)&from_addr, &from_len);

    if (received > 0) {
      int seq = response.getMiliSeq(TimeSeq::receive);
      receivedPackets.insert(&response);
      count++;
      std::cout << "miliseconds for packet# " << count << ": " << seq << "\n";
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

  std::cout << "Received " << receivedPackets.size() << " responses out of "
            << sentPackets.size() << "\n";
  std::cout << "count = " << count << "\n";
}

void NTPPacket::init(uint64_t sequence) {
  memset(data, 0, sizeof(data));
  data[0] = 0x1B; // NTP version 3, client mode

  // Store sequence number in transmit timestamp field (bytes 40-43)
  uint64_t seq_net = htonl(sequence);
  memcpy(&data[24], &seq_net, sizeof(seq_net));
}

uint32_t NTPPacket::getSecondSeq(TimeSeq seq) {
  uint32_t seconds;
  uint8_t startingByte = seq; // enum corresponds to the location of the section
  memcpy(&seconds, &data[startingByte], sizeof(uint32_t));
  return ntohl(seconds);
}

int NTPPacket::getMiliSeq(TimeSeq seq) {
  uint32_t fracSeconds;
  uint8_t startingByte = seq + 4; // enum corresponds to the location of the section; 4 is the offset to get the second half of the 64 bits
  memcpy(&fracSeconds, &data[startingByte], sizeof(uint32_t));
  return (ntohl(fracSeconds) * 1000.0) / (1ULL << 32); // divide by 2^32 to get the true fraction of a second, then convert to milis
}
