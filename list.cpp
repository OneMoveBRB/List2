#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "list_setup.h"

// FIXME enum error -> string

static ListErr_t RealInsertAfter(List_t* list, size_t position, type value);
static ListErr_t ListDeleteAt(List_t* list, size_t position);

ListErr_t ListInit(List_t** list) {
    assert( list != NULL );
    
    *list = (List_t*)calloc(1, sizeof(List_t));
    if (*list == NULL) {
        return LIST_OVERFLOW;
    }
    
    List_t* list_ptr = *list;

    list_ptr->arr.size = 1;
    list_ptr->arr.capacity = FIRST_SIZE;
    list_ptr->arr.data = (Node*)calloc(FIRST_SIZE, sizeof(Node));
    if (list_ptr->arr.data == NULL) {
        FREE(*list);
        return LIST_OVERFLOW;
    }

    list_ptr->free = 1;

    for (size_t i = list_ptr->free; i < FIRST_SIZE - 1; i++) {
        list_ptr->arr.data[i].next = i + 1;
        list_ptr->arr.data[i].prev = UNINITIALIZED;
    }   
    
    list_ptr->arr.data[FakeElemIdx].next = FakeElemIdx;
    list_ptr->arr.data[FakeElemIdx].prev = FakeElemIdx;
    list_ptr->arr.data[FIRST_SIZE - 1].next = FakeElemIdx;
    list_ptr->arr.data[FIRST_SIZE - 1].prev = UNINITIALIZED;

    return LIST_OK;
}

ListErr_t ListUpdate(List_t* list) {
    assert( list != NULL );

    const size_t exp_multiplier = 2;
    
    Node* new_data = NULL;

    new_data = (Node*)realloc(list->arr.data, list->arr.capacity * exp_multiplier * sizeof(Node));

    if (new_data == NULL) {
        return LIST_OVERFLOW;
    }

    list->arr.capacity *= exp_multiplier;
    list->arr.data = new_data;
    new_data = NULL;

    list->arr.data[list->free].next = list->arr.size + 1;

    for (size_t i = list->arr.size + 1; i < list->arr.capacity - 1; i++) {
        list->arr.data[i].value = 0;
        list->arr.data[i].next = i + 1;
        list->arr.data[i].prev = UNINITIALIZED;
    }

    list->arr.data[list->arr.capacity - 1].value = 0;
    list->arr.data[list->arr.capacity - 1].next = FakeElemIdx;
    list->arr.data[list->arr.capacity - 1].prev = UNINITIALIZED;

    return LIST_OK;
}

ListErr_t ListDestroy(List_t** list) {
    assert(  list != NULL );
    assert( *list != NULL );

    List_t* list_ptr = *list;

    list_ptr->free = 0;

    list_ptr->arr.size = 0;
    list_ptr->arr.capacity = 0;
    FREE(list_ptr->arr.data);

    FREE(*list);

    return LIST_OK;
}

type ListGet(List_t* list, size_t position) {
    assert( list != NULL );

    return list->arr.data[position].value;
}

size_t ListNext(const List_t* list, size_t position) {
    assert( list != NULL );
    return list->arr.data[position].next;
}

size_t ListPrev(const List_t* list, size_t position) {
    assert( list != NULL );
    return list->arr.data[position].prev;
}

size_t ListFront(const List_t* list) {
    assert( list != NULL );
    return list->arr.data[FakeElemIdx].next;
}

size_t ListEnd(const List_t* list) {
    assert( list != NULL );
    return list->arr.data[FakeElemIdx].prev;
}

size_t ListSize(const List_t* list) {
    assert( list != NULL );
    return list->arr.size - 1;
}

ListErr_t ListSet(List_t* list, size_t position, type value) {
    assert( list != NULL );

    list->arr.data[position].value = value;

    return LIST_OK;
}

ListErr_t ListPushBack(List_t* list, type value) {
    assert( list != NULL );

    return RealInsertAfter(list, ListEnd(list), value);
}

ListErr_t ListInsert(List_t* list, size_t position, type value) {
    assert( list != NULL );

    size_t real_position = position + 1;

    if (real_position >= list->arr.capacity) {
        return LIST_INDEX_OUT_OF_RANGE;
    }

    if (list->arr.data[real_position].prev == UNINITIALIZED) {
        return LIST_INSERT_BEFORE_UNINITIALIZED;
    }

    return RealInsertAfter(list, list->arr.data[real_position].prev, value);
}

ListErr_t ListPop(List_t* list) {
    assert( list != NULL );

    return ListDeleteAt(list, ListEnd(list));
}

ListErr_t ListErase(List_t* list, size_t position) {
    assert( list != NULL );

    size_t real_position = position + 1;
    
    if (real_position >= list->arr.capacity) {
        return LIST_INDEX_OUT_OF_RANGE;
    }

    if (list->arr.data[real_position].prev == UNINITIALIZED) {
        return LIST_ERASE_UNINITIALIZED;
    }

    return ListDeleteAt(list, real_position);
}

static ListErr_t RealInsertAfter(List_t* list, size_t position, type value) {
    assert( list != NULL );

    size_t prev_position = position;
    Node*  prev_node     = &list->arr.data[prev_position];

    size_t next_position = prev_node->next;
    Node*  next_node     = &list->arr.data[next_position];

    size_t free_position = list->free;
    Node*  free_node     = &list->arr.data[free_position];

    list->free = free_node->next;

    free_node->value = value;

    prev_node->next = free_position;
    free_node->prev = prev_position;

    next_node->prev = free_position;
    free_node->next = next_position;

    ++list->arr.size;

    if (list->arr.size + 1 == list->arr.capacity) {
        return ListUpdate(list);
    }

    return LIST_OK;
}

static ListErr_t ListDeleteAt(List_t* list, size_t position) {
    assert( list != NULL );

    size_t del_position = position;
    Node*  del_node     = &list->arr.data[del_position];
    
    size_t prev_position = del_node->prev;
    Node*  prev_node     = &list->arr.data[prev_position];
    
    size_t next_position = del_node->next;
    Node*  next_node     = &list->arr.data[next_position];

    del_node->value = 0;
    del_node->prev = UNINITIALIZED;

    del_node->next = list->free;
    list->free = del_position;
    
    prev_node->next = next_position;
    next_node->prev = prev_position;

    --list->arr.size;

    return LIST_OK;
}
