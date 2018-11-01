#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include "vector.h"
#include "list_cp.h"

static struct list_cp *list_cp;
int square (int x)
{
    return x*x;
}
struct vector2 *normalize_vec(struct vector2 vec)
{
    struct vector2 *res = vector2_new();
    double div = (sqrt(square(vec.x) + square(vec.y)));
    if (div == 0)
	div = 1;
    res->x = (vec.x)/div;
    res->y = (vec.y)/div;
    return res;
}

struct vector2 *create_vector(struct vector2 p1, struct vector2 *p2)
{
    struct vector2 *res = vector2_new();
    res->x = p2->x - p1.x;
    res->y = p2->y - p1.y;
    return res;
}

struct vector2 *get_arrival(struct car *car)
{
    struct vector2 *arrival = vector2_new();
    for( int i = 0; i < car->map->width; i++)
    {
        for (int j = 0; j < car->map->height; j++)
        {
            if ( map_get_floor(car->map,i,j) == FINISH)
            {
                arrival->x = i + 0.5f;
                arrival->y = j + 0.5f;
            }
        }
    } 
    return arrival;
}


struct list_cp *create_checkpoint(struct car *car)
{
    struct list_cp *list_cp = list_cp_init();
    struct vector2 *cp1 = vector2_new();
    cp1->x = 16;
    cp1->y = 30;
    list_cp->cp = cp1;
    struct vector2 *cp2 = vector2_new();
    cp2->x = 30;
    cp2->y = 18;

    struct vector2 *cp3 = vector2_new();
    cp3->x = 45;
    cp3->y = 3;

    struct vector2 *cp4 = get_arrival(car);

    list_cp = list_cp_append(list_cp,cp1);
    list_cp = list_cp_append(list_cp,cp2);
    list_cp = list_cp_append(list_cp,cp3);
    list_cp = list_cp_append(list_cp,cp4);

    return list_cp;
}


struct vector2 *get_angle(struct car *car, struct vector2 *cp)
{

    struct vector2 *check_point = cp;

    struct vector2 *triangle_rec = vector2_new();
    triangle_rec->x = car->position.x;
    triangle_rec->y = check_point->y;

    float adjacent = sqrt(pow((triangle_rec->x - car->position.x),2) 
            + pow((triangle_rec->y - car->position.y),2)); 
    float oppose = sqrt(pow((triangle_rec->x - check_point->x),2) 
            + pow((triangle_rec->y - check_point->y),2));
    float hypotenus = sqrt(pow((check_point->x - car->position.x),2) 
            + pow((check_point->y - car->position.y),2));

    float cos = adjacent/hypotenus;
    float sin = oppose/hypotenus; 

    struct vector2 *angle = vector2_new();
    angle->x = sin;
    angle->y = -cos;

    return angle;
}

double get_determinant(struct vector2 *checkpoint, struct car *car)
{
    struct vector2 *car_direction = normalize_vec(car->direction);
    struct vector2 *path = normalize_vec(*create_vector(car->position,checkpoint));
    double determinant = path->x*car_direction->y - car_direction->x*path->y;
    printf("determinant = %f\n", determinant);

    return determinant;

}
enum move action (struct car *car)
{

    struct vector2 *checkpoint = list_cp->cp;
    double determinant = get_determinant(checkpoint,car);
    struct vector2 *angle = get_angle(car, checkpoint);

    if (car->speed.x > 0.4f || car->speed.y > 0.4f)
        return BRAKE;
    if (car->direction.x > (angle->x - 0.025f) 
            && car->direction.x < (angle->x + 0.025f) 
            && car->direction.y > (angle->y - 0.025f) 
            && car->direction.y < (angle->y + 0.025f))
        return ACCELERATE;
    
    if (car->direction.x > (angle->x - 0.3f) 
            && car->direction.x < (angle->x + 0.3f) 
            && car->direction.y > (angle->y - 0.3f) 
            && car->direction.y < (angle->y + 0.3f))
    {
	if (determinant <= 0)
	    return ACCELERATE_AND_TURN_RIGHT;
        return ACCELERATE_AND_TURN_LEFT;
    }

    printf("direct x = %f || angle x = %f\n", car->direction.x, angle->x);
    printf("direct y = %f || angle y = %f\n", car->direction.y, angle->y);

    if (determinant <= 0)
        return TURN_RIGHT;
    return TURN_LEFT;
}
enum move update(struct car *car)
{
    if (list_cp == NULL)
        list_cp = create_checkpoint(car);
    if (car->position.x == list_cp->cp->x && car->position.y == list_cp->cp->y)
	list_cp = list_cp_pop(list_cp);
    printf("x = %f && y = %f", list_cp->cp->x, list_cp->cp->y);
    car = car;
    return action(car);
}
