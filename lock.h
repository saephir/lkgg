#ifndef LOCK_H
#define LOCK_H

void locks_init(void);

void locks_cleanup(void);

void socket_lock(void);

void socket_unlock(void);

void stdio_lock(void);

void stdio_unlock(void);

#endif
