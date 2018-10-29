#include "control.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>


struct vector2 *get_arrival(struct car *car)
{
    struct vector2 *arrival = vector2_new();
    for( int i = 0; i < car->map->width; i++)
    {
	for (int j = 0; j < car->map->height; j++)
 	{
	    if ( map_get_floor(car->map,i,j) == FINISH)
	    {
		arrival->x = i;
		arrival->y = j;
	    }
	}
    } 
    return arrival;
}

struct vector2 *get_angle(struct car *car)
{
    struct vector2 *arrival = get_arrival(car);
    struct vector2 *triangle_rec = vector2_new();
    triangle_rec->x = car->position.x;
    triangle_rec->y = arrival->y;

    float adjacent = sqrt(pow((triangle_rec->x - car->position.x),2) + pow((triangle_rec->y - car->position.y),2)); 
    float oppose = sqrt(pow((triangle_rec->x - arrival->x),2) + pow((triangle_rec->y - arrival->y),2));
    float hypotenus = sqrt(pow((arrival->x - car->position.x),2) + pow((arrival->y - car->position.y),2));

    float cos = adjacent/hypotenus;
    float sin = oppose/hypotenus;

    struct vector2 *angle = vector2_new();
    angle->x = sin;
    angle->y = cos;
    return angle;
}

enum move action (struct car *car)
{
    struct vector2 *angle = get_angle(car);
    if (car->speed.x > 0.25f || car->speed.y > 0.25f)
	return BRAKE;
    if (car->direction.x > (angle->x -0.02f) && car->direction.x < (angle->x + 0.02f) 
	    && car->direction.y > (angle->y - 0.02f) && car->direction.y < (angle->y + 0.02f))
	return ACCELERATE;
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
    return TURN_RIGHT;
}
enum move update(struct car *car)
{
    car = car;
    struct vector2 *angle = get_angle(car);	
    printf("angle x = %f || car x = %f \n", angle->x, car->direction.x);
    printf("angle y = %f || car y = %f\n", angle->y, car->direction.y);
    printf("speed x = %f || speed y = %f\n", car->speed.x, car->speed.y);
    return action(car);
}
