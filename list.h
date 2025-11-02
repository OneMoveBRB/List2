#ifndef LIST_H
#define LIST_H

#include <stddef.h>

#define FREE(ptr) free(ptr); ptr = NULL;

const int FIRST_SIZE    = 8;
const int UNINITIALIZED = 0xBAD;
const int FakeElemIdx   = 0;

typedef int type;

typedef struct List_t List_t;

enum ListErr_t {
    LIST_OK,
    LIST_OVERFLOW,
    LIST_INSERT_BEFORE_UNINITIALIZED,
    LIST_INSERT_AFTER_UNINITIALIZED,
    LIST_INDEX_OUT_OF_RANGE,
    LIST_ERASE_UNINITIALIZED,
    LIST_OUTPUT_FAILED
};

ListErr_t ListInit(List_t** list);
ListErr_t ListUpdate(List_t* list);
ListErr_t ListDestroy(List_t** list);

type ListGet(List_t* list, size_t position);

size_t ListNext(const List_t* list, size_t position);
size_t ListPrev(const List_t* list, size_t position);
size_t ListFront(const List_t* list);
size_t ListEnd(const List_t* list);
size_t ListSize(const List_t* list);

ListErr_t ListSet(List_t* list, size_t position, type value);

ListErr_t ListPushBack(List_t* list, type value);
ListErr_t ListInsert(List_t* list, size_t position, type value);
ListErr_t ListPop(List_t* list);
ListErr_t ListErase(List_t* list, size_t position);

#endif // LIST_H