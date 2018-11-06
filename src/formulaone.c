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


// à toi de mettre tes checkpoints !
struct vector *create_checkpoint(struct car *car)
{
    printf("entered creat_checkpoint\n");
    return find_path(car->map);
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

    double car_turn_angle = (CAR_TURN_ANGLE)*180/M_PI;

    if (angle > fabs(car_angle) - car_turn_angle*2 
            && angle < fabs(car_angle) + car_turn_angle*2
            && determinant < 0.3f && determinant > -0.3f) 
        return ACCELERATE;

    if (angle > fabs(car_angle) - 4 && angle < fabs(car_angle) + 4 
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

struct vector2 *brake_to_stop(struct car *car)
{
    struct car *save_car = car_clone(car);
    int iter = 0;
    while (save_car->speed.x != 0 && save_car->speed.y != 0)
    {
        car_move(save_car,BRAKE);
        iter++;
    }
    struct vector2 *pos = vector2_new();
    pos->x = save_car->position.x;
    pos->y = save_car->position.y;
    car_delete(save_car);
    return pos;
}

enum move update(struct car *car)
{
    if (list_cp == NULL)
        list_cp = create_checkpoint(car);

    if (pos != list_cp->size - 1)
    {
        struct vector2 *pos_stop = brake_to_stop(car);
        if (pos_stop->x >= list_cp->data[pos].x - 0.5f 
                && pos_stop->x <= list_cp->data[pos].x + 0.5f
                && pos_stop->y >= list_cp->data[pos].y - 0.5f 
                && pos_stop->y <= list_cp->data[pos].y + 0.5f
                && pos < list_cp->size)
        {
            if (car->speed.x == 0 && car->speed.y == 0)
            {
                if (pos < list_cp->size - 1)
                {
                    pos++;
                }
                if (pos == list_cp->size - 1)
                {
                    list_cp->data[pos].x += 0.5f;
                    list_cp->data[pos].y += 0.5f;
                }
                printf("supposed pos x = %f y = %f", list_cp->data[pos-1].x,
                        list_cp->data[pos-1].y);
                printf("mine x = %f y = %f", car->position.x, car->position.y);
                printf("CHECKPOINT GOOD ! SWAPING TO NEXT ONE\n\n\n\n");
            }
            else
                    return BRAKE;
        }
    }

    printf("speed x = %f && y = %f\n",car->speed.x,car->speed.y); 
    printf("list x = %f && y = %f\n",list_cp->data[pos].x,list_cp->data[pos].y); 
    printf("car x = %f && y = %f\n", car->position.x, car->position.y);
    car = car;
    return action(car);
}
