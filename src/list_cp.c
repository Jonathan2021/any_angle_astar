#include <stdlib.h>
#include <stdio.h>
#include "list_cp.h"

struct list_cp *list_cp_init(void)
{
    struct list_cp *list_cp_init = malloc(sizeof(struct list_cp));
    list_cp_init->cp = vector2_new();
    list_cp_init->next = NULL;
    
    return list_cp_init;
}

struct list_cp *list_cp_append(struct list_cp *list_cp, struct vector2 *element)
{
    struct list_cp *new_list = malloc(sizeof(struct list_cp));
    new_list->cp = element;
    new_list->next = NULL;

    struct list_cp *elt = list_cp;
    while(elt->next)
	elt = elt->next;

    elt->next = new_list;
    return list_cp;
}


struct list_cp *list_cp_pop(struct list_cp *list_cp)
{
    list_cp = list_cp->next;
    return list_cp;
}
        

void list_cp_destroy(struct list_cp *list_cp)
{
    list_cp = NULL;
    free(list_cp->next);
    free(list_cp);
}
