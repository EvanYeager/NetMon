#include "bar.h"
#include <iostream>
#include <ncurses.h>

Bar::Bar(int y, int x, const float& trackedValue, float max, std::string label, int min)
  : y(y), x(x), value(trackedValue), max(max), label(label), min(min) {}

void Bar::print(WINDOW *window, int height, int width) {
  PanelPrinter::print(window, height, width);

  float percent = (value - min) / (float)max;
  int filledWidth = (int)(percent * width);

  // printing bar
  wattron(window, COLOR_PAIR(1));
  mvwhline(window, y, x, ACS_CKBOARD, filledWidth);
  wattroff(window, COLOR_PAIR(1));
  mvwhline(window, y, x + filledWidth, '-', width - filledWidth);

  // print the text to the right of the bar
  if (showValue && !label.empty()) { // showing the value and label
    mvwprintw(window, y, x + width + 2, "%.1f %s", value, label.c_str());
  } else if (showValue) { // show the value without any label
    mvwprintw(window, y, x + width + 2, "%.1f", value);
  } else if (!label.empty()) { // show a label without any value
    mvwprintw(window, y, x + width + 2, "%s", label.c_str());
  }

  wrefresh(window);
}

void Bar::setLabel(std::string label) { this->label = label; }

void Bar::setShowValue(bool show) { this->showValue = show; }
