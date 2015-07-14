#include "colors.h"

#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>

extern WINDOW* msg_window;

void textcolor(int c) {

	switch(c) {
		case BLACK:
		wattrset(msg_window, COLOR_PAIR(c));
		break;
		case WHITE:
		wattrset(msg_window, COLOR_PAIR(c) | A_BOLD);
		break;
		case GRAY:
		wattrset(msg_window, COLOR_PAIR(c));
		break;
		case RED:
		wattrset(msg_window, COLOR_PAIR(c));
		break;
		case GREEN:
		wattrset(msg_window, COLOR_PAIR(c));
		break;
		case BLUE:
		wattrset(msg_window, COLOR_PAIR(c));
		break;
		case VIOLET:
		wattrset(msg_window, COLOR_PAIR(c));
		break;
		case CYAN:
		wattrset(msg_window, COLOR_PAIR(c));
		break;
		case ORANGE:
		wattrset(msg_window, COLOR_PAIR(c));
		break;
		case LIGHT_RED:
		wattrset(msg_window, COLOR_PAIR(c) | A_BOLD);
		break;
		case LIGHT_GREEN:
		wattrset(msg_window, COLOR_PAIR(c) | A_BOLD);
		break;
		case LIGHT_BLUE:
		wattrset(msg_window, COLOR_PAIR(c) | A_BOLD);
		break;
		case YELLOW:
		wattrset(msg_window, COLOR_PAIR(c) | A_BOLD);
		break;
		case LIGHT_VIOLET:
		wattrset(msg_window, COLOR_PAIR(c) | A_BOLD);
		break;
		case LIGHT_CYAN:
		wattrset(msg_window, COLOR_PAIR(c) | A_BOLD);
		break;
		case DEFAULT:
		wattrset(msg_window, A_NORMAL);
		default:
		break;
	}
	
}
