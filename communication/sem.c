
#include "sem.h"

#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

static sem_t *sem;

static int isHost;

void initSem(int host) {

    isHost = host;

    if (host) {
        sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);
        int val;

        sem_getvalue(sem, &val);

        if (val == 0) {
            semPost();
        }

    } else {
        sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    }

    if (sem == SEM_FAILED || sem == NULL) {
        perror("Unable to create semaphore\n");

        sem_unlink(SEM_NAME);

        return;
    }

}

int semWait() {
    return sem_wait(sem);
}

int semPost() {

    int result = sem_post(sem);

    printf("Sem post called, result %d, new sem value: %d \n",result, semValue());

    return result;
}

int semTryWait() {
    return sem_trywait(sem);
}

int semValue() {
    int value;

    sem_getvalue(sem, &value);

    return value;
}

void semDestroy() {
    if (isHost) {
        sem_close(sem);
        sem_unlink(SEM_NAME);
    } else {
        sem_close(sem);
    }
}

