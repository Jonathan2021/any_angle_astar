#include <stdlib.h>
#include <stdio.h>
#include "list_cp.h"

struct list_cp *list_cp_init(void)
{
    struct list_cp *list_cp_init = malloc(sizeof(struct list_cp));
    list_cp_init->cp = vector2_init();
    list_cp_init->next = NULL;
    
    return list_cp_init;
}

void list_cp_push(struct list_cp *list_cp, struct vector2 element)
{
    struct list_cp *new_list = malloc(sizeof(struct list_cp));
    new_list->cp = element;
    new_list->next = NULL;
    list_cp->next = new_list;
}


void list_cp_pop(struct list_cp *list_cp)
{
    list_cp = list_cp->next;
}
        

void list_cp_destroy(struct list_cp *list_cp)
{
    list_cp = NULL;
    free(list_cp->next);
    free(list_cp);
}
