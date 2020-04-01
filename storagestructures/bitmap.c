#include "bitmap.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


BitMatrix * createBitMatrix(unsigned int rows, unsigned int columns, unsigned int word_size) {

    BitMatrix *matrix = malloc(sizeof(BitMatrix));

    matrix->rows = rows;
    matrix->columns = columns;

    unsigned int matrix_size_i = matrix_size(matrix);

    BitMap * map = createBitMap(matrix_size_i, word_size);

    matrix->data = map;

    return matrix;
}

void m_setBit(BitMatrix * matrix, unsigned int row, unsigned int col, unsigned int value) {

    BitMap *data = matrix->data;

    //Get the correct row position and add the correct column
    unsigned int slot_in_data = row * matrix->columns + col;

    setBit(data, slot_in_data, value);

}

unsigned int m_getBit(BitMatrix * matrix, unsigned int row, unsigned int col) {

    BitMap *data = matrix->data;

    unsigned int slot_int_data = row * matrix->columns + col;

    return getBit(data, slot_int_data);

}

void freeBitMatrix(BitMatrix *matrix) {

    freeBitMap(matrix->data);

    free(matrix);

}

BitMap *createBitMap(unsigned int size, unsigned int word_size) {

    BitMap *map = malloc(sizeof(BitMap));

    unsigned int charSize = sizeof(unsigned char) * 8;

    unsigned int finalSize = ((word_size * size) / charSize) + 1;

    map->data = calloc(charSize, finalSize);
    map->word_size = word_size;
    map->size = size;
    map->blockSize = charSize;

    return map;
}

void setBit(BitMap *map, unsigned int pos, unsigned int value) {

    if (pos >= map->size) {
        fprintf(stderr, "Attempting to set bit too large. %d > %d (- 1 as it includes the 0)", pos, map->size);

        exit(0);
    }

    unsigned char *data = map->data;

    unsigned int bitPos = pos * map->word_size, bytePos = bitPos / map->blockSize,
            withinBlock = bitPos % map->blockSize;

    unsigned int reset = (unsigned int) pow(2, map->word_size) - 1;

    //Set the bits to 0
    data[bytePos] &= ~(reset << withinBlock);

    //Setting the value
    data[bytePos] |= (value << withinBlock);
}

unsigned int getBit(BitMap *map, unsigned int pos) {

    unsigned char * data = map->data;

    unsigned int bitPos = pos * map->word_size, bytePos = bitPos / map->blockSize,
            withinBlock = bitPos % map->blockSize;

    unsigned int worldValue = (unsigned int) pow(2, map->word_size) - 1;

    return (data[bytePos] & (worldValue << withinBlock)) >> withinBlock;
}

void freeBitMap(BitMap *map) {

    free(map->data);

    free(map);

}