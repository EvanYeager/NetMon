#include <ncurses.h>
#include <iostream>
#include "bar.h"

Bar::Bar(int y, int x, float value, float max, std::string label, int min) {
	this->y = y;
	this->x = x;
	this->value = value;
	this->max = max;
	this->label = label;
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

	// print the text to the right of the bar
	if (showValue && !label.empty()) { // showing the value and label
		mvwprintw(window, y, x + width + 2, "%.1f %s", value, label.c_str());
	}
	else if (showValue) { // show the value without any label
		mvwprintw(window, y, x + width + 2, "%.1f", value);
	}
	else if (!label.empty()) { // show a label without any value
		mvwprintw(window, y, x + width + 2, "%s", label.c_str());
	}

	wrefresh(window);
}

void Bar::setLabel(std::string label) {
	this->label = label;

}

void Bar::setShowValue(bool show) {
	this->showValue = show;
}

