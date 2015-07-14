#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

/* Konwersja korzystająca z iconv */
char* call_iconv(const char* str, const char* from, const char* to) {

	char* buffer;
	int buffersize;
	int rurka_in[2];
	int rurka_out[2];
	int forkval;
	char param1[100];
	char param2[100];

	strcpy(param1, "-f ");
	strcpy(param2, "-t ");
	strcat(param1, from);
	strcat(param2, to);

	buffersize = strlen(str)*2+1;
	buffer = (char*)malloc(buffersize);
	memset(buffer, buffersize, 0);

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
		execlp("iconv", param2, (char*)NULL);
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

int main() {

	char* text = call_iconv("zażółć gęślą jaźń",
		"utf-8", "windows-1250");

	printf("%s\n", text);

	char* text2 = call_iconv(text, "windows-1250", "utf-8");

	printf("%s\n", text2);
	return 0;

}
