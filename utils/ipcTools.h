#include <sys/types.h>




int semalloc(key_t key, int valInit);

void P(int semid);

void V(int semid);

int semfree(int semid);

int mutalloc(key_t key);

int mutfree(int semid);

void* shmalloc(key_t, int);

int shmfree(key_t);

int msgalloc(key_t);

int msgfree(int);

int msgsend(int, char*, int);

int msgrecv(int, char*, int);