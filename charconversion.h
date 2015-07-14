#ifndef CHARCONVERSION_H
#define CHARCONVERSION_H

char* windows1250_to_utf8(const char* str);

char* utf8_to_windows1250(const char* str);

/* Zlicz dwuznaki UTF-8 */
int count_utf8_specials(const char* str);

#endif
