#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include "vector.h"
#include "pathfinding.h"

static struct vector *list_cp = NULL;
static size_t pos = 0;

double square (double x)
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
    struct vector *path = find_path(car->map);
    return path;
}


double get_angle(struct vector2 car, struct vector2 *cp)
{
    struct vector2 *check_point = cp;

    struct vector2 *triangle_rec = vector2_new();
    triangle_rec->x = car.x;
    triangle_rec->y = check_point->y;

    float oppose = sqrt(pow((triangle_rec->x - check_point->x),2) 
	    + pow((triangle_rec->y - check_point->y),2));
    float hypotenus = sqrt(pow((check_point->x - car.x),2) 
	    + pow((check_point->y - car.y),2));

    float sin = oppose/hypotenus; 

    double angle = asin(sin)*180/M_PI;

    return angle;
}

double get_determinant(struct vector2 *checkpoint, struct car *car, 
	struct vector2 car_pos)
{
    struct vector2 *car_direction = normalize_vec(car->direction);
    struct vector2 *path = normalize_vec(*create_vector(car_pos,checkpoint));
    double determinant = path->x*car_direction->y - car_direction->x*path->y;
    printf("determinant = %f\n", determinant);

    return determinant;

}

enum move action (struct car *car)
{
    struct vector2 checkpoint = list_cp->data[pos];
    double determinant = get_determinant(&checkpoint,car, car->position);
    double angle = get_angle(car->position, &checkpoint);
    double car_angle = asin(car->direction.x)*180/M_PI; 
    double diff = fabs(fabs(car_angle) - fabs(angle));

    printf("angle = %f\n", angle);
    printf("car angle = %f\n", asin(car->direction.x)*180/M_PI);

    double car_turn_angle = (CAR_TURN_ANGLE)*180/M_PI;

    if (diff <= car_turn_angle/2 && determinant < 0.3f && determinant > -0.3f)
    {
	return ACCELERATE;
    }
    if (diff < 5 && determinant < 0.3f && determinant > -0.3f)
    {
	if (determinant <= 0)
	    return ACCELERATE_AND_TURN_RIGHT;
	return ACCELERATE_AND_TURN_LEFT;
    } 
    if (diff < 10 && determinant < 0.3f && determinant > -0.3f)
    {
	if (determinant <= 0)
	    return TURN_RIGHT;
	return TURN_LEFT;
    }
    if (determinant <= 0)
	return BRAKE_AND_TURN_RIGHT;
    return BRAKE_AND_TURN_LEFT;
}

struct vector2 *brake_to_speed(struct car *car, double speed)
{
    struct car *save_car = car_clone(car);
    double car_speed = fmax(save_car->speed.x, save_car->speed.y);
    while (fabs(car_speed) > speed)
    {
	car_move(save_car,BRAKE);
	car_speed = fmax(save_car->speed.x, save_car->speed.y);
    }
    struct vector2 *pos = vector2_new();
    pos->x = save_car->position.x;
    pos->y = save_car->position.y;
    car_delete(save_car);
    return pos;
}

double speed_according_to_angle(struct car *car)
{
    double next_determinant = 
	fabs(get_determinant(&list_cp->data[pos+1],car,list_cp->data[pos]));
    //return 0; 
    if (next_determinant < 0.05f)
	return 0.35f;
    if (next_determinant < 0.1f)
	return 0.25f;
    if (next_determinant < 0.2f)
	return 0.15f;
    if (next_determinant < 0.4f)
	return 0.05f;
    return 0;
}

enum move go_to_cp(struct car *car);

int crash_test(struct car *car, size_t real_pos)
{
    struct car *crash_car = car_clone(car);
    int crash = 0;
    double approx = (fabs(crash_car->speed.x) + fabs(crash_car->speed.y));
    if (approx < 0.25f)
	approx = 0.25f;
    while (!(crash_car->position.x >= list_cp->data[pos].x - approx 
		&& crash_car->position.x <= list_cp->data[pos].x + approx
		&& crash_car->position.y >= list_cp->data[pos].y - approx 
		&& crash_car->position.y <= list_cp->data[pos].y + approx)
		&& crash != 1)
    {
	enum status car_status = car_move(crash_car,go_to_cp(crash_car));
	printf("crash_car x = %f, y = %f\n", 
                crash_car->position.x, crash_car->position.y);
	if (car_status == CRASH)
	    crash = 1;
    }

    car_delete(crash_car);
    printf("crash = %d\n\n\n", crash);
    if (crash == 1 && pos > real_pos)
    {
	pos--;
	return crash_test(car,real_pos);
    }
    else
	return pos;
}

enum move go_to_cp(struct car *car)
{
    if (pos < list_cp->size - 1)
    {
	double speed = speed_according_to_angle(car);
	double approx = (fabs(car->speed.x) + fabs(car->speed.y));
	if (approx < 0.25f)
	    approx = 0.25f;
	struct vector2 *pos_stop = brake_to_speed(car, speed);
	double stop_distance = sqrt(square(pos_stop->x - car->position.x) + square(pos_stop->y - car->position.y));
	double car_distance = sqrt(square(list_cp->data[pos].x - car->position.x) + square(list_cp->data[pos].y - car->position.y));

	if ((car_distance > (stop_distance - approx) && car_distance < (stop_distance + approx))
		|| (pos_stop->x >= list_cp->data[pos].x - approx 
		&& pos_stop->x <= list_cp->data[pos].x + approx
		&& pos_stop->y >= list_cp->data[pos].y - approx 
		&& pos_stop->y <= list_cp->data[pos].y + approx
		&& pos < list_cp->size))
	{
	    if (fabs(fmax(car->speed.x,car->speed.y)) <= speed)
	    {	
		size_t save_pos = pos;
		if (pos + 5 > list_cp->size - 2)
		    pos = list_cp->size-2;
		else
		    pos += 5;
		crash_test(car, save_pos);
		if (pos == save_pos && pos < list_cp->size-1)
		    pos++;

		printf("supposed pos x = %f y = %f", list_cp->data[pos-1].x,
			list_cp->data[pos-1].y);
		printf("mine x = %f y = %f", car->position.x, car->position.y);
		printf("CHECKPOINT GOOD ! SWAPING TO NEXT ONE\n\n\n\n");
	    }
	    else
		return BRAKE;
	}
    }
    return action(car);
}
enum move update(struct car *car)
{
    if (list_cp == NULL)
	list_cp = create_checkpoint(car);

    printf("speed x = %f && y = %f\n",car->speed.x,car->speed.y); 
    printf("list x = %f && y = %f\n",list_cp->data[pos].x,list_cp->data[pos].y); 
    printf("car x = %f && y = %f\n", car->position.x, car->position.y);
    car = car;
    return go_to_cp(car);
}
