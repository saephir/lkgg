#include "err.h"
#include "ggstruct.h"
#include "ggutils.h"
#include "loop.h"
#include "contacts.h"
#include "colors.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ncurses.h>
#include <locale.h>

/* trochę globalnego syfu */
int rozmowca = 0;
struct contact* lista_kontaktow = empty_list();
WINDOW* msg_window;
FILE* log_file;

/* Wykonaj stosowne czynności */
/* sock - połączenie GG */
/* cmd - polecenie zakończone NULL'em */
static void parse_command(int sock, const char* cmd) {

	/* komendy zaczynają się od wykrzyknika jak w 'vi' */
	if(cmd[0] == '!') {

		char buffer[50];
		char reszta[500];
		int i, index;

		index = -1;

		for(i=0;i<strlen(cmd+1);i++) {
			if(*(cmd+1+i) == ' ') {
				index = i;
				break;
			}
		}

		/* wyczyść bufory */
		memset(buffer, 0, 50);
		memset(reszta, 0, 500);

		if(index != -1) {
			strncpy(buffer, cmd+1, index);
			strcpy(reszta, cmd+2+index);
		}
		else {
			strcpy(buffer, cmd+1);
		}
	
		/* zmiana rozmówcy */
		if(strcmp(buffer, "talk") == 0) {

			int numerek;
			contact* kontakt;

			if(sscanf(reszta, "%d", &numerek) > 0) {
				
				rozmowca = numerek;
				
				stdio_lock();
					textcolor(LIGHT_VIOLET);
					wprintw(msg_window, "piszesz teraz do: %d\n", rozmowca);
					textcolor(DEFAULT);
				stdio_unlock();

			}
			else {
				kontakt = find_person_by_name(reszta, lista_kontaktow);
				
				if(kontakt != 0)
					rozmowca = kontakt->gg_number;

				kontakt = find_person_by_number(rozmowca, lista_kontaktow);

				stdio_lock();
					textcolor(LIGHT_VIOLET);
					if(kontakt == 0)
						wprintw(msg_window, "piszesz teraz do: %d\n", rozmowca);
					else
						wprintw(msg_window, "piszesz teraz do: %s\n", kontakt->description);
					textcolor(DEFAULT);
				stdio_unlock();
				
			}

		}
		/* lista kontaktów */
		else if(strcmp(buffer, "list") == 0) {

			contact* buf = lista_kontaktow;

			stdio_lock();
				wprintw(msg_window, "Lista kontaktów:\n");
			stdio_unlock();

			while(buf != 0) {

				char statusChar;
				int statusColor;

				if(buf->status == GG_DOSTEPNY) {
					statusChar = '*';
					statusColor = LIGHT_GREEN;
				}
				else if(buf->status == GG_ZARAZWRACAM) {
					statusChar = '*';
					statusColor = LIGHT_BLUE;
				}
				else if(buf->status == GG_NIEWIDOCZNY) {
					statusChar = '*';
					statusColor = WHITE;
				}
				else if(buf->status == GG_NIEDOSTEPNY) {
					statusChar = '*';
					statusColor = GRAY;
				}
				else {
					statusChar = '?';
					statusColor = LIGHT_RED;
				}

				/* nie wyświetlamy niedostępnych */
				if(buf->status != GG_NIEDOSTEPNY) {

					stdio_lock();
						wprintw(msg_window, "%s [", buf->description);
						textcolor(statusColor);
						wprintw(msg_window, "%c", statusChar);
						textcolor(DEFAULT);
						wprintw(msg_window, "]\n");
					stdio_unlock();

				}

				buf = next_contact(buf);

			}

		}
		else if(strcmp(buffer, "clear") == 0) {

			stdio_lock();
				wclear(msg_window);
				wrefresh(msg_window);
			stdio_unlock();

		}

	}	
	
	/* zwykła wiadomość */
	else {

		if(rozmowca != 0) {

			contact* finder = find_person_by_number(rozmowca, lista_kontaktow);

			send_message(sock, rozmowca, cmd);

			stdio_lock();

				textcolor(LIGHT_BLUE);

				if(finder != 0) {
					wprintw(msg_window, ">>%s\n", finder->description);
					fprintf(log_file, ">>%s\n", finder->description);
				}
				else {
					wprintw(msg_window, ">>%d\n", rozmowca);
					fprintf(log_file, ">>%d\n", rozmowca);
				}

				textcolor(DEFAULT);			

				wprintw(msg_window, "%s\n", cmd);
				fprintf(log_file, "%s\n", cmd);

				fflush(log_file);

			stdio_unlock();

		}
		else {
			stdio_lock();
				wprintw(msg_window, "[Nie zdefiniowano rozmówcy. Wpisz: !talk NUMER_LUB_NAZWA]\n");
			stdio_unlock();
		}

	}

}

/* Ekran prezentacyjny programu */
void entry_screen() {

	wprintw(msg_window, "==============================\n");
	wprintw(msg_window, "======== lkgg ver 0.1 ========\n");
	wprintw(msg_window, "== linuksowy komunikator gg ==\n");
	wprintw(msg_window, "======= Saephir, 2011. =======\n");
	wprintw(msg_window, "==============================\n");

}

int main(int argn, char** argv) {

	pthread_t loop_thread;
	pthread_t ping_thread;
	int sock;
	char buffer[1024];
	char input[500];
	unsigned int seed;
	int result;
	int request;
	gg_status request_result;
	int ret;
	WINDOW* input_wnd;
	int login;
	char haslo[100];
	FILE* conf_file;

	setlocale(LC_CTYPE, "en_US.UTF-8");

	conf_file = fopen("lkgg.conf", "r");

	if(!conf_file)
		syserr("Błąd odczytu pliku konfiguracyjnego lkgg.conf! (0)");

	if(fscanf(conf_file, "%d", &login) <= 0)
		syserr("Błąd odczytu pliku konfiguracyjnego lkgg.conf! (1)");

	if(fscanf(conf_file, "%99s", haslo) <= 0)
		syserr("Błąd odczytu pliku konfiguracyjnego lkgg.conf! (2)");

	fclose(conf_file);
	
	/* inicjalizacja ncurses */
	initscr();
	start_color();
	use_default_colors();

	init_pair(BLACK, COLOR_BLACK, -1);
	init_pair(WHITE, COLOR_WHITE, -1);
	init_pair(GRAY, COLOR_WHITE, -1);
		
	init_pair(RED, COLOR_RED, -1);
	init_pair(LIGHT_RED, COLOR_RED, -1);
		
	init_pair(GREEN, COLOR_GREEN, -1);
	init_pair(LIGHT_GREEN, COLOR_GREEN, -1);

	init_pair(BLUE, COLOR_BLUE, -1);
	init_pair(LIGHT_BLUE, COLOR_BLUE, -1);

	init_pair(VIOLET, COLOR_MAGENTA, -1);
	init_pair(LIGHT_VIOLET, COLOR_MAGENTA, -1);

	init_pair(CYAN, COLOR_CYAN, -1);
	init_pair(LIGHT_CYAN, COLOR_CYAN, -1);

	init_pair(ORANGE, COLOR_YELLOW, -1);
	init_pair(YELLOW, COLOR_YELLOW, -1);

	cbreak();

	msg_window = newwin(LINES-3, COLS, 0, 0);
	scrollok(msg_window, TRUE);
	
	input_wnd = newwin(3, COLS, LINES-3, 0);

	keypad(msg_window, TRUE);
	keypad(input_wnd, TRUE);

	box(input_wnd, 0, 0);

	/* ekran powitalny */
	entry_screen();

	wmove(input_wnd, 1, 2);

	wrefresh(msg_window);
	wrefresh(input_wnd);

	log_file = fopen("log", "a");
	
	/* Pobieramy z pliku listę kontaktów */
	lista_kontaktow = read_list_from_file("contacts");
	
	if(lista_kontaktow) {
		textcolor(LIGHT_VIOLET);
		wprintw(msg_window, "załadowano listę kontaktów...\n");
	}
	else {
		textcolor(LIGHT_RED);
		wprintw(msg_window, "błąd ładowania listy kontaktów!\n");
	}

	textcolor(DEFAULT);
	wrefresh(msg_window);

	/* Próbujemy się logować */

	/* Inicjalizacja mutexu */
	locks_init();

	sock = gg_login(login, haslo);

	if(sock < 0) {

		textcolor(LIGHT_RED);
		wprintw(msg_window, "błąd logowania! naciśnij dowolny klawisz aby wyjść...\n");
		wprintw(msg_window, "sprawdź login i hasło w pliku lkgg.conf...\n");
		textcolor(DEFAULT);
		wrefresh(msg_window);
		wgetch(msg_window);
		close(sock);
		locks_cleanup();
		fflush(log_file);
		fclose(log_file);
		endwin();
		return 1;

	}

	textcolor(LIGHT_VIOLET);
	wprintw(msg_window, "zalogowano: %d\n", login);
	textcolor(DEFAULT);
	wrefresh(msg_window);

	/* Wysyłamy listę kontaktów */
	send_contact_list(sock, lista_kontaktow);

	/* Uruchamiamy pętlę odczytu komunikatów */
	pthread_create(&loop_thread, 0, loop_procedure, (void*)sock);

	/* Uruchamiamy pętlę ping */
	pthread_create(&ping_thread, 0, ping_procedure, (void*)sock);

	/* Nasłuchujemy komend */

	while(strncmp(input, "!q", 2) != 0) {

		memset(input, 0, 500);		
		wgetstr(input_wnd, input);
		wrefresh(input_wnd);
		wclear(input_wnd);
		box(input_wnd, 0, 0);
		wmove(input_wnd, 1, 2);

		parse_command(sock, input);
		wrefresh(msg_window);

	}

	set_offline(sock);

	pthread_cancel(loop_thread);
	pthread_cancel(ping_thread);
	
	close(sock);

	locks_cleanup();

	fflush(log_file);
	fclose(log_file);

	endwin();
	
	return 0;

}
