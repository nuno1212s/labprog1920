#ifndef LABPROG_SEM_H
#define LABPROG_SEM_H

#define SEM_NAME "/gamesem1"

void initSem(int host);

int semWait();

int semPost();

void semDestroy();

int semTryWait();

int semValue();

#endif //LABPROG_SEM_H
