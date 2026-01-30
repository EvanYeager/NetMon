#pragma once
#include <ncurses.h>
#include "printer.h"

class Bar : public PanelPrinter {
private:
	int y;
	int x;
	float value;
	float max;
	int min;

public:
	Bar(int y, int x, float value, float max, int min = 0);

	void print(WINDOW* window, int height, int width) override;

	void setBarValue(int value);
};
