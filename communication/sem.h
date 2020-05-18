#ifndef LABPROG_SEM_H
#define LABPROG_SEM_H

#define SEM_NAME "/gamesem"

void initSem(int host);

void semWait();

void semPost();

void semDestroy();

#endif //LABPROG_SEM_H
