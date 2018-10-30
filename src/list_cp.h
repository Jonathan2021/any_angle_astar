#ifndef LIST_CP_H_
#define LIST_CP_H_

#include "control.h"

struct list_cp
{
    struct vector2 *cp;
    struct list_cp *next;
};

struct list_cp *list_cp_init(void);
struct list_cp *list_cp_append(struct list_cp *list_cp,
     struct vector2 *element);
struct list_cp *list_cp_pop(struct list_cp *list_cp);
void list_cp_destroy(struct list_cp *list_cp);

#endif
