#include <ncurses.h>

#include "panel.h"
#include <array>
#include <iostream>

struct PanelStruct {
  WINDOW* window;
  PanelPrinter* panel;
  int y;
  int x;
};

class NetMon {
  const int HEADER_HEIGHT = 4;
  const int MAIN_HEIGHT = 11;
  const int MAIN_PANEL_WIDTH = 40;
  const int FOOTER_HEIGHT = 6;

  std::array<PanelStruct, 4> panels{};

  bool paused = false;

public:
  void run() {

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

    while (true) {
      int ch = getch();

      if (ch == 'q' || ch == 'Q') {
        break;
      }
      if (ch == 'p' || ch == 'P') {
        paused = !paused;
      }
    }

    for (auto &panel : panels) {
      delwin(panel.window);
      delete panel.panel;
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

int main() {
  NetMon monitor;
  monitor.run();
  return 0;
}
