#include "bitmap.h"
#include <stdlib.h>
#include <math.h>

BitMap *createBitMap(unsigned int size, unsigned int word_size) {

    BitMap *map = malloc(sizeof(BitMap));

    unsigned int charSize = sizeof(unsigned char);

    unsigned int finalSize = ((word_size * size) / charSize) + 1;

    map->data = malloc(charSize * finalSize);
    map->word_size = word_size;
    map->size = finalSize;
    map->blockSize = charSize;

    return map;
}

void setBit(BitMap *map, unsigned int pos, unsigned int value) {

    unsigned char *data = map->data;

    unsigned int bitPos = pos * map->word_size, bytePos = bitPos / map->blockSize,
            withinBlock = bitPos % map->blockSize;

    data[bytePos] |= (value << withinBlock);

}

unsigned int getBit(BitMap *map, unsigned int pos) {

    unsigned char * data = map->data;

    unsigned int bitPos = pos * map->word_size, bytePos = bitPos / map->blockSize,
            withinBlock = bitPos % map->blockSize;

    unsigned int worldValue = pow(2, map->word_size) - 1;

    return (data[bytePos] & (worldValue << withinBlock)) >> withinBlock;
}

void freeBitMap(BitMap *map) {

    free(map->data);

    free(map);

}