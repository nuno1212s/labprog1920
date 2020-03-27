#ifndef LABPROG_BITMAP_H
#define LABPROG_BITMAP_H

#define matrix_rows(matrix) (matrix->rows)
#define matrix_cols(matrix) (matrix->columns)
#define matrix_size(matrix) (matrix_rows(matrix) * matrix_cols(matrix))

struct BitMatrix_ {

    unsigned int rows, columns;

    struct BitMap_ *data;

};

struct BitMap_ {

    unsigned char *data;

    unsigned int word_size, blockSize;

    unsigned int size;

};

typedef struct BitMap_ BitMap;

typedef struct BitMatrix_ BitMatrix;

BitMatrix * createBitMatrix(unsigned int rows, unsigned int columns, unsigned int word_size);

void m_setBit(BitMatrix*, unsigned int row, unsigned int column, unsigned int value);

unsigned int m_getBit(BitMatrix *, unsigned int row, unsigned int column);

void freeBitMatrix(BitMatrix *);

BitMap *createBitMap(unsigned int size, unsigned int word_size);

void setBit(BitMap *, unsigned int pos, unsigned int value);

unsigned int getBit(BitMap *, unsigned int pos);

void freeBitMap(BitMap *);

#endif //LABPROG_BITMAP_H
