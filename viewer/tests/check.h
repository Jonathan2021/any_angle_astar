#ifndef CHECK_H_
#define CHECK_H_

#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include "control.h"

struct map *map_new(int width, int height, float sx, float sy);
void map_set_floor(struct map *self, enum floortype type, int x, int y);
void map_delete(struct map *self);

#endif /* CHECK_H_ */
