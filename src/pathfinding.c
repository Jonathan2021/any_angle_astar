#include "control.h"
#include "vector.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

/*je sais pas ce que je branle mdr*/

struct vector *find_path(struct map *map)
{
    struct vector2 *start = malloc(sizeof struct vecotr2);
    start->x = map_get_start_x(map);
    start->y = map_get_start_y(map);
    struct vector *parent = vector_init(map->width * map->height);

    //enum floortype **height = malloc(map->height * sizeof(floortype *));
    //for(int i = 0; i < map->height; ++i)
    //{
    //    height[i] = malloc(map->width * sizeof(floortype));
    //}

}
