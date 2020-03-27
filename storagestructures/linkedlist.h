#ifndef LABPROG_LINKEDLIST_H
#define LABPROG_LINKEDLIST_H

struct Node_s {

    struct Node_s *next;

    void *data;

};

struct LinkedList_s {

    struct Node_s *first, *last;

    int size;

};

typedef struct LinkedList_s LinkedList;

LinkedList *ll_initList();

void *ll_peek(LinkedList *);

void *ll_poll(LinkedList *);

void *ll_peekLast(LinkedList *);

void* ll_get(LinkedList *, int);

int ll_size(LinkedList *);

void ll_enqueue(void *, LinkedList *);

void ll_addLast(void *, LinkedList *);

void ll_addFirst(void *, LinkedList *);

void ll_clear(LinkedList *);

void ll_print(LinkedList *, void (*)(void *));

void ll_forEach(LinkedList *, void (*)(void *));

#endif //LABPROG_LINKEDLIST_H