#include "control.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>

struct car *car_angle(struct car *car)
{
    for( int i = 0; i < car->map->width; i++)
    {
	for (int j = 0; j < car->map->height; j++)
 	{
	    if ( map_get_floor(car->map,i,j) == FINISH)
	    {
		car->direction_angle = atan((float)j - (float)map_get_start_y(car->map))/((float)i - (float)map_get_start_x(car->map)); 
		car->direction.x = i;
		car->direction.y = j;
	    }
	}
    }
    
    return car;
}
enum move update(struct car *car)
{
    car = car;	
    car = car_angle(car);
    return ACCELERATE;
}
