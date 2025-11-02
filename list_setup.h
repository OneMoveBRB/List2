#ifndef LIST_SETUP_H
#define LIST_SETUP_H

#include <stddef.h>

typedef int type;

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

#endif // LIST_SETUP_H