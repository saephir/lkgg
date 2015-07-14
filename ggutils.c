#include "ggutils.h"
#include "ggstruct.h"
#include "charconversion.h"
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

/* Proste hashowanie hasła */
static int gg_login_hash(unsigned char *password, unsigned int seed)
{
	unsigned int x, y, z;

	y = seed;

	for (x = 0; *password; password++) {
		x = (x & 0xffffff00) | *password;
		y ^= x;
		y += x;
		x <<= 8;
		y ^= x;
		x <<= 8;
		y -= x;
		x <<= 8;
		y ^= x;

		z = y & 0x1f;
		y = (y << z) | (y >> (32 - z));
	}

	return y;
}

int gg_login(int gg_number, unsigned char* password) {

	/* port i adres serwera GG */
	const unsigned int port = 8074;
	const char* addr = "91.214.237.2";

	gg_login60 loginstruct;
	gg_header header;
	struct sockaddr_in adres;
	char buffer[1024];
	int hash;
	int seed;
	int sock;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	adres.sin_family = AF_INET;
	adres.sin_port = htons(port);
	adres.sin_addr.s_addr = inet_addr(addr);

	if(connect(sock, (struct sockaddr*)&adres, sizeof(adres)) < 0)
		return -1;

	if(recv(sock, buffer, sizeof(buffer), 0) < 0)
		return -1;

	seed = ((struct gg_welcome*)&buffer[sizeof(struct gg_header)])->seed;

	/* operacja samego logowania */

	memset(&loginstruct, 0, sizeof(loginstruct));
	memset(&header, 0, sizeof(header));

	hash = gg_login_hash(password, seed);

	header.type = GG_LOGIN60;
	header.length = sizeof(loginstruct);

	loginstruct.uin = gg_number;
	loginstruct.hash = hash;
	loginstruct.status = GG_STATUS_AVAIL;
	loginstruct.version = 0x20;
	loginstruct.unknown1 = 0x00;
	loginstruct.local_ip = 0;
	loginstruct.unknown2 = 0;

	send(sock, (char*)&header, sizeof(header), 0);

	send(sock, (char*)&loginstruct, sizeof(loginstruct), 0);

	recv(sock, buffer, sizeof(buffer), 0);

	memcpy((char*)&header, buffer, sizeof(gg_header));

	/* Logowanie udane */
	if(header.type == GG_LOGIN_OK) {
		return sock;
	} /* Logowanie nieudane */
	else {
		close(sock);
		return -1;
	}

}

void send_contact_list(int socket, contact* lista) {

	gg_header header;
	gg_notify notify;

	header.type = GG_LIST_EMPTY;
	header.length = 0;

	socket_lock();

	send(socket, (char*)&header, sizeof(gg_header), 0);

	socket_unlock();

	header.length = sizeof(notify);


	if(lista == 0)
		return;

	while(lista != 0) {

		header.type = GG_ADD_NOTIFY;	

		notify.uin = lista->gg_number;
		notify.type = GG_USER_NORMAL;

		socket_lock();

		send(socket, (char*)&header, sizeof(gg_header), 0);

		send(socket, (char*)&notify, sizeof(notify), 0);

		socket_unlock();

		lista = next_contact(lista);

	}


}

void check_status(int sock, int gg_number) {

	gg_header header;
	gg_notify notify;

		
	header.type = GG_ADD_NOTIFY;
	header.length = sizeof(notify);

	notify.uin = gg_number;
	notify.type = GG_USER_NORMAL;

	socket_lock();
	
	send(sock, (char*)&header, sizeof(header), 0);

	send(sock, (char*)&notify, sizeof(notify), 0);

	socket_unlock();

}

/* Wiadomość "ping" */
void send_ping(int sock) {

	gg_header header;
	
	header.type = GG_PING;
	header.length = 0;

	socket_lock();

	send(sock, (char*)&header, sizeof(header), 0);

	socket_unlock();

}

/* Wysyłamy wiadomość */
void send_message(int sock, int gg_number, const char* text) {

	gg_header header;
	gg_send_msg msg;
	char* windows1250;

	header.type = GG_SEND_MSG;

	windows1250 = utf8_to_windows1250(text);

	header.length = sizeof(msg)+strlen(windows1250)+1;

	msg.recipient = gg_number;
	msg.seq = 666;
	msg.class = 0x08;

	socket_lock();

	send(sock, (char*)&header, sizeof(header), 0);
	send(sock, (char*)&msg, sizeof(msg), 0);

	send(sock, windows1250, strlen(windows1250)+1, 0);

	socket_unlock();

	free(windows1250);

}

/* Przesyłamy pakiet przełączenia na niedostępny */
void set_offline(int socket) {

	gg_header header;
	gg_new_status status;

	header.type = GG_NEW_STATUS;
	header.length = sizeof(status);

	status.status = GG_STATUS_BUSY;
	status.description[0] = '\0';

	socket_lock();

	send(socket, (char*)&header, sizeof(header), 0);
	send(socket, (char*)&status, header.length, 0);

	socket_unlock();

}
