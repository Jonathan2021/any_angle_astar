#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "check.h"
#include "utils.h"

char get_char_from_move(enum move m)
{
    switch (m)
    {
        case ACCELERATE:
            return 'a';
        case BRAKE:
            return 'b';
        case TURN_LEFT:
            return 'l';
        case TURN_RIGHT:
            return 'r';
        case ACCELERATE_AND_TURN_LEFT:
            return 'k';
        case ACCELERATE_AND_TURN_RIGHT:
            return 'e';
        case BRAKE_AND_TURN_LEFT:
            return 'j';
        case BRAKE_AND_TURN_RIGHT:
            return 'w';
        case DO_NOTHING:
            return 'n';
        default:
            return 'n';
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("arguments: %s path_to_map.frc\n", argv[0]);
        return 1;
    }

    size_t len = strlen(argv[1]);
    char frp_name[len];
    strcpy(frp_name, argv[1]);
    frp_name[len - 1] = 'p';

    FILE *frp_file = fopen(frp_name, "w+");
    if (!frp_file)
    {
        perror("open: could not open file: ");
        return 1;
    }

    struct map *map = read_map(argv[1]);
    struct car *car = car_new(map);

    car->direction.x = 0.0f;
    car->direction.y = -1.0f;
    car->direction_angle = 3.0f / 2.0f * M_PI;

    unsigned step = 0;
    enum status state = NONE;
    char moves[512];

    do {
        struct car *cln = car_clone(car);

        if (step == 512)
        {
            if (!fwrite(moves, sizeof(char), step, frp_file))
            {
                perror("write: error when writing to file: ");
                car_delete(cln);
                car_delete(car);
                map_delete(map);
                fclose(frp_file);
                return 1;
            }

            step = 0;
        }

        // We do not trust you enough to give you our own car, sorry
        enum move m = update(cln);
        moves[step++] = get_char_from_move(m);
        state = car_move(car, m);
        car_delete(cln);

    } while (state == NONE);

    car_delete(car);
    map_delete(map);

    if (state == END)
        moves[step - 1] = 'f';
    else
        moves[step - 1] = 'c';

    if (!fwrite(moves, sizeof(char), step, frp_file))
    {
        perror("write: error when writing to file: ");
        car_delete(car);
        map_delete(map);
        fclose(frp_file);
        return 1;
    }

    if (fclose(frp_file) == EOF)
    {
        perror("close: could not close file: ");
        return 1;
    }

    return 0;
}
