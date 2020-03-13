//
// Created by nuno on 13/03/20.
//

#ifndef LABPROG_LINKEDLIST_H
#define LABPROG_LINKEDLIST_H

struct LinkedList_s;

typedef struct LinkedList_s LinkedList;

LinkedList *ll_initList();

void *ll_peek(LinkedList *);

void *ll_poll(LinkedList *);

void *ll_peekLast(LinkedList *);

int ll_size(LinkedList *);

void ll_enqueue(void *, LinkedList *);

void ll_addLast(void *, LinkedList*);

void ll_addFirst(void *, LinkedList*);

void ll_clear(LinkedList *);

void ll_print(LinkedList *, void (*)(void*));

#endif //LABPROG_LINKEDLIST_H