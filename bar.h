#pragma once
#include "printer.h"
#include <ncurses.h>
#include <string>

class Bar : public PanelPrinter {
private:
  int y;
  int x;
  const float& value;
  float max;
  std::string label;
  int min;
  bool showValue = true;

public:
  Bar(int y, int x, const float& trackedValue, float max, std::string label = "",
      int min = 0);

  void print(WINDOW *window, int height, int width) override;

  void setLabel(std::string label);
  void setShowValue(bool show);
};
