#include <ncurses.h>
#include <iostream>
#include <array>

struct FooterItem {
	std::string text;
	int colorNum = 0; // if 0, no color
	bool bold = true;
};

class Footer {
	public:
	WINDOW* footer;
	int height;
	int width;
	
	Footer(WINDOW* footer, int y, int x) {
		this->footer = footer;	
		height = y;
		width = x;
	}
	
	void makeFooter() {
		// controls
		printRow(controls, 1, 1);

		// jitter
		printRow(jitter, 1, width - 2 - 36); // right justified

		// speed
		printRow(speed, 2, width - 2 - 36); // right justified

		wrefresh(footer);
	}


	private:
	template <typename T, std::size_t N>
	void printRow(std::array<T, N> items, int y, int x) {
		for (auto item : items) {
			if (item.colorNum != 0) wattron(footer, COLOR_PAIR(item.colorNum));
			if (item.bold) wattron(footer, WA_BOLD);

			mvwprintw(footer, y, x, "%s", item.text.c_str());

			wattroff(footer, COLOR_PAIR(item.colorNum));
			wattroff(footer, WA_BOLD);
			x += item.text.length();
		}
	}

	std::array<FooterItem, 1> controls = {
		makeFooterItem(std::string("Controls: [Q]uit | [R]efresh | [P]ause"))
	};

	std::array<FooterItem, 6> jitter = {
		makeFooterItem(std::string("Jitter: ")),
		makeFooterItem(std::string("<10ms"), 1),
		makeFooterItem(std::string(" | ")),
		makeFooterItem(std::string("<30ms"), 2),
		makeFooterItem(std::string(" | ")),
		makeFooterItem(std::string(">30ms"), 3),
	};

	std::array<FooterItem, 6> speed = {
		makeFooterItem(std::string("Speed:  ")),
		makeFooterItem(std::string(">400mbps"), 1),
		makeFooterItem(std::string(" | ")),
		makeFooterItem(std::string(">50mbps"), 2),
		makeFooterItem(std::string(" | ")),
		makeFooterItem(std::string("<50mbps"), 3),
	};

	
	FooterItem makeFooterItem(std::string text, int colorNum = 0, bool bold = true) {
		FooterItem item;
		item.text = text;
		item.colorNum = colorNum;
		item.bold = bold;
		return item;
	}
};
