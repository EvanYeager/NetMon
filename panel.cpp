#include "panel.h"

void Header::printText(WINDOW* window, int height, int width) {
    PanelPrinter::printText(window, height, width);

    wbkgd(window, COLOR_PAIR(5));
	box(window, 0, 0);
	wattron(window, WA_BOLD | WA_UNDERLINE);
	mvwprintw(window, 1, (x - 25) / 2, "NETWORK STRENGTH MONITOR");
	wattroff(window, WA_BOLD | WA_UNDERLINE);
	wrefresh(window);
}

void SpdPanel::printText(WINDOW* window, int height, int width) {
    PanelPrinter::printText(window, height, width);

    box(window, 0, 0);
 	wattron(window, WA_BOLD);
 	mvwprintw(window, 0, 2, "Network Speed");
 	wattroff(window, WA_BOLD);
 	wrefresh(window);
}

void StrPanel::printText(WINDOW* window, int height, int width) {
    PanelPrinter::printText(window, height, width);

	box(window, 0, 0);
   	wattron(window, WA_BOLD);
   	mvwprintw(window, 0, 2, "Network Strength");
   	wattroff(window, WA_BOLD);
   	wrefresh(window);
}

Footer::Footer() {
	controls = {
		makeFooterItem(std::string("Controls: [Q]uit | [R]efresh | [P]ause"))
	};

	jitter = {
		makeFooterItem(std::string("Jitter: ")),
		makeFooterItem(std::string("<10ms"), 1),
		makeFooterItem(std::string(" | ")),
		makeFooterItem(std::string("<30ms"), 2),
		makeFooterItem(std::string(" | ")),
		makeFooterItem(std::string(">30ms"), 3),
	};

	speed = {
		makeFooterItem(std::string("Speed:  ")),
		makeFooterItem(std::string(">400mbps"), 1),
		makeFooterItem(std::string(" | ")),
		makeFooterItem(std::string(">50mbps"), 2),
		makeFooterItem(std::string(" | ")),
		makeFooterItem(std::string("<50mbps"), 3),
	};
}

void Footer::printText(WINDOW* window, int height, int width) {
	PanelPrinter::printText(window, height, width);

	box(window, 0, 0);

	// controls
	printRow(controls, 1, 1);

	// jitter
	printRow(jitter, 1, x - 2 - 36); // right justified

	// speed
	printRow(speed, 2, x - 2 - 36); // right justified

	wrefresh(window);
}


FooterItem Footer::makeFooterItem(std::string text, int colorNum, bool bold) {
	FooterItem item;
	item.text = text;
	item.colorNum = colorNum;
	item.bold = bold;
	return item;
}
