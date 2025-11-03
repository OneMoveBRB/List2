#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "list_private.h"
#include "../stack/stack.h"

// FIXME enum error -> string
static int IndexCompare(const void* a, const void* b);

ListErr_t ListInit(List_t** list) {
    assert( list != NULL );

    List_t* list_ptr = NULL;
    list_ptr = (List_t*)calloc(1, sizeof(List_t));
    if (list_ptr == NULL) {
        return LIST_ALLOCATION_FAILED;
    }

    list_ptr->arr.size = 1;
    list_ptr->arr.capacity = FIRST_SIZE;
    list_ptr->arr.data = (Node*)calloc(FIRST_SIZE, sizeof(Node));
    if (list_ptr->arr.data == NULL) {
        FREE(list_ptr);
        return LIST_ALLOCATION_FAILED;
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

    *list = list_ptr;
    list_ptr = NULL;

    return LIST_OK;
}

ListErr_t ListReallocUp(List_t* list) {
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

    if (list->free == FakeElemIdx) {
        list->free = list->arr.size + 1;
    } else {
        list->arr.data[list->free].next = list->arr.size + 1;
    }

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

ListErr_t ListReallocDown(List_t* list, size_t capacity) {
    assert( list != NULL );

    if (capacity >= list->arr.capacity) {
        return LIST_WRONG_CAPACITY;

    } else if (capacity == NONE_CAPACITY) {
        size_t new_capacity = (list->arr.size + FIRST_SIZE - 1)/(FIRST_SIZE - 1) * FIRST_SIZE + 1;
        if (new_capacity >= list->arr.capacity) 
            return LIST_OK;

        list->arr.capacity = new_capacity;
    }

    ListLinearization(list);

    list->arr.data = (Node*)realloc(list->arr.data, list->arr.capacity * sizeof(Node));

    list->arr.data[list->arr.capacity - 1].next = FakeElemIdx;

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

    return RealGet(list, position + 1);
}

size_t ListNext(const List_t* list, size_t position) {
    assert( list != NULL );

    return RealNext(list, position + 1);
}

size_t ListPrev(const List_t* list, size_t position) {
    assert( list != NULL );

    return RealPrev(list, position + 1);
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

    if (position + 1 >= list->arr.capacity) {
        return LIST_INDEX_OUT_OF_RANGE;
    }

    list->arr.data[position + 1].value = value;

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

    return RealDeleteAt(list, ListEnd(list));
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

    return RealDeleteAt(list, real_position);
}

ListErr_t ListLinearization(List_t* list) {
    assert( list != NULL );

    if (ListSize(list) == 0) {
        return LIST_OK;
    }

    type* buffer = (type*)calloc(list->arr.size, sizeof(size_t));
    if (buffer == NULL) {
        return LIST_LINEARIZATION_FAILED;
    }

    size_t buffer_pointer = 0;
    for (size_t i = ListFront(list); i != FakeElemIdx; i = RealNext(list, i)) {
        buffer[buffer_pointer++] = RealGet(list, i);
    }

    list->arr.data[FakeElemIdx].next = 1;
    list->arr.data[FakeElemIdx].prev = list->arr.size - 1;

    for (buffer_pointer = 0; buffer_pointer < list->arr.size - 1; buffer_pointer++) {
        list->arr.data[buffer_pointer + 1].value = buffer[buffer_pointer];
        list->arr.data[buffer_pointer + 1].next = buffer_pointer + 2;
        list->arr.data[buffer_pointer + 1].prev = buffer_pointer;
    }

    list->arr.data[list->arr.size - 1].next = FakeElemIdx;

    list->free = list->arr.size;
    for (size_t i = list->free; i < list->arr.capacity; i++) {
        list->arr.data[i].value = 0;
        list->arr.data[i].next = i + 1;
        list->arr.data[i].prev = UNINITIALIZED;
    }

    list->arr.data[list->arr.capacity - 1].next = FakeElemIdx;

    FREE(buffer);

    return LIST_OK;
}

ListErr_t ListGroupNodes(List_t* list) {
    assert( list != NULL );

    size_t last_node_index = list->arr.capacity - 1;
    for (; list->arr.data[last_node_index].prev == UNINITIALIZED; last_node_index--);

    size_t free_size = list->arr.capacity - list->arr.size;

    size_t* buffer = (size_t*)calloc(free_size, sizeof(size_t));
    if (buffer == NULL) {
        return LIST_GROUP_FAILED;
    }

    size_t buffer_pointer = 0;
    for (size_t free_index = list->free; free_index != FakeElemIdx; free_index = RealNext(list, free_index)) {
        buffer[buffer_pointer++] = free_index;
    }

    qsort(buffer, free_size, sizeof(size_t), IndexCompare);


    list->free = buffer[0];
    for (buffer_pointer = 0; buffer_pointer < free_size - 1; buffer_pointer++) {
        size_t cur_position = buffer[buffer_pointer];
        size_t next_position = buffer[buffer_pointer + 1];

        list->arr.data[cur_position].next = next_position;        
    }
    list->arr.data[buffer[buffer_pointer]].next = FakeElemIdx;

    Stack_t* del_nodes = NULL;
    StackInit(&del_nodes, sizeof(size_t), FIRST_SIZE, "del_nodes");

    for (buffer_pointer = 0; buffer[buffer_pointer] < last_node_index; buffer_pointer++) {
        ListInsert(list, last_node_index - 1, ListGet(list, last_node_index - 1));
        StackPush(del_nodes, &last_node_index);
        
        for (last_node_index--; list->arr.data[last_node_index].prev == UNINITIALIZED; last_node_index--);
    }

    while (del_nodes->meta.size != 0) {
        StackPop(del_nodes, &last_node_index);
        ListErase(list, last_node_index - 1);
    }

    StackDestroy(del_nodes);
    FREE(buffer);

    return LIST_OK;
}

static int IndexCompare(const void* a, const void* b) {
    size_t idx1 = *(const size_t*)a;
    size_t idx2 = *(const size_t*)b;

    if (idx1 < idx2)
        return -1;

    else if (idx1 == idx2)
        return 0;

    return 1;
}

