#include "bar.h"
#include "panel.h"
#include "netstats.h"
#include <ncurses.h>
#include <unistd.h>

void Header::print(WINDOW *window, int height, int width) {
  PanelPrinter::print(window, height, width);

  wbkgd(window, COLOR_PAIR(5));
  box(window, 0, 0);
  wattron(window, WA_BOLD | WA_UNDERLINE);
  mvwprintw(window, 1, (width - 25) / 2, "NETWORK STRENGTH MONITOR");
  wattroff(window, WA_BOLD | WA_UNDERLINE);
  wrefresh(window);
}

void StrPanel::print(WINDOW *window, int height, int width) {
  PanelPrinter::print(window, height, width);

  // packet loss bar
  const LogScale* lossScale = new LogScale(100);
  Bar lossBar(1, 1, netstats::getStats().lostPacketPcnt, lossScale, "Pckt Loss %"); 
  lossBar.setShowValue(true);
  lossBar.print(window, 1, 20);

  // jitter bar
  const LogScale* jitterScale = new LogScale(50, 5.0f);
  Bar jitterBar(3, 1, netstats::getStats().jitter, jitterScale, "jitter, ms");
  jitterBar.setShowValue(true);
  jitterBar.print(window, 1, 20); 

  box(window, 0, 0);
  wattron(window, WA_BOLD);
  mvwprintw(window, 0, 2, "Network Strength");
  wattroff(window, WA_BOLD);
  wrefresh(window);
}

void SpdPanel::print(WINDOW *window, int height, int width) {
  PanelPrinter::print(window, height, width);

  // down speed bar
  const LogScale* speedScale = new LogScale(1000.0f, 30.0f);
  Bar downSpdBar(1, 1, netstats::getStats().downloadSpeed.value, speedScale, "download, Mbps");
  downSpdBar.setShowValue(true);
  downSpdBar.setColorDirection(colorDirection::reversed);
  downSpdBar.print(window, 1, 20);

  // upload speed bar
  Bar upSpdBar(3, 1, netstats::getStats().uploadSpeed.value, speedScale, "upload, Mbps");
  upSpdBar.setShowValue(true);
  upSpdBar.setColorDirection(colorDirection::reversed);
  upSpdBar.print(window, 1, 20);

  // latency bar
  const LogScale* latencyScale = new LogScale(300.0f, 5.0f);
  Bar latencyBar(5, 1, netstats::getStats().latency, latencyScale, "latency, ms"); 
  latencyBar.setShowValue(true);
  latencyBar.print(window, 1, 20);

  box(window, 0, 0);
  wattron(window, WA_BOLD);
  mvwprintw(window, 0, 2, "Network Speed");
  wattroff(window, WA_BOLD);
  wrefresh(window);
}

Footer::Footer() {
  controls = {
      makeFooterItem(std::string("Controls: [Q]uit | [R]efresh | [P]ause"))};

  jitter = {
      makeFooterItem(std::string("Jitter: ")),
      makeFooterItem(std::string("<10ms"), 1),
      makeFooterItem(std::string(" | ")),
      makeFooterItem(std::string("<30ms"), 2),
      makeFooterItem(std::string(" | ")),
      makeFooterItem(std::string(">30ms"), 3),
  };

  speed = {
      makeFooterItem(std::string("Speed:  ")),
      makeFooterItem(std::string(">400mbps"), 1),
      makeFooterItem(std::string(" | ")),
      makeFooterItem(std::string(">50mbps"), 2),
      makeFooterItem(std::string(" | ")),
      makeFooterItem(std::string("<50mbps"), 3),
  };
}

void Footer::print(WINDOW *window, int height, int width) {
  PanelPrinter::print(window, height, width);

  box(window, 0, 0);

  // controls
  printRow(controls, 1, 1);

  // jitter
  printRow(jitter, 1, width - 2 - 36); // right justified

  // speed
  printRow(speed, 2, width - 2 - 36); // right justified

  wrefresh(window);
}

FooterItem Footer::makeFooterItem(std::string text, int colorNum, bool bold) {
  FooterItem item;
  item.text = text;
  item.colorNum = colorNum;
  item.bold = bold;
  return item;
}
