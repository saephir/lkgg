#include "charconversion.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/* Korzystamy z obecności programu iconv w systemie */
/* używamy uniksowych pipe'ów do komunikacji. */
char* call_iconv(const char* str, const char* from, const char* to) {

	char* buffer;
	int buffersize;
	int rurka_in[2];
	int rurka_out[2];
	int forkval;
	char param1[100];
	char param2[100];
	char* ptrtable[4];
	const char* appname = "iconv";

	memset(param1, 0, 100);
	memset(param2, 0, 100);
	strcpy(param1, "-f ");
	strcpy(param2, "-t ");
	strcat(param1, from);
	strcat(param2, to);

	ptrtable[0] = appname;
	ptrtable[1] = param1;
	ptrtable[2] = param2;
	ptrtable[3] = NULL;

	buffersize = strlen(str)*2+1;
	buffer = (char*)malloc(buffersize);
	memset(buffer, 0, buffersize);

	pipe(rurka_in);
	pipe(rurka_out);

	forkval = fork();

	if(forkval == 0) {

		close(0);
		close(1);
		
		dup(rurka_in[0]);
		dup(rurka_out[1]);

		close(rurka_in[0]);
		close(rurka_in[1]);
		close(rurka_out[0]);
		close(rurka_out[1]);
		execvp("iconv", ptrtable);
		exit(0);
	}
	else {

		write(rurka_in[1], str, strlen(str));
		close(rurka_in[1]);

		read(rurka_out[0], buffer, buffersize);

		wait(0);

		close(rurka_in[0]);
		close(rurka_out[0]);
		close(rurka_out[1]);
	}

	return buffer;

}

/* Poniższe funkcje działają dla znaków ASCII
 * oraz zestawu polskich znaków. */

char* windows1250_to_utf8(const char* str) {

	return call_iconv(str, "windows-1250", "utf-8");

}

char* utf8_to_windows1250(const char* str) {

	return call_iconv(str, "utf-8", "cp1250");

}

int count_utf8_specials(const char* str) {

	int ret;

	char* conv;

	conv = utf8_to_windows1250(str);
	
	ret = strlen(str)-strlen(conv);

	return ret;

}
