#include "netdiag.h"
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include "icmp.h"
#include "speedtester.h"


// TODO: make a the return type something to indicate a possible error?
void NetDiag::runPacketLoss() {

  ICMP icmp;
  icmp.startPings();
}

void NetDiag::runSpeedTest() {
  SpeedTester::downloadTest();
  SpeedTester::uploadTest();
}
