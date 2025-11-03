#include <stdio.h>

#include "list.h"
#include "dump.h"


int main() {
    List_t* list = NULL;
    ListInit(&list);

    ListPushBack(list, 10);
    ListPushBack(list, 20);
    ListInsert(list, 1, 30);
    ListInsert(list, 2, 40);
    ListInsert(list, 2, 50);
    ListInsert(list, 2, 60);
    ListInsert(list, 2, 70);
    // ListInsert(list, 7, 60);

    // ListInsertAfter(&list, 6, 555);
    // ListInsert(&list, 7, 444);
    // ListDeleteAt(&list, 6);
    // ListPop(list);
    ListErase(list, 0);
    // ListErase(list, 4);

    // ListGroupNodes(list);
    ListLinearization(list);
    ListErase(list, 0);

    TextDumpList(list);

    DotVizualizeList(list, "dotstvo.txt");

    ListDestroy(&list);

    return 0;
}
