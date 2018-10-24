#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "vector.h"

struct vector *vector_init(size_t n)
{
    struct vector *vect = malloc(sizeof(struct vector));
    if(vect)
    {
        vect->capacity= n;
        vect->size = 0;
        vect->data = malloc(n* sizeof(struct vector2));
        if(!vect->data)
            return NULL;
    }
    return vect;
}

void vector_destroy(struct vector *v)
{
    free(v->data);
    v->size=0;
    v->capacity=0;
    free(v);
}

struct vector *vector_resize(struct vector *v, size_t n)
{
        if(n<v->size)
            v->size = n;
        v->capacity=n;
        v->data=realloc(v->data, n * sizeof(struct vector2));
        if(!v->data)
            return NULL;
        return v;
}

struct vector *vector_append(struct vector *v, struct vector2 elt)
{
    if(v->size == v->capacity)
    {
            vector_resize(v, v->capacity*2);
    }
    v->data[v->size] = elt;
    v->size++;
    return v;
    
}

void vector_print(const struct vector *v)
{
    for(size_t i = 0; i<v->size; ++i)
    {
        if(i == v->size-1)
            printf("(%f, %f)", v->data[i].x, v->data[i].y);
        else
            printf("(%f, %f); ", v->data[i].x, v->data[i].y);
    }
    putchar('\n');
}

struct vector *vector_reset(struct vector *v, size_t n)
{
    v->size = 0;
    free(v->data);
    v->capacity = n;
    v->data = malloc(n * sizeof(int));
    if(!v->data)
        v->capacity = 0;
    return v;
}

struct vector *vector_insert(struct vector *v, size_t i, struct vector2 elt)
{
    struct vector2 tmp;
        vector_append(v, elt);
    for(; i < v->size; ++i)
    {
        tmp = v->data[i];
        v->data[i] = elt;
        elt = tmp;
    }
    return v;
}

struct vector *vector_remove(struct vector *v, size_t i)
{
    for(; i < (v->size-1); ++i)
    {
        v->data[i]=v->data[i+1];
    }
    v->size--;
    if((v->size) * 2 < v->capacity)
    {
        vector_resize(v, v->capacity/2);
    }
    return v;
}

