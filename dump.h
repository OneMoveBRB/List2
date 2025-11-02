#ifndef DUMP_H
#define DUMP_H

#include "list.h"

ListErr_t TextDumpList(List_t* list);
ListErr_t DotVizualizeList(const List_t* list, const char* filename);

#endif