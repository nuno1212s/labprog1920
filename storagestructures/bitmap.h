#ifndef LABPROG_BITMAP_H
#define LABPROG_BITMAP_H

struct BitMap_ {

    unsigned char *data;

    unsigned int word_size, blockSize;

    unsigned int size;

};

typedef struct BitMap_ BitMap;

BitMap *createBitMap(unsigned int size, unsigned int word_size);

void setBit(BitMap *, unsigned int pos, unsigned int value);

unsigned int getBit(BitMap *, unsigned int pos);

void freeBitMap(BitMap *);

#endif //LABPROG_BITMAP_H
