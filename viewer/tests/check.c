#include "check.h"
#include "utils.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("arguments: %s path_to_map.frc\n", argv[0]);
        return 1;
    }

    struct map *map = read_map(argv[1]);
    struct car *car = car_new(map);

    car->direction.x = 0.0f;
    car->direction.y = -1.0f;
    car->direction_angle = 3.0f / 2.0f * M_PI;

    unsigned step = 0;
    enum status state = NONE;

    do {
        step++;

        // We do not trust you enough to give you our own car, sorry
        struct car *cln = car_clone(car);
        state = car_move(car, update(cln));
        car_delete(cln);
    } while (state == NONE);

    car_delete(car);
    map_delete(map);

    if (state == END)
        printf("Map finished in %d steps!\n", step);
    else
        printf("Crash after %d steps!\n", step);

    return 0;
}
