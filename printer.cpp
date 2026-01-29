#include "printer.h"

PanelPrinter::PanelPrinter() {}

void PanelPrinter::printText(WINDOW* window, int height, int width) {
	this->window = window;
	y = height;
	x = width;	
}
