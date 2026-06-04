#include "icmp.h"
#include "netstats.h"
#include "panel.h"
#include "speedtester.h"
#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <ncurses.h>
#include <thread>

struct PanelStruct {
  WINDOW *window;
  PanelPrinter *panel;
  int y;
  int x;
};

  std::atomic<bool> shouldStop{false};
  std::condition_variable cv;
  std::mutex cvMutex;
  std::thread refreshThrd;

void networkTest() {
  // speed test (both at once)
  std::thread downThrd([]() { SpeedTester::downloadTest(shouldStop); });
  std::thread upThrd([]() { SpeedTester::uploadTest(shouldStop); });

  // wait to finish before doing other tests
  downThrd.join();
  upThrd.join();

  if (shouldStop) return;
  ICMP icmp;
  icmp.startPings(shouldStop);
}
  

class NetMon {
  const int HEADER_HEIGHT = 4;
  const int MAIN_HEIGHT = 11;
  const int MAIN_PANEL_WIDTH = 50;
  const int FOOTER_HEIGHT = 6;

  std::array<PanelStruct, 4> panels{};


public:

  void run(int argc, char *argv[]) {

    initscr();            /* Start curses mode 		*/
    raw();                /* Line buffering disabled	*/
    keypad(stdscr, TRUE); /* We get F1, F2 etc..		*/
    noecho();             /* Don't echo() while we do getch */
    curs_set(0);

    initColors();

    refresh();

    int y, x;
    getmaxyx(stdscr, y, x); /* get the number of rows and columns */

    makePanels(y, x);

    // start a new thread to refresh panels with new data (if available)
    refreshThrd = std::thread([this] {
      while (!shouldStop) {
        std::unique_lock<std::mutex> lock(cvMutex);
        cv.wait_for(lock, std::chrono::milliseconds(2000),
                    [this] { return shouldStop.load(); });

        if (!shouldStop) {
          netstats::updateStats();
          panels[1].panel->print(panels[1].window, MAIN_HEIGHT,
                                 MAIN_PANEL_WIDTH); // strength panel
          panels[2].panel->print(panels[2].window, MAIN_HEIGHT,
                                 MAIN_PANEL_WIDTH); // speed panel
        }
      }
    });

    std::thread networkTestThrd(networkTest);

    while (true) {
      int ch = getch();

      if (ch == 'q' || ch == 'Q') {
        break;
      }
    }

    shouldStop = true; // tell thread to stop
    cv.notify_all();   // wake up the thread

    // join all threads
    if (refreshThrd.joinable()) {
      refreshThrd.join();
      networkTestThrd.join();
    }

    for (auto &panel : panels) {
      delwin(panel.window);
      delete panel.panel;
    }

    // log stats to file if flag is present
    if (argc > 1) {
      std::string arg = argv[1];
      if (arg == "--log") {
        std::ofstream stats("netmonlog.txt");
        stats << "avg latency: " << netstats::getStats().latency << "\n";
        stats << "sent packets: " << netstats::getStats().sentPackets << "\n";
        stats << "% lost packets: " << netstats::getStats().lostPacketPcnt
              << "\n";
        stats << "jitter: " << netstats::getStats().jitter << "\n";
        stats << "download speed: " << netstats::getStats().downloadSpeed.value
              << "mbps\n";
      }
    }

    endwin(); /* End curses mode		  */
  }

  void makePanels(int y, int x) {
    // header
    WINDOW *headerWin = newwin(HEADER_HEIGHT, x, 0, 0);
    PanelPrinter *headerPtr = new Header();

    // strength panel
    WINDOW *strengthWin =
        newwin(MAIN_HEIGHT, MAIN_PANEL_WIDTH, HEADER_HEIGHT + 1,
               x / 2 - MAIN_PANEL_WIDTH - 1);
    PanelPrinter *strengthPanelPtr = new StrPanel;

    // speed panel
    WINDOW *speedWin =
        newwin(MAIN_HEIGHT, MAIN_PANEL_WIDTH, HEADER_HEIGHT + 1, x / 2 + 1);
    PanelPrinter *speedPanelPtr = new SpdPanel;

    // footer
    WINDOW *footerWin = newwin(FOOTER_HEIGHT, x, y - FOOTER_HEIGHT - 1, 0);
    PanelPrinter *footerPtr = new Footer;

    panels[0] = {headerWin, headerPtr, HEADER_HEIGHT, x};
    panels[1] = {strengthWin, strengthPanelPtr, MAIN_HEIGHT, MAIN_PANEL_WIDTH};
    panels[2] = {speedWin, speedPanelPtr, MAIN_HEIGHT, MAIN_PANEL_WIDTH};
    panels[3] = {footerWin, footerPtr, FOOTER_HEIGHT, x};

    for (auto &panel : panels) {
      panel.panel->print(panel.window, panel.y, panel.x);
    }
  }

  void initColors() {
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);  // good
    init_pair(2, COLOR_YELLOW, COLOR_BLACK); // warning
    init_pair(3, COLOR_RED, COLOR_BLACK);    // critical
    init_pair(4, COLOR_CYAN, COLOR_BLACK);   // header
    init_pair(5, COLOR_WHITE, COLOR_BLACK);  // title bar
  }
};

int main(int argc, char *argv[]) {
  NetMon monitor;
  monitor.run(argc, argv);
  return 0;
}
