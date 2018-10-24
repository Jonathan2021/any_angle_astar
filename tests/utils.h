#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include "control.h"

enum move update(struct car *car);
enum floortype get_type(struct map *map, char cell, int i, int j);
void load_map(struct map *map, FILE *file);
struct map *read_map(char *filename);

#endif /* UTILS_H_ */
