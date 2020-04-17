#include <stdlib.h>
#include "linkedlist.h"

void freeNode(struct Node_s *node) {

    free(node);

}

LinkedList *ll_initList() {

    LinkedList *list = malloc(sizeof(LinkedList));

    list->size = 0;
    list->first = NULL;
    list->last = NULL;

    return list;
}

void *ll_peek(LinkedList *list) {

    if (list->first == NULL) {
        return NULL;
    }

    return list->first->data;

}

void *ll_peekLast(LinkedList *list) {

    if (list->last == NULL) {
        return NULL;
    }

    return list->last->data;
}

void *ll_get(LinkedList *list, int pos) {

    if (pos >= list->size || pos < 0) {
        return NULL;
    }

    int current = 0;

    struct Node_s *first = list->first;

    while (current != pos) {

        current++;

        first = first->next;
    }

    return first->data;
}

void *ll_remove(LinkedList *list, int index) {

    if (index < 0 || index >= ll_size(list)) {
        return NULL;
    }

    struct Node_s* first = list->first;

    void *data;

    if (index == 0) {

        list->first = first->next;

        data = first->data;

        if (first == list->last) {
            //A list with a size of 1
            list->last = NULL;
        }

        list->size--;

        freeNode(first);

        return data;
    }

    int current = 1;

    struct Node_s* prev = first;

    first = first->next;

    while (first != NULL) {

        if (current++ == index) {
            prev->next = first->next;

            if (first == list->last) {
                list->last = prev;
            }

            data = first->data;

            list->size--;

            freeNode(first);

            break;
        }

        prev = first;
        first = first->next;
    }

    return data;
}

int ll_indexOf(LinkedList *list, void *data) {

    int current = 0;

    struct Node_s *first = list->first;

    while (first != NULL) {

        if (first->data == data) {
            return current;
        }

        current++;
        first = first->next;
    }

    return -1;
}

int ll_size(LinkedList *list) {
    return list->size;
}

void *ll_poll(LinkedList *list) {

    if (list->size == 0) {
        return NULL;
    }

    struct Node_s *first = list->first;

    list->first = first->next;

    list->size--;

    void *data = first->data;

    free(first);

    return data;
}

void ll_addFirst(void *data, LinkedList *list) {

    struct Node_s *newNode = malloc(sizeof(struct Node_s));

    newNode->data = data;

    if (list->size == 0) {
        //The list is empty

        list->last = newNode;
        list->first = newNode;
        newNode->next = NULL;

    } else {

        newNode->next = list->first;
        list->first = newNode;
    }

    list->size++;
}


void ll_addLast(void *data, LinkedList *list) {

    struct Node_s *newNode = malloc(sizeof(struct Node_s));

    newNode->data = data;
    newNode->next = NULL;

    if (list->size == 0) {
        //The list is empty

        list->last = newNode;
        list->first = newNode;
    } else {

        list->last->next = newNode;
        list->last = newNode;
    }

    list->size++;

}

void ll_enqueue(void *data, LinkedList *list) {
    ll_addLast(data, list);
}

void ll_clear(LinkedList *list) {

    struct Node_s *current = list->first;

    while (current != NULL) {

        struct Node_s *next = current->next;

        free(current);

        current = next;
    }

    list->size = 0;
    list->first = NULL;
    list->last = NULL;
}

void ll_print(LinkedList *list, void(*print)(void *)) {

    struct Node_s *first = list->first;

    while (first != NULL) {

        print(first->data);

        first = first->next;
    }

}

void ll_forEach(LinkedList *list, void (*accept)(void *)) {

    struct Node_s *first = list->first;

    while (first != NULL) {
        accept(first->data);

        first = first->next;
    }

}

void ll_free(LinkedList *list) {

    ll_clear(list);

    free(list);

}