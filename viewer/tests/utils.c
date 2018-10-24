#include "check.h"
#include "utils.h"

enum floortype get_type(struct map *map, char cell, int i, int j)
{
    enum floortype type = BLOCK;

    switch (cell)
    {
        case 's':
            map->start.x = i + 0.5f;
            map->start.y = j + 0.5f;
            type = ROAD;
            break;
        case 'r':
            type = ROAD;
            break;
        case 'f':
            type = FINISH;
            break;
        case 'g':
            type = GRASS;
            break;
        default:
            break;
    }

    return type;
}

void load_map(struct map *map, FILE *file)
{
    char *line = NULL;
    long int size = 0;
    size_t cap = 0;
    int j = 0;

    while ((size = getline(&line, &cap, file)) > 0)
    {
        for (int i = 0; i < size - 1; i++)
        {
            enum floortype type = get_type(map, line[i], i, j);
            map_set_floor(map, type, i, j);
        }
        j++;
        free(line);
        line = NULL;
    }
    free(line);
}

struct map *read_map(char *filename)
{
    FILE *file = fopen(filename, "r");
    char *line = NULL;
    long int size = 0;
    size_t cap = 0;
    int size_x = 0;
    int size_y = 0;

    while ((size = getline(&line, &cap, file)) > 0)
    {
        if (size - 1 > size_x)
            size_x = size - 1;
        size_y++;
        free(line);
        line = NULL;
    }
    free(line);

    rewind(file);

    struct map *map = map_new(size_x, size_y, 0.0f, 0.0f);

    load_map(map, file);

    fclose(file);
    return map;
}
