#ifndef LIST_PRIVATE_H
#define LIST_PRIVATE_H

#include <stddef.h>

#include "list.h"

struct Node {
    type value;
    size_t next;
    size_t prev;
};

struct Nodes {
    Node* data;
    size_t size;
    size_t capacity;
};

struct List_t {
    Nodes  arr;
    size_t free;
};

type RealGet(List_t* list, size_t position);
size_t RealNext(const List_t* list, size_t position);
size_t RealPrev(const List_t* list, size_t position);

ListErr_t RealInsertAfter(List_t* list, size_t position, type value);
ListErr_t RealDeleteAt(List_t* list, size_t position);

#endif // LIST_PRIVATE_H
