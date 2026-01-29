#pragma once
#include <ncurses.h>

class PanelPrinter {
public:
	PanelPrinter();

	// populate text in the panel
	virtual void printText(WINDOW* window, int height, int width);


protected:
	WINDOW* window;
	int y;
	int x;	
};
