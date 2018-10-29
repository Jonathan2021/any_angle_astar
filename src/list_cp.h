#ifndef LIST_CP_H_
#define LIST_CP_H_

#include "control.h"

struct list_cp
{
    struct vector2 *cp;
    struct list_cp *next;
};

struct list_cp *list_append(struct list_cp *l, struct list_cp elt);
struct list_cp *list_remove(struct list_cp *l, size_t i);

#endif
