#include <stdlib.h>
#include <stdio.h>
#include "list_cp.h"
/*
struct list_cp *list_cp_init(void)
{
    struct list_cp *list_cp_init = malloc(sizeof(struct list_cp));
    list_cp_init->size = 0;
    list_cp_init->head = NULL;
    list_cp_init->tail = NULL;
    return list_cp_init;
}

size_t list_cp_size(struct list_cp *list_cp)
{
    return list_cp->size;
}

void list_cp_push(struct list_cp *list_cp, int element)
{
    struct list *new_list = malloc(sizeof(struct list));
    new_list->data = element;
    new_list->next = NULL;

    new_list->next = list_cp->tail;
    list_cp->tail = new_list;
    list_cp->size++;
}

int list_cp_head(struct list_cp *list_cp)
{
    struct list *elm = malloc(sizeof(struct list));
    elm = list_cp->tail;
    while (elm->next)
        elm = elm->next;

    return elm->data;
}

void list_cp_pop(struct list_cp *list_cp)
{
    struct list *elm = malloc(sizeof(struct list));
    elm = list_cp->tail;
    while (elm->next)
    {
        if (elm->next->next == NULL)
        {
            list_cp->head = elm;
            list_cp->size--;
            break;
        }
        elm = elm->next;
    }
}
        
void list_cp_clear(struct list_cp *list_cp) 
{
    list_cp = NULL;
    free(list_cp);
}

void list_cp_destroy(struct list_cp *list_cp)
{
    list_cp = NULL;
    free(list_cp);
}


void print_me(int i)                                                               
{                                                                                  
    int save = i;                                                                  
    int count = 0;                                                                 
    int pos = 0;                                                                   
    while (save > 0)                                                               
    {                                                                              
        count++;                                                                   
        save/=10;                                                                  
    }                                                                              
    char *res = malloc(sizeof(char)*count);                                        
    while ( i > 0)                                                                 
    {                                                                              
        res[pos] = i%10 + 48;                                                      
        i/=10;                                                                     
        pos++;                                                                     
    }                                                                              
                                                                                   
    for (int i = pos; i > 0; i--)                                                  
        putchar(res[i]);                                                           
}               

void list_cp_print(const struct list_cp *list_cp)
{
    struct list *elm = malloc(sizeof(struct list));
    elm = list_cp->head;

    while(!elm)
    {
        print_me(elm->data);
        elm = elm->next;
    }
}

*/
