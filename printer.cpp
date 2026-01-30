#include "printer.h"

PanelPrinter::PanelPrinter() {}

void PanelPrinter::print(WINDOW* window, int height, int width) {
	this->window = window;
	y = height;
	x = width;	
}
