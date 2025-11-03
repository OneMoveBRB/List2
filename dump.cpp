#include "dump.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include "list_private.h"

ListErr_t TextDumpList(List_t* list) {
    assert( list != NULL );

    const char* filename = "List_test.txt";
    FILE* fp = fopen(filename, "a");
    if (fp == NULL) {
        fprintf(stderr, "Couldn't open  List_test.txt\n");
        return LIST_OUTPUT_FAILED;
    }

    fprintf(fp, "\n############## LIST DUMP ##############\n");
    fprintf(fp, "List size: %zu\nList capacity: %zu\nList data: ", 
                ListSize(list), list->arr.capacity);
    
    for (size_t i = 0; i < list->arr.capacity; i++) {
        fprintf(fp, "%d ", RealGet(list, i));
    } fprintf(fp, "\n");

    fprintf(fp, "List next: ");

    for (size_t i = 0; i < list->arr.capacity; i++) {
        fprintf(fp, "%zu ", RealNext(list, i));
    } fprintf(fp, "\n");

    fprintf(fp, "List prev: ");

    for (size_t i = 0; i < list->arr.capacity; i++) {
        fprintf(fp, "%zu ", RealPrev(list, i));
    } fprintf(fp, "\n");

    fprintf(fp, "List HEAD: %zu\nList TAIL: %zu\nList FREE: %zu\n", 
                ListFront(list), ListEnd(list), list->free);

    fclose(fp);

    return LIST_OK;
}

ListErr_t DotVizualizeList(const List_t* list, const char* filename) {
    assert( list != NULL );
    assert( filename != NULL );

    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        return LIST_OUTPUT_FAILED; 
    }

    errno = 0;

    fprintf(fp, "digraph List {\n\t");
    fprintf(fp, "rankdir=LR;\n\t");
    fprintf(fp, "node [shape=record, style=filled, fillcolor=lightblue];\n\t");
    fprintf(fp, "edge [fontsize=10,  color=black];\n\n\t");

    Node* fake_node = &list->arr.data[FakeElemIdx];
    fprintf(fp, "node%d [label=\"<f0> value: %d | <f1> Ridx: %d | Uidx: None | {<f2>    Rprev: %zu | <f3> Rnext: %zu}\"];\n\t",
                FakeElemIdx, fake_node->value, FakeElemIdx, fake_node->prev, fake_node->next);

    for (size_t i = 1; i != list->arr.capacity; i++) {
        Node* cur_node = &list->arr.data[i];
        
        if (cur_node->prev == UNINITIALIZED) {
            fprintf(fp, "node%zu [label=\"<f0> value: %d | <f1> Ridx: %zu | Uidx: %zu | {<f2>  Rprev: POIZON | <f3> Rnext: %zu}\", fillcolor=pink];\n\t",
                    i, cur_node->value, i, i-1, cur_node->next);
        } else {
            fprintf(fp, "node%zu [label=\"<f0> value: %d | <f1> Ridx: %zu | Uidx: %zu | {<f2>  Rprev: %zu | <f3> Rnext: %zu}\"];\n\t",
                    i, cur_node->value, i, i-1, cur_node->prev, cur_node->next);
        }

    } 
    
    fprintf(fp, "\n\t");

    int is_arrows_diverge = 0;

    // list->arr.data[4].next = 6;

    for (size_t cur_pos = FakeElemIdx; cur_pos != ListEnd(list); cur_pos = RealNext(list, cur_pos)) {
        Node* cur_node  = &list->arr.data[cur_pos];
        Node* next_node = &list->arr.data[cur_node->next];

        if (cur_pos == next_node->prev) {
            fprintf(fp, "node%zu:f3 -> node%zu:f2 [color=green, dir=both, arrowhead=normal];\n\t",
                        cur_pos, cur_node->next);
        } else {
            is_arrows_diverge = 1;

            fprintf(fp, "node%zu:f3 -> node%zu:f2 [color=blue, arrowhead=normal];\n\t",
                        cur_pos, cur_node->next);
        }
    } 
    
    fprintf(fp, "\n\t");

    if (is_arrows_diverge) {
        size_t cur_pos = ListFront(list);

        do {
            cur_pos = RealNext(list, cur_pos);

            Node* cur_node  = &list->arr.data[cur_pos];
            Node* prev_node = &list->arr.data[cur_node->prev];

            if (cur_pos != prev_node->next) {
                fprintf(fp, "node%zu:f2 -> node%zu:f3 [color=red, arrowhead=normal];\n\t",
                            cur_pos, cur_node->prev);
            }

        } while (cur_pos != ListEnd(list));

        fprintf(fp, "\n\t");
    }

    fprintf(fp, "head [shape=plaintext, label=\"HEAD\"];\n\t");
    fprintf(fp, "head -> node%zu [color=green, style=bold];\n\t", ListFront(list));

    fprintf(fp, "tail [shape=plaintext, label=\"TAIL\"];\n\t");
    fprintf(fp, "tail -> node%zu:f0 [color=orange, style=bold];\n\t", ListEnd(list));

    fprintf(fp, "free [shape=plaintext, label=\"FREE\"];\n\t");
    fprintf(fp, "free -> node%zu [color=purple, style=bold];\n\t", list->free);

    fprintf(fp, "\n\t");

    if (list->free != FakeElemIdx) {
        for (size_t cur_pos = list->free; RealNext(list, cur_pos) != FakeElemIdx; cur_pos = RealNext(list, cur_pos)) {
            fprintf(fp, "node%zu -> node%zu [color=\"purple\", style=bold];\n\t", 
                        cur_pos, RealNext(list, cur_pos));
        }
    }

    if (list->arr.data[FakeElemIdx].next != FakeElemIdx) {
        fprintf(fp, "node0:f3 -> node%zu:f2 [color=white, dir=both, arrowhead=normal];\n\t",
                    ListFront(list));
        fprintf(fp, "node%zu:f3 -> node%zu:f2 [color=white, arrowhead=normal];\n\t",
                    RealPrev(list, ListEnd(list)), ListEnd(list));
    }

    fprintf(fp, "node%zu:f3 -> node%zu:f2 [color=white, arrowhead=normal];\n\t",
                ListEnd(list), list->free);
    fprintf(fp, "node%zu:f3 -> node%zu:f2 [color=white, arrowhead=normal];\n}",
                ListEnd(list), list->free);

    fclose(fp);
        
    system("dot -Tsvg dotstvo.txt > img.svg");
    if (errno != 0) {
        fprintf(stderr, "System command failed!\n");
        return LIST_OUTPUT_FAILED;
    }

    return LIST_OK;
}
