#ifndef LOOP_H
#define LOOP_H

/* Wątek wysyłający ping, gniazdo rzutowane na void* */
void* ping_procedure(void* v);

/* Przekazujemy numer gniazda rzutując na void*. */
void* loop_procedure(void* v);

#endif
