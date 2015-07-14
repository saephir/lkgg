#ifndef CONTACTS_H
#define CONTACTS_H

/* Statusy */
#define GG_NIEDOSTEPNY 0
#define GG_NIEWIDOCZNY 1
#define GG_ZARAZWRACAM 2
#define GG_DOSTEPNY 3

/* Struktura listy kontaktów */
typedef struct contact {

	/* numer gg */
	int gg_number;
	/* maksymalnie 50 znaków */
	char description[51];
	/* status */
	int status;

	/* następny kontakt */
	struct contact* next;

} contact;

/* Tworzy nowy kontakt */
contact* make_contact(int gg_number, const char* desc);

/* Tworzy pustą listę kontaktów */
#define empty_list() ((contact*)0)

/* Dodaje nowy kontakt do listy */
void append(contact* c, contact** lista);

/* Pobiera lastępny kontakt z listy */
#define next_contact(list) (list->next)

/* Czyści listę */
void cleanup_list(contact** c);

/* Czyta listę z pliku */
contact* read_list_from_file(const char* filename);

/* Zapisuje listę do pliku */
void save_list_to_file(contact* lista, const char* filename);

/* Poszukuje ludzi po numerze GG i zwraca opis */
contact* find_person_by_number(int gg_number, contact* lista);

/* Poszukuje ludzi po opisie, zwraca numer GG lub -1 */
contact* find_person_by_name(const char* name, contact* lista);

#endif
