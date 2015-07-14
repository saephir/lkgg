#ifndef GGUTILS_H
#define GGUTILS_H

#include "contacts.h"

typedef struct gg_status {

	int typ;
	char opis[256];

} gg_status;

/* Próbujemy zalogować się na serwer */
int gg_login(int gg_number, unsigned char* password);

/* Wysyłamy wiadomość */
void send_message(int socket, int gg_number, const char* msg);

/* Wysyłamy wiadomość, że żyjemy */
void send_ping(int socket);

/* Wysyłamy listę kontaktów do serwera */
void send_contact_list(int socket, contact* lista);

/* Sprawdzamy status danej osoby */
void check_status(int socket, int gg_number);

/* Ustawiamy się na niedostępny */
void set_offline(int socket);

#endif
