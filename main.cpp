#include <ncurses.h>

#include <iostream>

const int HEADER_HEIGHT = 4;
const int MAIN_HEIGHT = 7;
const int MAIN_PANEL_MIN_WIDTH = 30;
const int FOOTER_HEIGHT = 6;

WINDOW* header;
WINDOW* strength;
WINDOW* speed;
WINDOW* footer;

bool paused = false;

void initColors() {
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);		// good
	init_pair(2, COLOR_YELLOW, COLOR_BLACK);	// warning
	init_pair(3, COLOR_RED, COLOR_BLACK);		// critical
	init_pair(4, COLOR_CYAN, COLOR_BLACK);		// header
	init_pair(5, COLOR_WHITE, COLOR_BLACK);		// title bar
}

void makePanels(int y, int x) {
	// header
	header = newwin(HEADER_HEIGHT, x, 0, 0);
	wbkgd(header, COLOR_PAIR(5));
	box(header, 0, 0);
	wattron(header, WA_BOLD | WA_UNDERLINE);
	mvwprintw(header, 1, (x - 25) /2, "NETWORK STRENGTH MONITOR");
	wattroff(header, WA_BOLD | WA_UNDERLINE);
	wrefresh(header);

	// strength panel
 	strength = newwin(MAIN_HEIGHT, MAIN_PANEL_MIN_WIDTH, HEADER_HEIGHT + 1, 0);
 	box(strength, 0, 0);
 	wattron(strength, WA_BOLD);
 	mvwprintw(strength, 0, 2, "Network Strength");
 	wattroff(strength, WA_BOLD);
 	wrefresh(strength);
 
 	// speed panel
 	speed = newwin(MAIN_HEIGHT, MAIN_PANEL_MIN_WIDTH, HEADER_HEIGHT + 1, MAIN_PANEL_MIN_WIDTH + 3);
 	box(speed, 0, 0);
 	wattron(speed, WA_BOLD);
 	mvwprintw(speed, 0, 2, "Network Speed");
 	wattroff(speed, WA_BOLD);
 	wrefresh(speed);	
 
 	// footer
 	footer = newwin(FOOTER_HEIGHT, x, y - FOOTER_HEIGHT - 1, 0);
 	box(footer, 0, 0);
 	wattron(footer, WA_BOLD | WA_UNDERLINE);
 	// mvwprintw(footer, 1, (x - 8) / 2, "CONTROLS");''
 	wattroff(footer, WA_UNDERLINE);
 	mvwprintw(footer, 1, 2, "Controls: [Q]uit | [R]efresh | [P]ause");
 	wattroff(footer, WA_BOLD);
 	wrefresh(footer); 
}


int main() {

	// int ch;

	initscr();			/* Start curses mode 		*/
	raw();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */
	curs_set(0);

	initColors();

	refresh();

	int y, x;
	getmaxyx(stdscr, y, x);		/* get the number of rows and columns */


	makePanels(y, x);
	

	while (true) {
		int ch = getch();

		if (ch == 'q' || ch == 'Q') {
			break;
		}
		if (ch == 'p' || ch == 'P') {
			paused = !paused;
		}
	}

   	delwin(header);
   	delwin(strength);
   	delwin(speed);
   	delwin(footer);
	endwin();			/* End curses mode		  */

	return 0;
}

