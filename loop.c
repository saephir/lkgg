#include "loop.h"
#include "ggstruct.h"
#include "lock.h"
#include "ggutils.h"
#include "charconversion.h"
#include "contacts.h"
#include "colors.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/select.h>
#include <ncurses.h>

extern int rozmowca;
extern struct contact* lista_kontaktow;
extern WINDOW* msg_window;
extern FILE* log_file;

void* ping_procedure(void* v) {

	int sock;

	sock = (int)v;

	while(1) {

		send_ping(sock);
		
		/* co 3 minuty */
		usleep(3*60*1000*1000);

	}

}

void* loop_procedure(void* v) {

	fd_set set;
	struct timeval val;
	char buffer[4096];
	int len, type;
	char* converted;

	gg_header* ptr;
	gg_recv_msg* ptr2;
	gg_notify_reply60* ptr3;
	gg_send_msg_ack* ptr4;
	gg_status60* ptr5;

	int flag = 0;
	int socket = (int)v;

	while(flag >= 0) {

		FD_ZERO(&set);
		FD_SET(socket, &set);

		val.tv_sec = 0;
		val.tv_usec = 0;

		select(socket+1, &set, 0, 0, &val);

		if(FD_ISSET(socket, &set)) {

			socket_lock();

			/* wyczyść bufor */
			memset(buffer, 0, sizeof(buffer));

			/* odbierz nagłówek */
			flag = recv(socket, buffer, sizeof(gg_header), 0);

			if(flag < 0)
				break;

			ptr = (gg_header*)&buffer;

			len = ptr->length;
			type = ptr->type;

			/* Pobierz resztę wiadomości */
			flag = recv(socket, buffer, len, 0);

			socket_unlock();

			if(flag < 0)
				break;

			/* Nie ma co zabierać się do pustych pakietów */
			if(len != 0) {
				if(type == GG_NOTIFY_REPLY60) {

					contact* finder;

					/* Wiadomość o statusie */
					ptr3 = (gg_notify_reply60*)&buffer;

					finder = find_person_by_number(ptr3->uin, lista_kontaktow);

					if(finder != 0) {
	
						if(ptr3->status == GG_STATUS_NOT_AVAIL)
							finder->status = GG_NIEDOSTEPNY;

						else if(ptr3->status == GG_STATUS_AVAIL)
							finder->status = GG_DOSTEPNY;

						else if(ptr3->status == GG_STATUS_BUSY)
							finder->status = GG_ZARAZWRACAM;

						else if(ptr3->status == GG_STATUS_INVISIBLE)
							finder->status = GG_NIEWIDOCZNY;

						else if(ptr3->status ==  GG_STATUS_NOT_AVAIL_DESCR)
							finder->status = GG_NIEDOSTEPNY;

						else if(ptr3->status ==  GG_STATUS_AVAIL_DESCR)
							finder->status = GG_DOSTEPNY;

						else if(ptr3->status ==  GG_STATUS_BUSY_DESCR)
							finder->status = GG_ZARAZWRACAM;

					}

				}
				else if(type == GG_RECV_MSG) {
				
					contact* finder;

					ptr2 = (gg_recv_msg*)&buffer;

					converted = windows1250_to_utf8(ptr2->message);

					finder = find_person_by_number(ptr2->sender, lista_kontaktow);

					stdio_lock();

						if(ptr2->sender == rozmowca)
							textcolor(LIGHT_GREEN);
						else
							textcolor(LIGHT_RED);

						if(finder != 0) {
							wprintw(msg_window, "<<%s", finder->description);
							fprintf(log_file, "<<%s", finder->description);
						}
						else {
							wprintw(msg_window, "<<%d", ptr2->sender);
							fprintf(log_file, "<<%d", ptr2->sender);
						}

						textcolor(DEFAULT);
						wprintw(msg_window, "\n%s\n", converted);
						fprintf(log_file, "\n%s\n", converted);
						fflush(log_file);

					stdio_unlock();

					wrefresh(msg_window);
											
					free(converted);


				}
				else if(type == GG_SEND_MSG_ACK) {

					/* Potwierdzenie wiadomości - sprawa kluczowa 
					ptr4 = (gg_send_msg_ack*)&buffer;

					wprintw(msg_window, "%d: ", ptr4->recipient);
					if(ptr4->status == GG_ACK_DELIVERED)
						wprintw(msg_window, "dotarło!\n");
					else
						wprintw(msg_window, "zakolejkowano!\n");*/

				}
				else if(type == GG_STATUS60) {

					contact* finder;

					ptr5 = (gg_status60*)&buffer;

					finder = find_person_by_number(ptr5->uin, lista_kontaktow);

					stdio_lock();
					
						textcolor(ORANGE);

						if(finder != 0)
							wprintw(msg_window, "%s", finder->description);
						else
							wprintw(msg_window, "%d", ptr5->uin);
	
						textcolor(DEFAULT);
					
						wprintw(msg_window, " zmienił(a) status na ");

						if(ptr5->status == GG_STATUS_NOT_AVAIL)
							wprintw(msg_window, "niedostępny\n");
						else if(ptr5->status == GG_STATUS_AVAIL)
							wprintw(msg_window, "dostępny\n");
						else if(ptr5->status == GG_STATUS_BUSY)
							wprintw(msg_window, "zaraz wracam\n");
						else if(ptr5->status == GG_STATUS_INVISIBLE)
							wprintw(msg_window, "niewidoczny\n");
						else if(ptr5->status ==  GG_STATUS_NOT_AVAIL_DESCR)
							wprintw(msg_window, "niedostępny z opisem: %s\n", ptr5->description);
						else if(ptr5->status ==  GG_STATUS_AVAIL_DESCR)
							wprintw(msg_window, "dostępny z opisem: %s\n", ptr5->description);
						else if(ptr5->status ==  GG_STATUS_BUSY_DESCR)
							wprintw(msg_window, "zaraz wracam z opisem: %s\n", ptr5->description);
						else
							wprintw(msg_window, "błąd pobrania statusu!\n");

					stdio_unlock();

					wrefresh(msg_window);

					if(finder != 0) {

						if(ptr5->status == GG_STATUS_NOT_AVAIL)
							finder->status = GG_NIEDOSTEPNY;
						
						else if(ptr5->status == GG_STATUS_AVAIL)
							finder->status = GG_DOSTEPNY;
						
						else if(ptr5->status == GG_STATUS_BUSY)
							finder->status = GG_ZARAZWRACAM;
						
						else if(ptr5->status == GG_STATUS_INVISIBLE)
							finder->status = GG_NIEWIDOCZNY;
						
						else if(ptr5->status ==  GG_STATUS_NOT_AVAIL_DESCR)
							finder->status = GG_NIEDOSTEPNY;			
	
						else if(ptr5->status ==  GG_STATUS_AVAIL_DESCR)
							finder->status = GG_DOSTEPNY;					
	
						else if(ptr5->status ==  GG_STATUS_BUSY_DESCR)
							finder->status = GG_ZARAZWRACAM;

					}


				}

			}

		}

		usleep(10000);

	}

	socket_unlock();

	stdio_lock();
		textcolor(LIGHT_VIOLET);
		wprintw(msg_window, "exit from loop...\n");
		textcolor(DEFAULT);
	stdio_unlock();

	pthread_exit(0);

}
