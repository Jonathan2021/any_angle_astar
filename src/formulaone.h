#ifndef FORMULAONE_C_
#define FORMULAONE_C_
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "vector.h"
#include "pathfinding.h"

enum move go_to_cp(struct car *car);
int crash_test(struct car *car, size_t real_pos);
double speed_according_to_angle(struct car *car);
struct vector2 *brake_to_speed(struct car *car, double speed);
enum move action(struct car *car);

#endif /* FORMULAONE_C_ */
