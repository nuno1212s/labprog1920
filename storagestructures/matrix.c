#include "matrix.h"
#include <stdlib.h>

Matrix *initMatrix(int size) {

    Matrix *matrix = malloc(sizeof(Matrix));

    matrix->size = size;

    matrix->matrixData = malloc(sizeof(void *) * size);

    for (int i = 0; i < size; i++) {

        matrix->matrixData[i] = malloc(sizeof(void *) * size);

    }

    return matrix;
}

void *m_lookup(Matrix *matrix, Position *pos) {

    return matrix->matrixData[p_getBaseX(pos)][p_getBaseY(pos)];

}

void m_insert(Matrix *matrix, Position *pos, void *value) {

    matrix->matrixData[p_getBaseX(pos)][p_getBaseY(pos)] = value;

}

void *m_delete(Matrix *matrix, Position *pos) {
    void * result = matrix->matrixData[p_getBaseX(pos)][p_getBaseY(pos)];

    matrix->matrixData[p_getBaseX(pos)][p_getBaseY(pos)] = NULL;

    return result;
}

void freeMatrix(Matrix *matrix) {

    for (int i = 0; i < matrix->size; i++) {

        free(matrix->matrixData[i]);

    }

    free(matrix->matrixData);

    free(matrix);
}