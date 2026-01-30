#pragma once
#include <ncurses.h>
#include <string>
#include <array>
#include "printer.h"

class Header : public PanelPrinter {
public:
    void print(WINDOW* window, int height, int width) override;
};

class SpdPanel : public PanelPrinter {
public:
    void print(WINDOW* window, int height, int width) override;
};

class StrPanel : public PanelPrinter {
public:
    void print(WINDOW* window, int height, int width) override;
};

struct FooterItem {
    std::string text;
	int colorNum = 0; // if 0, no color
	bool bold = true;
};

class Footer : public PanelPrinter {
public:
  Footer();
	void print(WINDOW* window, int height, int width) override;

private:
	template <typename T, std::size_t N>
	void printRow(std::array<T, N> items, int y, int x) {
		for (auto item : items) {
			if (item.colorNum != 0) wattron(window, COLOR_PAIR(item.colorNum));
			if (item.bold) wattron(window, WA_BOLD);

			mvwprintw(window, y, x, "%s", item.text.c_str());

			wattroff(window, COLOR_PAIR(item.colorNum));
			wattroff(window, WA_BOLD);
			x += item.text.length();
		}
	}

	FooterItem makeFooterItem(std::string text, int colorNum = 0, bool bold = true);

	// populated in the constructor
	std::array<FooterItem, 1> controls;
	std::array<FooterItem, 6> jitter;
	std::array<FooterItem, 6> speed;

};
