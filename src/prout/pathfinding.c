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

/*je sais pas ce que je branle mdr*/
static struct vector **list_p = NULL;
static int list_size = 0;
static int map_width = 0;
static int map_height = 0;
static int row_static, col_static; //postion of the point being explored
static int goal = 0; //will be set to one when path is found
//list of points to be opened
static struct vector2 *finish = NULL; //first end point found
static int **closed = NULL;

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

//initializes map_height and map_width
static void get_width(struct map *map)
{
    //printf("entering get_width\n");
    if(!map_width && !map_height)
    {
        map_width = map->width;
        map_height = map->height;
    }
    //printf("exiting get_width\n");
}

//check if vector2 is inside map
static int in_limit_vec(struct vector2 *v)
{
    //printf("entering in_limit_vec\n");
    if(!v)
        return 0;
    return ((v->x >= 0 && v->x < (float)map_width) && \
            (v->y >= 0 && v->y < (float)map_height));
}

//check if coords are inside map
static int in_limit(int y, int x)
{
    //printf("entering in_limit\n");
    return (y >= 0 && y < map_height && x >= 0 && x < map_width);
}

//get floortype of vector2
static enum floortype floor_vect(struct map *map, struct vector2 *v)
{
    //printf("entering floor_vect\n");
    return map_get_floor(map, (int)v->x, (int)v->y);
}

// return true if vector2 is a BLOCK floortype
static int blocked_vec(struct map *map, struct vector2 *v)
{
    //printf("entering blocked_vec\n");
    enum floortype floor = floor_vect(map, v);
    //printf("exiting floor_vect\n");
    return (floor == BLOCK /*|| floor == GRASS*/);
}

// return true if coords is BLOCK floortype
static int blocked(struct map *map, int row, int col)
{
    //printf("entering blocked\n");
    enum floortype floor = map_get_floor(map, col, row);
    return(floor == BLOCK /*|| floor == GRASS*/);
}

static int squeezed(struct map *map, int old_row, int old_col, int row, int col)
{
    int col_diff = col - old_col;
    int row_diff = row - old_row;
    return (blocked(map, old_row, old_col + col_diff) && \
    blocked(map, old_row + row_diff, old_col));
}

// return true if vector2 is FINISH BLOCK
static int arrived_vec(struct map *map, struct vector2 *v)
{
    //printf("entering arrived_vec\n");
    return (floor_vect(map, v) == FINISH);
}

// return true if coords is FINISH BLOCK
static int arrived(struct map *map, int row, int col)
{
    //printf("entering arrived\n");
    return (map_get_floor(map, col, row) == FINISH);
}

//Finds vector2 with floortype FINISH in map
//FIXME find closest finish instead of first one
static void find_finish(struct map *map)
{
    //printf("entering find_finish\n");
    if(!finish)
    {
        finish = malloc(sizeof(struct vector2));
        finish->x = -1;
        finish->y = -1;
        for(int i = 0; i < map_height; ++i)
        {
            for(int j = 0; j < map_width; ++j)
            {
                if(map_get_floor(map, j, i) == FINISH)
                {
                    finish->y = i;
                    finish->x = j;
                    break;
                }
            }
        }
    }
    //printf("exiting find_finish\n");
}

//fill matrice of 0
static void zero_fill(int **mat, int height, int width)
{
    //printf("entering zero_fill\n");
    for(int i = 0; i < height; ++i)
    {
        for(int j = 0; j < width; ++j)
            mat[i][j] = 0;
    }
    //printf("exiting zero_fill\n");
}

//point is initialized to parent coords = its coords f=g=h=0
static void init_point(struct point *p, float y, float x)
{
    //printf("entering init_point\n");
    p->parent_x = x;
    p->parent_y = y;
    p->h = 0.0;
    p->g = 0.0;
    p->f = 0.0;
    //printf("exiting init_point\n");
}

//initialize point matrice to parent coords = self and h=g=f=max value
static void init_mat_point(int height, int width)
{
    //printf("entering init_mat_point\n");
    for(int i = 0; i < height; ++i)
    {
        for(int j = 0; j < width; ++j)
        {
            //printf("i = %d, j = %d\n", i, j);
            mat_point[i][j].parent_x = -1;
            mat_point[i][j].parent_y = -1;
            mat_point[i][j].h = -1;
            mat_point[i][j].g = -1;
            mat_point[i][j].f = -1;
        }
    }
    //printf("exiting init_mat_point\n");
}

int line_of_sight(struct map *map, float parent_y, float parent_x, float y, float x)
{
    float old_x = parent_x;
    float old_y = parent_y;
    printf("entering line_of_sight\n");
    float sy, sx;
    float dy = y - parent_y;
    float dx = x - parent_x;
    float f = 0;
    if (dy < 0)
    {
        dy *= -1;
        sy = -1;
    }
    else
        sy = 1;
    if (dx < 0)
    {
        dx *= -1;
        sx = -1;
    }
    else
        sx = 1;
    if (dx >= dy)
    {
        while (parent_x != x)
        {
            f = f + dy;
            if (f >= dx)
            {
                if (blocked(map, parent_y + ((sy - 1) / 2), parent_x + ((sx - 1) / 2)))
                    return 0;
                parent_y = parent_y + sy;
                f = f - dx;
            }
            if (f != 0 && blocked(map, parent_y + ((sy - 1) / 2), parent_x + ((sx - 1) / 2)))
                return 0;
            if (!dy && blocked(map, parent_y, parent_x + ((sx - 1) / 2)) && blocked(map, parent_y - 1, parent_x + ((sx - 1) / 2)))
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
                if (blocked(map, parent_y + ((sy - 1) / 2), parent_x + ((sx - 1) / 2)))
                    return 0;
                parent_x += sx;
                f = f - dy;
            }
            if (f != 0 && blocked(map, parent_y + ((sy - 1) / 2), parent_x + ((sx - 1) / 2)))
                return 0;
            if (!dx && blocked(map, parent_y + ((sy - 1) / 2), parent_x) && blocked(map, parent_y + ((sy - 1) / 2), parent_x - 1))
                return 0;
            parent_y += sy;   
        }
    }
    printf("yay there is a line of sight between %.1f %.1f and %.1f %.1f\n", old_y, old_x, y, x);
    return 1;
}
/*
struct point parent(struct point p)
{
    return (mat_point[p.parent_y][p.parent_x]);
}
*/
//makes and returns vector2 with corresponding coordinates
static struct vector2 make_vec2(float y, float x)
{
    //printf("entering lake_vec2\n");
    struct vector2 v = {.x = x, .y = y};
    return v;
}

//calculates value of point.h at corresponding coords
static double get_c(int row, int col, int finish_x, int finish_y)
{
    //printf("entering get_h\n");
    return ((double)sqrt((row - finish_y)*(row - finish_y) + \
                (col - finish_x)*(col - finish_x)));
}

void ComputeCost(struct map *map, int old_y, int old_x, int y, int x)
{
    printf("entering ComputeCost\n");
    double new_g;
    struct point par = mat_point[old_y][old_x];
    printf("old_y %.0f old_x %.0f\n", par.parent_y, par.parent_x);
    struct point gp = mat_point[(int)par.parent_y][(int)par.parent_x];
    if (line_of_sight(map, par.parent_y, par.parent_x, y, x)\
        && line_of_sight(map, par.parent_y, par.parent_x, y + 1, x + 1)
        && line_of_sight(map, par.parent_y + 1, par.parent_x + 1, y, x))
    {
        printf("entering first if\n");
        new_g = (gp.g + get_c((int)par.parent_y, (int)par.parent_x, y, x));
        if(mat_point[y][x].g < 0 || new_g < mat_point[y][x].g)
        {
            printf("entering second if\n");
            mat_point[y][x].parent_x = par.parent_x;
            mat_point[y][x].parent_y = par.parent_y;
            mat_point[y][x].g = new_g;
        }
    }
    else
    {
        printf("entering else\n");
        new_g = mat_point[old_y][old_x].g + get_c(old_y, old_x, y, x);
        if(mat_point[y][x].g < 0 || new_g < mat_point[y][x].g)
        {
        printf("entering if of else\n");
            mat_point[y][x].parent_x = old_x;
            mat_point[y][x].parent_y = old_y;
            mat_point[y][x].g = new_g;
        }
    }
    printf("exiting ComputeCost\n");
}

/*Builds path vector in correct order
  doesn't include starting coords because they could be float but path vect
  only has rounded numbers*/
static void get_path(int row, int col)
{
    printf("entering get_path\n");
    int row_tmp;
    struct vector *path = vector_init(8);
    while(!(mat_point[row][col].parent_y == row && \
                mat_point[row][col].parent_x == col))
    {
        path = vector_insert(path, 0, make_vec2((float)(row) + 0.5, (float)(col) + 0.5));
        printf("exiting make_vec2\n");
        row_tmp = mat_point[row][col].parent_y;
        col = mat_point[row][col].parent_x;
        row = row_tmp;
    }
    
    list_size++;
    list_p = realloc(NULL, list_size * sizeof(struct vector *));
    list_p[list_size-1] = path;
    printf("exiting get_path\n");
}
/*explore point mat_point[row][col] next to mat_point[row_static][col_static]
  and finds end or modifies f, g, h values if needed*/
static void explore(struct map *map, int row, int col, struct vector *open)
{
    printf("entering explore\n");
    if(goal)
        return;
    if(in_limit(row, col) && !squeezed(map, row_static, col_static, row, col))
    {
        if(arrived(map, row, col)) //we reached end point
        {
            printf("arrived %d %d\n", row, col);
            ComputeCost(map, row_static, col_static, row, col);
            get_path(row, col); //trace back path
            goal = 1; //YAY we arrived at the finish line
            return;
        }

        if (!closed[row][col] && !blocked(map, row, col)) //not a block 
        {                                            //and not explored 
            ComputeCost(map, row_static, col_static, row, col);
            double new_h = get_c(row, col, (int)finish->y, (int)finish->x);
            printf("exiting get_h\n");
            printf("%d %d\n", row, col);
            double new_f = mat_point[row][col].g + new_h;
            if(mat_point[row][col].f < 0 || mat_point[row][col].f > new_f)
            {
                struct vector2 tmp = make_vec2(row, col);
                open = vector_remove_v2(open, tmp);
                printf("\ngot to append in size: %lu capacity: %lu\n\n", open->size, open->capacity);
                open = vector_append(open, tmp); //to be explored
                vector_print(open);
                printf("exiting make_vec2\n");
                mat_point[row][col].h = new_h;
                mat_point[row][col].f = new_f;
            }

        }
    }
    printf("exiting explore\n");
}

//explore all 8 directions
static void explore_all(struct map *map, 
    struct vector *open)
{
    printf("entering explore_all\n");
    explore(map, row_static - 1, col_static, open); //above
    explore(map, row_static + 1, col_static, open); //under
    explore(map, row_static, col_static - 1, open); //left
    explore(map, row_static, col_static + 1, open); //right
    explore(map, row_static - 1, col_static - 1, open); //upper-left
    explore(map, row_static - 1, col_static + 1, open); //upper-right
    explore(map, row_static + 1, col_static - 1, open); //lower-left
    explore(map, row_static + 1, col_static + 1, open); //lower-right
    printf("exiting explore_all\n");
}

//initializing some static variables
static void init_static()
{
    printf("entering init_static\n");
    closed = malloc(map_height * (sizeof(int *)));
    mat_point = malloc(map_height * sizeof(struct point *));
    for(int i = 0; i < map_height; ++i)
    {
        closed[i] = malloc(map_width * sizeof(int));
        mat_point[i] = malloc(map_width * sizeof(struct point));
    }
    zero_fill(closed, map_height, map_width); 
    init_mat_point(map_height, map_width);
    printf("exiting init_static\n");
}

static void free_all(struct vector *open)
{
    printf("entering free_all\n");
    for(int i = 0; i < map_height; ++i)
    {
        free(closed[i]);
        free(mat_point[i]);
    }
    free(closed);
    free(mat_point);
    free(finish);
    vector_destroy(open);
    printf("exiting free_all\n");
}
/*
static int is_checkpoint(int y, int x, struct vector *path)
{
    for(size_t i = 0; i < path->size; ++i)
    {
        if((int)path->data[i].y == y && (int)path->data[i].x == x)
            return 1;
    }
    return 0;
}
*/

static char floor_to_char(int y, int x, struct map *map)
{
    //printf("entering floor_to_char\n");
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
    //printf("exiting floor_to_char\n");
}

static char **create_matrix(struct vector *path, struct map *map)
{
    //printf("entering create_matrix\n");
    char **mat = malloc(map->height * sizeof(char *));
    for(int i = 0; i < map_height; ++i)
    {
        mat[i] = malloc(map_width * sizeof(char));
    }
    for(int i = 0; i < map_height; ++i)
    {
        for(int j = 0; j < map_width; ++j)
        {
            mat[i][j] = floor_to_char(i, j, map);
        }
    }
    //printf("path size = %lu\n", path->size);
    for(size_t i = 0; i < path->size; ++i)
    {
        mat[(int)path->data[i].y][(int)path->data[i].x] += ('A' - 'a');
    }
    //printf("exiting create_matrix\n");
    return mat;
}

static void free_mat(char **mat, int height)
{
    //printf("entering free_mat\n");
    for(int i = 0; i < height; ++i)
    {
        free(mat[i]);
    }
    free(mat);
    //printf("exiting free_mat\n");
}

static void print_cell(char c)
{
    //printf("entering print_cell\n");
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
    //printf("exiting print_cell\n");
}

static void print_map(struct vector *path, struct map *map)
{
    //printf("entering print_map\n");
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
    //printf("exiting print_map\n");
}

static struct vector *get_smallest_path(void)
{
    if(!list_size)
        return NULL;
    struct vector *res = list_p[0];
    for(int i = 1; i < list_size; ++i)
    {
        if(res->size > list_p[i]->size)
        {
            res = list_p[i];
        }
    }
    return res;
}

/* A* path finding algorithm on map
   FIXME modify me to become Theta* any-angle algorithm to reduce actions*/
struct vector *find_path(struct map *map)
{
    printf("entering findpath\n");
    get_width(map); //initialize static map_width and map_height

    struct vector2 start; //starting point
    start.x = map_get_start_x(map);
    start.y = map_get_start_y(map);

    find_finish(map); //set static vector2 finish variable;
    if(finish)
    {
        printf("finish is %d %d\n", (int)finish->y, (int)finish->x);
    }
    //if initial conditions don't make sense
    if(!in_limit_vec(&start) || !in_limit_vec(finish) || blocked_vec(map, \
                &start) || blocked_vec(map, finish) || arrived_vec(map, &start))
    {
        printf("exiting in_limit_vec false * 2, blocked true * 2, arrived_vec true\n");
        goto fail_init; //frees memory and returns NULL(path didn't change);
    }

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
        explore_all(map, open);
        //if(goal)
        //{
        //    break;
        //}
    }
    if(goal)
        printf("goal found !!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    else
        printf("no goal lol\n");
    struct vector *path = get_smallest_path();
    print_map(path, map);
    printf("\nlist_size%d\n\n", list_size);

fail_init: //free allocated stuff and return the path(NULL if not found)
    free_all(open);
    if(path)
    {
        printf("path found\n");
        printf("path size = %lu", path->size);
        printf("capacity = %lu", path->capacity);
    }
    vector_print(path);
    return path;
}
