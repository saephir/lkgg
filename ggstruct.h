#ifndef GGSTRUCT_H
#define GGSTRUCT_H

/* Statusy GG */

#define GG_STATUS_NOT_AVAIL 0x0001
#define GG_STATUS_NOT_AVAIL_DESCR 0x0015
#define GG_STATUS_AVAIL 0x0002
#define GG_STATUS_AVAIL_DESCR 0x0004
#define GG_STATUS_BUSY	0x0003
#define GG_STATUS_BUSY_DESCR 0x0005
#define GG_STATUS_INVISIBLE 0x0014
#define GG_STATUS_INVISIBLE_DESCR 0x0016
#define GG_STATUS_BLOCKED 0x0006

#define GG_PING 0x0008

typedef struct gg_header {
	int type;	/* typ pakietu */
	int length;	/* długość reszty pakietu */
} gg_header;

#define GG_WELCOME 0x0001

struct gg_welcome {
	int seed;	/* ziarno */
};

#define GG_LOGIN60 0x0015

typedef struct gg_login60 {
	int uin;              /* mój numerek */
        int hash;             /* hash hasła */
        int status;           /* status na dzień dobry */
        int version;          /* moja wersja klienta */
        char unknown1;        /* 0x00 */
        int local_ip;         /* mój adres ip */
	short local_port;     /* port, na którym słucham */
        int external_ip;      /* zewnętrzny adres ip */
        short external_port;  /* zewnętrzny port */
	char image_size;      /* maksymalny rozmiar grafiki w KB */
        char unknown2;        /* 0xbe */
	char description[];   /* opis, nie musi wystąpić */
	//int time;             /* czas, nie musi wystąpić */
} gg_login60;

#define GG_LOGIN_OK 0x0003
#define GG_NEED_EMAIL 0x14
#define GG_LOGIN_FAILED 0x0009

#define GG_NOTIFY_FIRST 0x000f
#define GG_NOTIFY_LAST 0x0010

#define GG_LIST_EMPTY 0x0012

#define GG_USER_NORMAL 0x01

typedef struct gg_notify {
	int uin __attribute__ ((packed));	/* numerek danej osoby */
	char type __attribute__ ((packed));	/* rodzaj użytkownika */
} gg_notify;


#define GG_ADD_NOTIFY 0x000d
#define GG_REMOVE_NOTIFY 0x000e

#define GG_NOTIFY_REPLY 0x000c
	
typedef struct gg_notify_reply {
	int uin;		/* numer */
	char status;		/* status danej osoby */
	int remote_ip;		/* adres ip delikwenta */
	short remote_port;	/* port, na którym słucha klient */
	int version;		/* wersja klienta */
	short unknown1;		/* znowu port? */
	char description[];	/* opis, nie musi wystąpić */
} gg_notify_reply;
	
#define GG_NOTIFY_REPLY60 0x0011
	
typedef struct gg_notify_reply60 {
	int uin __attribute__ ((packed));		/* numerek plus flagi w najstarszym bajcie */
	char status __attribute__ ((packed));		/* status danej osoby */
	int remote_ip __attribute__ ((packed));		/* adres IP bezpośrednich połączeń */
	short remote_port __attribute__ ((packed));	/* port bezpośrednich połączeń */
	char version __attribute__ ((packed));		/* wersja klienta */
	char image_size __attribute__ ((packed));	/* maksymalny rozmiar obrazków w KB */
	char unknown1 __attribute__ ((packed));		/* 0x00 */
	char description_size __attribute__ ((packed));	/* rozmiar opisu i czasu, nie musi wystąpić */
	char description[] __attribute__ ((packed));	/* opis, nie musi wystąpić */
} gg_notify_reply60;

#define GG_STATUS60 0x000f
	
typedef struct gg_status60 {
	int uin __attribute__ ((packed));	        /* numer plus flagi w najstarszym bajcie */
	char status __attribute__ ((packed));	        /* nowy stan */
	int remote_ip __attribute__ ((packed));		/* adres IP bezpośrednich połączeń */
	short remote_port __attribute__ ((packed));	/* port bezpośrednich połączeń */
	char version __attribute__ ((packed));		/* wersja klienta */
	char image_size __attribute__ ((packed));	/* maksymalny rozmiar grafiki */
	char unknown1 __attribute__ ((packed));		/* 0x00 */
	char description[] __attribute__ ((packed));	/* opis, nie musi wystąpić */
} gg_status60;

#define GG_NEW_STATUS 0x0002
	
typedef struct gg_new_status {
	int status __attribute__ ((packed));		/* na jaki zmienić? */
	char description[];	/* opis, nie musi wystąpić */
} gg_new_status;

#define GG_SEND_MSG 0x000b
#define GG_CLASS_MSG 0x0004
#define GG_CLASS_CHAT 0x0008

typedef struct gg_send_msg {
	int recipient __attribute__ ((packed));		/* numer odbiorcy */
	int seq __attribute__ ((packed));		/* numer sekwencyjny */
	int class __attribute__ ((packed));		/* klasa wiadomości */
	char message[] __attribute__ ((packed));		/* treść */
} gg_send_msg;

#define GG_RECV_MSG 0x000a

#define GG_CLASS_QUEUED	0x0001

typedef struct gg_recv_msg {
	int sender __attribute__ ((packed));		/* numer nadawcy */
	int seq __attribute__ ((packed));		/* numer sekwencyjny */
	int time __attribute__ ((packed));		/* czas nadania */
	int class __attribute__ ((packed));		/* klasa wiadomości */
	char message[] __attribute__ ((packed));		/* treść wiadomości */
} gg_recv_msg;

#define GG_SEND_MSG_ACK 0x0005
	
typedef struct gg_send_msg_ack {
	int status;	/* stan wiadomości */
	int recipient;	/* numer odbiorcy */
	int seq;	/* numer sekwencyjny */
} gg_send_msg_ack;

#define GG_ACK_BLOCKED 0x0001

#define GG_ACK_DELIVERED 0x0002

#define GG_ACK_QUEUED 0x0003

#define GG_ACK_MBOXFULL	0x0004

#define GG_ACK_NOT_DELIVERED 0x0006

#endif
