
#include "sem.h"

#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>

static sem_t *sem;

void initSem(int host) {

    if (host) {
        sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);
        int val;

        sem_getvalue(sem, &val);

        if (val == 0) {
            semPost();
        }

    } else {
        sem = sem_open(SEM_NAME, 0);
    }

    if (sem == SEM_FAILED) {
        perror("Unable to create semaphore\n");

        sem_unlink(SEM_NAME);

        return;
    }
}

void semWait() {
    sem_wait(sem);
}

void semPost() {
    sem_post(sem);
}

void semDestroy() {
    sem_unlink(SEM_NAME);
}

