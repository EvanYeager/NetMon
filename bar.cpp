#include "bar.h"
#include <cmath>
#include <ncurses.h>

Bar::Bar(int y, int x, const float &trackedValue, const Scale *scale,
         std::string label)
    : y(y), x(x), value(trackedValue), scale(scale), label(label) {}

void Bar::print(WINDOW *window, int height, int width) {
  PanelPrinter::print(window, height, width);

  float percent = scale->normalize(value);
  // float percent = (scaledValue - min) / (float)max;
  int filledWidth = (int)(percent * width);

  int color;
  if (direction == colorDirection::normal) {
    color = percent * 100.0f < 30 ? 1 : percent * 100.0f < 70 ? 2 : 3;  
  }
  else {
    color = percent * 100.0f < 30 ? 3 : percent * 100.0f < 70 ? 2 : 1;
  }

  // printing bar
  wattron(window, COLOR_PAIR(color));
  mvwhline(window, y, x, ACS_CKBOARD, filledWidth); // filled portion
  wattroff(window, COLOR_PAIR(color));
  mvwhline(window, y, x + filledWidth, '-',
           width - filledWidth); // unfilled portion

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

void Bar::setColorDirection(colorDirection direction) { this->direction = direction; }
