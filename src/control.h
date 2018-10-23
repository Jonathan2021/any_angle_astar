#ifndef CONTROL_H_
# define CONTROL_H_

/* CONSTANTS */

# define _USE_MATH_DEFINES

# include <math.h>

# define M_PI 3.14159265358979323846

# define CAR_ACCEL_FACTOR               (0.02f)
# define CAR_BRAKE_FACTOR               (0.95f)
# define CAR_FRICTION_FACTOR            (0.98f)
# define CAR_GRASS_FRICTION_FACTOR      (0.90f)
/* equivalent to 1° */
# define CAR_TURN_ANGLE                 ((1.0f * (2.0f * M_PI)) / (360.0f))
# define CAR_MAX_SPEED                  (0.5f)
# define CAR_MIN_SPEED                  (CAR_ACCEL_FACTOR / 2.0f)

/* API */

struct vector2
{
  float x;
  float y;
};

enum floortype
{
  ROAD = 0x00,
  GRASS = 0x01,
  BLOCK = 0x02,
  FINISH = 0x03
};

enum move
{
  ACCELERATE = 0x10,
  BRAKE = 0x20,
  TURN_LEFT = 0x01,
  TURN_RIGHT = 0x02,
  ACCELERATE_AND_TURN_LEFT = 0x11,
  ACCELERATE_AND_TURN_RIGHT = 0x12,
  BRAKE_AND_TURN_LEFT = 0x21,
  BRAKE_AND_TURN_RIGHT = 0x22,
  DO_NOTHING = 0x00
};

enum status
{
  NONE = 0x00,
  CRASH = 0x01,
  END = 0x02
};

struct map
{
  enum floortype *floor;
  int width;
  int height;
  struct vector2 start;
};

struct car
{
  struct vector2 position;
  struct vector2 speed;
  struct vector2 acceleration;
  struct vector2 direction;       /* direction and direction angle store the */
  float direction_angle; /* same data represented differently */

  struct map *map;
};

enum floortype map_get_floor(struct map *map, int x, int y);
float map_get_start_x(struct map *map);
float map_get_start_y(struct map *map);
void map_delete(struct map *map);

struct car *car_new(struct map *map);
struct car *car_clone(struct car *car);
void car_delete(struct car *car);
enum status car_move(struct car *car, enum move move);

struct vector2 *vector2_new(void);
struct vector2 *vector2_clone(struct vector2 *vector);
void vector2_delete(struct vector2 *vector);

#endif /* CONTROL_H_ */
