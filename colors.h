#ifndef COLORS_H
#define COLORS_H

/* kolorki */

#define BLACK 1
#define GRAY 2
#define WHITE 3
#define RED 4
#define GREEN 5
#define BLUE 6
#define CYAN 7
#define VIOLET 8
#define ORANGE 9
#define LIGHT_RED 10
#define LIGHT_GREEN 11
#define LIGHT_BLUE 12
#define YELLOW 13
#define LIGHT_VIOLET 14
#define LIGHT_CYAN 15
#define DEFAULT 16

/* Proste funkcje zapewniające przenośność kodu
 * pomiędzy platformami Win32/Unix */

void textcolor(int c);

#endif 
