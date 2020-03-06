#ifndef LABPROG_MATRIX_H
#define LABPROG_MATRIX_H

#include "../structures/position.h"

struct Matrix_ {

    void ***matrixData;

    int size;
};

typedef struct Matrix_ Matrix;

Matrix *initMatrix(int size);

void *m_lookup(Matrix *, Position *);

void m_insert(Matrix *, Position *, void *);

void *m_delete(Matrix *, Position *);

void freeMatrix(Matrix *);

#endif