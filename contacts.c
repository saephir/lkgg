#include "contacts.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


contact* make_contact(int gg_number, const char* desc) {

	contact* c = (contact*)malloc(sizeof(contact));
	c->gg_number = gg_number;
	strncpy(c->description, desc, sizeof(c->description));
	c->description[sizeof(c->description)-1] = 0;

	c->status = GG_NIEDOSTEPNY;

	c->next = 0;

	return c;

}

void append(contact* c, contact** list) {

	contact* listval = *list;

	if(listval == 0) {
		*list = c;
	}
	else {
		while(listval->next != 0)
			listval = listval->next;

		listval->next = c;

	}

}

void cleanup_list(contact** list) {

	if(*list != 0) {

		contact* ptr = *list;

		while(ptr != 0) {

			contact* temp = ptr;
			ptr = ptr->next;
			free(temp);

		}

	}

}

contact* read_list_from_file(const char* filename) {

	const int BUFFER_SIZE = 100;
	char buffer[BUFFER_SIZE];
	int numer;
	FILE* plik;
	contact* lista;
	char* first_pipe;

	lista = empty_list();

	plik = fopen(filename, "r");

	if(plik <= 0)
		return lista;

	while(fgets(buffer, BUFFER_SIZE, plik) != 0) {

		/* usuń znak końca wiersza */
		buffer[strlen(buffer)-1] = '\0';

		first_pipe = strchr(buffer, '|');
		sscanf(buffer, "%d", &numer);
		append(make_contact(numer, first_pipe+1), &lista);

	}

	fclose(plik);

	return lista;

}

void save_list_to_file(contact* lista, const char* filename) {

	FILE* plik;

	plik = fopen(filename, "w+");

	while(lista != 0) {

		fprintf(plik, "%d|%s\n", lista->gg_number, lista->description); 
		lista = next_contact(lista);

	}

	fclose(plik);

}

contact* find_person_by_number(int gg_number, contact* lista) {

	while(lista != 0) {

		if(gg_number == lista->gg_number)
			break;

		lista = next_contact(lista);

	}

	return lista;

}

contact* find_person_by_name(const char* name, contact* lista) {

	while(lista != 0) {

		if(strcmp(name, lista->description) == 0)
			break;
	
		lista = next_contact(lista);

	}

	return lista;

}
