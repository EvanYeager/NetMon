#pragma once
#include <ncurses.h>

class PanelPrinter {
public:
	PanelPrinter();

	virtual void print(WINDOW* window, int height, int width);


protected:
	WINDOW* window;
	int height;
	int width;	
};
