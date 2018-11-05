#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include "vector.h"
#include "pathfinding.h"

static struct vector *list_cp = NULL;
static size_t pos;

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

// à toi de mettre tes checkpoints !
struct vector *create_checkpoint(struct car *car)
{
    printf("entered creat_checkpoint\n");
    return find_path(car->map);
    /*struct vector *list_cp = vector_init(4);
      pos++;
      struct vector2 *cp1 = vector2_new();
      cp1->x = 15;
      cp1->y = 3;
      struct vector2 *cp2 = vector2_new();
      cp2->x = 30;
      cp2->y = 18;

      struct vector2 *cp3 = vector2_new();
      cp3->x = 45;
      cp3->y = 3;

      struct vector2 *cp4 = get_arrival(car);

      list_cp = vector_append(list_cp,*cp1);
      list_cp = vector_append(list_cp,*cp2);
      list_cp = vector_append(list_cp,*cp3);
      list_cp = vector_append(list_cp,*cp4);

      return list_cp;
      */
}


double get_angle(struct car *car, struct vector2 *cp)
{
    struct vector2 *check_point = cp;

    struct vector2 *triangle_rec = vector2_new();
    triangle_rec->x = car->position.x;
    triangle_rec->y = check_point->y;

    float oppose = sqrt(pow((triangle_rec->x - check_point->x),2) 
            + pow((triangle_rec->y - check_point->y),2));
    float hypotenus = sqrt(pow((check_point->x - car->position.x),2) 
            + pow((check_point->y - car->position.y),2));

    float sin = oppose/hypotenus; 

    double angle = asin(sin)*180/M_PI;

    return angle;
}

double get_determinant(struct vector2 *checkpoint, struct car *car)
{
    struct vector2 *car_direction = normalize_vec(car->direction);
    struct vector2 *path = 
        normalize_vec(*create_vector(car->position,checkpoint));
    double determinant = path->x*car_direction->y - car_direction->x*path->y;
    printf("determinant = %f\n", determinant);

    return determinant;

}

enum move action (struct car *car)
{
    struct vector2 checkpoint = list_cp->data[pos];
    double determinant = get_determinant(&checkpoint,car);
    double angle = get_angle(car, &checkpoint);
    double car_angle = asin(car->direction.x)*180/M_PI; 

    printf("angle = %f\n", angle);
    printf("car angle = %f\n", asin(car->direction.x)*180/M_PI);

    if (angle > car_angle - 2 && angle < car_angle + 2
            && determinant < 0.3f && determinant > -0.3f) 
        return ACCELERATE;

    if (angle > car_angle - 4 && angle < car_angle + 4 
            && determinant < 0.3f && determinant > -0.3f)
    {
        if (determinant <= 0)
            return ACCELERATE_AND_TURN_RIGHT;
        return ACCELERATE_AND_TURN_LEFT;
    }

    if (determinant <= 0)
        return BRAKE_AND_TURN_RIGHT;
    return BRAKE_AND_TURN_LEFT;
}

enum move update(struct car *car)
{
    if (list_cp == NULL)
        list_cp = create_checkpoint(car);
    
    double brake_distance = (fabs(car->speed.x) + fabs(car->speed.y))*15;
    if (brake_distance < 1)
        brake_distance = 1;

    if (car->position.x >= list_cp->data[pos].x - brake_distance
            && car->position.x <= list_cp->data[pos].x + brake_distance 
            && car->position.y >= list_cp->data[pos].y - brake_distance 
            && car->position.y <= list_cp->data[pos].y + brake_distance 
            && pos < list_cp->size)
    {
        if (car->speed.x == 0 && car->speed.y == 0)
        {
            if (pos < list_cp->size - 1)
                pos++;
            printf("CHECKPOINT GOOD ! SWAPING TO NEXT ONE\n");
        }
        else
            if (pos < list_cp->size - 1)
                return BRAKE;
    }
    
    printf("speed x = %f && y = %f\n",car->speed.x,car->speed.y); 
    printf("list x = %f && y = %f\n",list_cp->data[pos].x,list_cp->data[pos].y); 
    printf("car x = %f && y = %f\n", car->position.x, car->position.y);
    car = car;
    return action(car);
}
