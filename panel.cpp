#include "panel.h"
#include "bar.h"
#include <unistd.h>

void Header::print(WINDOW* window, int height, int width) {
    PanelPrinter::print(window, height, width);

    wbkgd(window, COLOR_PAIR(5));
	box(window, 0, 0);
	wattron(window, WA_BOLD | WA_UNDERLINE);
	mvwprintw(window, 1, (width - 25) / 2, "NETWORK STRENGTH MONITOR");
	wattroff(window, WA_BOLD | WA_UNDERLINE);
	wrefresh(window);
}

void StrPanel::print(WINDOW* window, int height, int width) {
    PanelPrinter::print(window, height, width);

	box(window, 0, 0);
   	wattron(window, WA_BOLD);
   	mvwprintw(window, 0, 2, "Network Strength");
   	wattroff(window, WA_BOLD);
   	wrefresh(window);
}

void SpdPanel::print(WINDOW* window, int height, int width) {
    PanelPrinter::print(window, height, width);

    box(window, 0, 0);
 	wattron(window, WA_BOLD);
 	mvwprintw(window, 0, 2, "Network Speed");
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

void Footer::print(WINDOW* window, int height, int width) {
	PanelPrinter::print(window, height, width);

	box(window, 0, 0);

	// controls
	printRow(controls, 1, 1);

	// jitter
	printRow(jitter, 1, width - 2 - 36); // right justified

	// speed
	printRow(speed, 2, width - 2 - 36); // right justified

	wrefresh(window);
}


FooterItem Footer::makeFooterItem(std::string text, int colorNum, bool bold) {
	FooterItem item;
	item.text = text;
	item.colorNum = colorNum;
	item.bold = bold;
	return item;
}
