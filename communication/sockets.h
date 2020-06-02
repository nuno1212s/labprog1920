#ifndef LABPROG_SOCKETS_H
#define LABPROG_SOCKETS_H

#include <stdlib.h>

void s_init(int);

void s_startWriter();

void s_writeToBuffer(void *, size_t);

void s_endWriter();

void s_startReader();

void s_readFromBuffer(void *, size_t);

void s_endReader();

void s_waitForOtherPlayerToChoosePieces();

void s_destroy();

#endif //LABPROG_SOCKETS_H
