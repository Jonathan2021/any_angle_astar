#include "control.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>


enum move update(struct car *car)
{
    car = car;
    return DO_NOTHING;
}

/*je sais pas ce que je branle mdr*/

init_mat(struct map *map)
{
    struct vector2 *start = malloc(sizeof struct vecotr2);
    start->x = map_get_start_x(map);
    start->y = map_get_start_y(map);
    enum floortype **height = malloc(map->height * sizeof(floortype *));
    for(int i = 0; i < map->height; ++i)
    {
        height[i] = malloc(map->width * sizeof(floortype));
    }
}
