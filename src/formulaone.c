#include "control.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include "vector.h"


static struct vector *list_cp;

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


struct vector *create_checkpoint(struct car *car)
{
    struct vector *list_cp = vector_init(4);

    struct vector2 *cp1 = vector2_new();
    cp1->x = 10;
    cp1->y = 10;

    struct vector2 *cp2 = vector2_new();
    cp1->x = 20;
    cp1->y = 20;

    struct vector2 *cp3 = vector2_new();
    cp1->x = 30;
    cp1->y = 30;

    struct vector2 *cp4 = get_arrival(car);

    vector_append(list_cp,*cp1);
    vector_append(list_cp,*cp2);
    vector_append(list_cp,*cp3);
    vector_append(list_cp,*cp4);

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

    double determinant = check_point->x*car->position.y 
        - car->position.x*check_point->y;

    struct vector2 *angle = vector2_new();
    angle->x = sin;
    if (determinant < 0)
        angle->y = -cos;
    else
        angle->y = cos;

    return angle;
}

enum move action (struct car *car)
{
    if (list_cp == NULL)
        list_cp = create_checkpoint(car);

    struct vector2 *check_point = list_cp->data;
    vector_remove(list_cp, 0);
    struct vector2 *angle = get_angle(car, check_point);
    double determinant = check_point->x*car->direction.y - 
        car->direction.x*check_point->y;
    printf("determinant = %f\n", determinant);
    if (car->speed.x > 0.4f || car->speed.y > 0.4f)
        return BRAKE;
    if (car->direction.x > (angle->x -0.02f) 
            && car->direction.x < (angle->x + 0.02f) 
            && car->direction.y > (angle->y - 0.02f) 
            && car->direction.y < (angle->y + 0.02f))
        return ACCELERATE;
    /*
       if (angle->x > 0 && angle->y > 0)
       {
       if (car->direction.x < angle->x && car->direction.y > angle->y)
       return BRAKE_AND_TURN_RIGHT;
       else
       return BRAKE_AND_TURN_LEFT;
       }
       if (angle->x > 0 && angle->y < 0)
       {
       if (car->direction.x > angle->x && car->direction.y > angle->y)
       return BRAKE_AND_TURN_RIGHT;
       else
       return BRAKE_AND_TURN_LEFT;
       }
       if (angle->x < 0 && angle->y < 0)
       {
       if (car->direction.x > angle->x && car->direction.y < angle->y)
       return BRAKE_AND_TURN_RIGHT;
       else
       return BRAKE_AND_TURN_LEFT;
       }
       if (angle->x < 0 && angle->y > 0)
       {
       if (car->direction.x < angle->x && car->direction.y < angle->y)
       return BRAKE_AND_TURN_RIGHT;
       else
       return BRAKE_AND_TURN_LEFT;
       }
       */
    if (determinant < 0)
        return TURN_RIGHT;
    else
        return TURN_LEFT;
}
enum move update(struct car *car)
{
    car = car;
    /*
    struct vector2 *angle = get_angle(car);	
    printf("angle x = %f || car x = %f \n", angle->x, car->direction.x);
    printf("angle y = %f || car y = %f\n", angle->y, car->direction.y);
    printf("speed x = %f || speed y = %f\n", car->speed.x, car->speed.y);
    */
    return action(car);
}
