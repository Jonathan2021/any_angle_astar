#include "control.h"
#include "vector.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define SCALE 5
#define PORTION 2 

static int map_width = 0;
static int map_height = 0;
static int row_static, col_static; //postion of the point being explored
static int goal = 0; //will be set to one when path is found
//list of points to be opened
static struct vector2 *finish = NULL; //first end point found
static int **closed = NULL;
static enum floortype **my_map = NULL;

struct point
{
    //parent coordinates
    float parent_x; 
    float parent_y;

    //values useful for A*
    double h; //distance to goal
    double g; //
    double f; //h + g
};

static struct point **mat_point = NULL;

//check if coords are inside map
static int in_limit(int y, int x)
{
    return (y >= 0 && y < map_height && x >= 0 && x < map_width);
}

//get floortype of vector2
static enum floortype floor_vect(struct vector2 v)
{
    return my_map[(int)v.y][(int)v.x];
}

// return true if vector2 is a BLOCK floortype
static int blocked_vec(struct vector2 v)
{
    enum floortype floor = floor_vect(v);
    return (floor == BLOCK);
}

// return true if coords is BLOCK floortype
static int blocked(int row, int col)
{
    enum floortype floor = my_map[row][col];
    return(floor == BLOCK);
}

static int squeezed(int old_row, int old_col, int row, int col)
{
    int col_diff = col - old_col;
    int row_diff = row - old_row;
    return (blocked(old_row, old_col + col_diff) && \
    blocked(old_row + row_diff, old_col));
}

// return true if vector2 is FINISH BLOCK
static int arrived_vec(struct vector2 v)
{
    return (floor_vect(v) == FINISH);
}

// return true if coords is FINISH BLOCK
static int arrived(int row, int col)
{
    return (my_map[row][col] == FINISH);
}

//Finds vector2 with floortype FINISH in map
static void find_finish()
{
    if(finish)
        return;
    for(int i = 0; i < map_height; ++i)
    {
        for(int j = 0; j < map_width; ++j)
        {
            if(my_map[i][j] == FINISH)
            {
                finish = malloc(sizeof(struct vector2));
                finish->y = i;
                finish->x = j;
                break;
            }
         }
    }
}

//fill matrice of 0
static void zero_fill(int **mat, int height, int width)
{
    for(int i = 0; i < height; ++i)
    {
        for(int j = 0; j < width; ++j)
            mat[i][j] = 0;
    }
}

//point is initialized to parent coords = its coords f=g=h=0
static void init_point(struct point *p, float y, float x)
{
    p->parent_x = x;
    p->parent_y = y;
    p->h = 0.0;
    p->g = 0.0;
    p->f = 0.0;
}

//initialize point matrice to parent coords = self and h=g=f=max value
static void init_mat_point(int height, int width)
{
    for(int i = 0; i < height; ++i)
    {
        for(int j = 0; j < width; ++j)
        {
            mat_point[i][j].parent_x = -1;
            mat_point[i][j].parent_y = -1;
            mat_point[i][j].h = -1;
            mat_point[i][j].g = -1;
            mat_point[i][j].f = -1;
        }
    }
}

void init_los(int *dy, int *sy, int *dx, int *sx)
{
    if (*dy < 0)
    {
        *dy *= -1;
        *sy = -1;
    }
    if (*dx < 0)
    {
        *dx *= -1;
        *sx = -1;
    }
}

int line_of_sight(int parent_y, int parent_x, int y, int x)
{
    int sy = 1, sx = 1, f = 0;
    int dy = y - parent_y;
    int dx = x - parent_x;
    init_los(&dy, &sy, &dx, &sx);
    if (dx >= dy)
    {
        while (parent_x != x)
        {
            f = f + dy;
            if (f >= dx)
            {
                if (blocked(parent_y + ((sy - 1) / 2), parent_x + ((sx - 1) / 2)))
                    return 0;
                parent_y = parent_y + sy;
                f = f - dx;
            }
            if (f != 0 && blocked(parent_y + ((sy - 1) / 2), parent_x + ((sx - 1) / 2)))
                return 0;
            if (!dy && blocked(parent_y, parent_x + ((sx - 1) / 2)) && blocked(parent_y - 1, parent_x + ((sx - 1) / 2)))
                return 0;
            parent_x += sx;
        }
    }
    else
    {
        while (parent_y != y)
        {
            f = f + dx;
            if (f >= dy)
            {
                if (blocked(parent_y + ((sy - 1) / 2), parent_x + ((sx - 1) / 2)))
                    return 0;
                parent_x += sx;
                f = f - dy;
            }
            if (f != 0 && blocked(parent_y + ((sy - 1) / 2), parent_x + ((sx - 1) / 2)))
                return 0;
            if (!dx && blocked(parent_y + ((sy - 1) / 2), parent_x) && blocked(parent_y + ((sy - 1) / 2), parent_x - 1))
                return 0;
            parent_y += sy;   
        }
    }
    return 1;
}
//makes and returns vector2 with corresponding coordinates
static struct vector2 make_vec2(float y, float x)
{
    struct vector2 v = {.x = x, .y = y};
    return v;
}

//calculates value of point.h at corresponding coords
static double get_c(int row, int col, int finish_x, int finish_y)
{
    return ((double)sqrt((row - finish_y)*(row - finish_y) + \
                (col - finish_x)*(col - finish_x)));
}

void ComputeCost(int old_y, int old_x, int y, int x)
{
    double new_g;
    struct point par = mat_point[old_y][old_x];
    struct point gp = mat_point[(int)par.parent_y][(int)par.parent_x];
    if (line_of_sight(par.parent_y, par.parent_x, y, x))
    {
        new_g = (gp.g + get_c((int)par.parent_y, (int)par.parent_x, y, x));
        if(mat_point[y][x].g < 0 || new_g < mat_point[y][x].g)
        {
            mat_point[y][x].parent_x = par.parent_x;
            mat_point[y][x].parent_y = par.parent_y;
            mat_point[y][x].g = new_g;
        }
    }
    else
    {
        new_g = mat_point[old_y][old_x].g + get_c(old_y, old_x, y, x);
        if(mat_point[y][x].g < 0 || new_g < mat_point[y][x].g)
        {
            mat_point[y][x].parent_x = old_x;
            mat_point[y][x].parent_y = old_y;
            mat_point[y][x].g = new_g;
        }
    }
}

/*Builds path vector in correct order
  doesn't include starting coords because they could be float but path vect
  only has rounded numbers*/
static void get_path(struct vector *path, int row, int col)
{
    int row_tmp;
    if(!path)
    {
        path = vector_init(8);
    }
    while(!(mat_point[row][col].parent_y == row && \
                mat_point[row][col].parent_x == col))
    {
        path = vector_insert(path, 0, 
        make_vec2((float)(row) / SCALE, (float)(col) / SCALE));
        row_tmp = mat_point[row][col].parent_y;
        col = mat_point[row][col].parent_x;
        row = row_tmp;
    }
}
/*explore point mat_point[row][col] next to mat_point[row_static][col_static]
  and finds end or modifies f, g, h values if needed*/
static void explore(struct vector *path, int row, int col, struct vector *open)
{
    if(goal)
        return;
    if(in_limit(row, col) && !squeezed(row_static, col_static, row, col))
    {
        if(arrived(row, col)) //we reached end point
        {
            ComputeCost(row_static, col_static, row, col);
            get_path(path, row, col); //trace back path
            goal = 1; //YAY we arrived at the finish line
            return;
        }

        if (!closed[row][col] && !blocked(row, col)) //not a block 
        {                                            //and not explored 
            ComputeCost(row_static, col_static, row, col);
            double new_h = get_c(row, col, (int)finish->y, (int)finish->x);
            double new_f = mat_point[row][col].g + new_h;
            if(mat_point[row][col].f < 0 || mat_point[row][col].f > new_f)
            {
                struct vector2 tmp = make_vec2(row, col);
                open = vector_remove_v2(open, tmp);
                open = vector_append(open, tmp); //to be explored
                mat_point[row][col].h = new_h;
                mat_point[row][col].f = new_f;
            }

        }
    }
}

//explore all 8 directions
static void explore_all(struct vector *path, struct vector *open)
{
    explore(path, row_static - 1, col_static, open); //above
    explore(path, row_static + 1, col_static, open); //under
    explore(path, row_static, col_static - 1, open); //left
    explore(path, row_static, col_static + 1, open); //right
    explore(path, row_static - 1, col_static - 1, open); //upper-left
    explore(path, row_static - 1, col_static + 1, open); //upper-right
    explore(path, row_static + 1, col_static - 1, open); //lower-left
    explore(path, row_static + 1, col_static + 1, open); //lower-right
}

void contour(int y, int x)
{
    int marge = SCALE / PORTION;
    for (int i = y - marge; i < y + SCALE + marge; ++i)
    {
        for(int j = x - marge; j < x + SCALE + marge; ++j)
        {
            if(in_limit(i, j))
            {
                my_map[i][j] = BLOCK;
            }
        }

    }
}

void fill_my_map(struct map *map, int height, int width)
{
    for(int i = 0; i < height; ++i)
    {
        for(int j = 0; j < width; ++j)
        {
            my_map[i][j] = map_get_floor(map, j / SCALE, i / SCALE);
        }
    }
    for(int i = 0; i < map->height; ++i)
    {
        for(int j = 0; j < map->width; ++j)
        {
            if(map_get_floor(map, j, i) == BLOCK)
            {
                contour(i * SCALE, j * SCALE);
            }
        }
    }
}

//initializing some static variables
static void init_static()
{
    closed = malloc(map_height * (sizeof(int *)));
    mat_point = malloc(map_height * sizeof(struct point *));
    for(int i = 0; i < map_height; ++i)
    {
        closed[i] = malloc(map_width * sizeof(int));
        mat_point[i] = malloc(map_width * sizeof(struct point));
    }
   
    zero_fill(closed, map_height, map_width); 
    init_mat_point(map_height, map_width);
}

static void free_all(struct vector *open)
{
    for(int i = 0; i < map_height; ++i)
    {
        free(closed[i]);
        free(mat_point[i]);
        free(my_map[i]);
    }
    free(my_map);
    free(closed);
    free(mat_point);
    free(finish);
    vector_destroy(open);
}

//initializes map_height and map_width
static void get_real_map(struct map *map)
{
    if(!my_map)
    {
        map_width = map->width * SCALE;
        map_height = map->height * SCALE;
        my_map = malloc(map_height * sizeof(enum floortype *));
        for (int i = 0; i < map_height ; ++i)
            my_map[i] = malloc(map_width * sizeof(enum floortype));
        fill_my_map(map, map_height , map_width);

    }
}
static char floor_to_char(int y, int x, struct map *map)
{
    switch (map_get_floor(map, x, y))
    {
        case ROAD:
            return 'r';
        case GRASS:
            return 'g';
        case BLOCK:
            return 'b';
        case FINISH:
            return 'f';
    }
    return 0;
}

static char **create_matrix(struct vector *path, struct map *map)
{
    char **mat = malloc(map->height * sizeof(char *));
    for(int i = 0; i < map->height; ++i)
    {
        mat[i] = malloc(map->width * sizeof(char));
    }
    for(int i = 0; i < map->height; ++i)
    {
        for(int j = 0; j < map->width; ++j)
        {
            mat[i][j] = floor_to_char(i, j, map);
        }
    }
    for(size_t i = 0; i < path->size; ++i)
    {
        mat[(int)path->data[i].y][(int)path->data[i].x] += ('A' - 'a');
    }
    return mat;
}

static void free_mat(char **mat, int height)
{
    for(int i = 0; i < height; ++i)
    {
        free(mat[i]);
    }
    free(mat);
}

static void print_cell(char c)
{
    if(c < 'a')
    {
        printf("%s%c%s", KRED, c, KWHT);
    }
    switch (c)
    {
        case 'r':
            printf("%s%c", KWHT, c);
            break;
        case 'g':
            printf("%s%c%s", KGRN, c, KWHT);
            break;
        case 'b':
            printf("%s%c%s", KBLU, c, KWHT);
            break;
        case 'f':
            printf("%s%c%s", KYEL, c, KWHT);
            break;
    }
}

static void print_map(struct vector *path, struct map *map)
{
    char **mat = create_matrix(path, map);
    for(int i = 0; i < map->height; ++i)
    {
        for(int j = 0; j < map->width; ++j)
        {
            print_cell(mat[i][j]);
        }
        printf("\n");
    }
    free_mat(mat, map->height);
}

struct vector2 get_start(struct map *map)
{
    struct vector2 start; //starting point
    start.x = map_get_start_x(map) * SCALE;
    start.y = map_get_start_y(map) * SCALE;
    return start;

}
// A* path finding algorithm on map
struct vector *find_path(struct map *map)
{
    get_real_map(map); //initialize static map_width and map_height
    struct vector2 start = get_start(map);
    struct vector *path = vector_init(8);
    
    find_finish(); //set static vector2 finish variable;
    //if initial conditions don't make sense
    if(!finish || blocked_vec(start) || arrived_vec(start))
        goto fail_init; //frees memory and returns NULL(path didn't change);

    init_static(); //initializing mat_point, closed and open static variables

    row_static = (int)start.y, col_static = (int)start.x;
    //parent=self h=f=g=0
    init_point(&mat_point[row_static][col_static], row_static, col_static);

    //add start point to soon to be opened points
    struct vector *open = vector_append(NULL, make_vec2(row_static, col_static));

    while(open->size) //something to be opened
    {
        
        //FIXME: open smallest one with smallest f in mat_point
        //instead of first one
        struct vector2 cur = open->data[0]; //first element of vect
        open = vector_remove(open, 0); //removing it
        row_static = cur.y, col_static = cur.x; //updating static coordinates
        closed[row_static][col_static] = 1; //explored
        explore_all(path, open);
        if(goal)
            break;
    }
    print_map(path, map);

fail_init: //free allocated stuff and return the path(NULL if not found)
    free_all(open);
    vector_print(path);
    return path;
}
