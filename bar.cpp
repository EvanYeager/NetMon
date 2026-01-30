#include <ncurses.h>
#include <iostream>
#include "bar.h"

Bar::Bar(int y, int x, float value, float max, int min) {
	this->y = y;
	this->x = x;
	this->value = value;
	this->max = max;
	this->min = min;
}

void Bar::setBarValue(int value) { this->value = value; }


void Bar::print(WINDOW* window, int height, int width) {
	PanelPrinter::print(window, height, width);

	float percent = (value - min) / (float)max;
	int filledWidth= (int)(percent * width);

	// printing bar
	wattron(window, COLOR_PAIR(1));
	mvwhline(window, y, x, ACS_CKBOARD, filledWidth);
	wattroff(window, COLOR_PAIR(1));
	mvwhline(window, y, x + filledWidth, '-', width - filledWidth);

	// print the value to the right of the bar
	mvwprintw(window, y, x + width + 2, "%.1f", value);

	wrefresh(window);
}

