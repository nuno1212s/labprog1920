#ifndef LABPROG_SHAREDMEM_H
#define LABPROG_SHAREDMEM_H

#include "communications.h"
#include <stdlib.h>

void p_init(int);

void p_startWriter();

void p_writeToBuffer(void *, size_t);

void p_endWriter();

void p_startReader();

void p_readFromBuffer(void *, size_t);

void p_endReader();

void p_waitForOtherPlayerToChoosePieces();

void p_block();

void p_destroy();

#endif //LABPROG_SHAREDMEM_H
