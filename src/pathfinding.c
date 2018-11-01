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
static int map_width = 0;
static int map_height = 0;
static int row_static, col_static; //postion of the point being explored
static int goal = 0; //will be set to one when path is found
//list of points to be opened
static struct vector *open = NULL;
static struct vector *path = NULL; //modified if path is found
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

static struct point **mat_point;

//initializes map_height and map_width
static void get_width(struct map *map)
{
    if(!map_width && !map_height)
    {
        map_width = map->width;
        map_height = map->height;
    }
}

//check if vector2 is inside map
static int in_limit_vec(struct vector2 *v)
{
    if(!v)
        return 0;
    return ((v->x >= 0 && v->x < (float)map_width) && \
            (v->y >= 0 && v->y < (float)map_height));
}

//check if coords are inside map
static int in_limit(int y, int x)
{
    return (y >= 0 && y < map_height && x >= 0 && x < map_width);
}

//get floortype of vector2
static enum floortype floor_vect(struct map *map, struct vector2 *v)
{
    return map_get_floor(map, (int)v->x, (int)v->y);
}

// return true if vector2 is a BLOCK floortype
static int blocked_vec(struct map *map, struct vector2 *v)
{
    return (floor_vect(map, v) == BLOCK);
}

// return true if coords is BLOCK floortype
static int blocked(struct map *map, int row, int col)
{
    return(map_get_floor(map, col, row) == BLOCK);
}

// return true if vector2 is FINISH BLOCK
static int arrived_vec(struct map *map, struct vector2 *v)
{
    return (floor_vect(map, v) == FINISH);
}

// return true if coords is FINISH BLOCK
static int arrived(struct map *map, int row, int col)
{
    return (map_get_floor(map, col, row) == FINISH);
}

//Finds vector2 with floortype FINISH in map
//FIXME find closest finish instead of first one
static void find_finish(struct map *map)
{
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
static void init_mat_point(struct point **mat_point, int height, int width)
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

//makes and returns vector2 with corresponding coordinates
struct vector2 make_vec2(float y, float x)
{
    struct vector2 v = {.y = y, .x = x};
    return v;
}

//calculates value of point.h at corresponding coords
double get_h(int row, int col)
{
    int finish_x = (int)finish->x;
    int finish_y = (int)finish->y;
    return ((double)sqrt((row - finish_y)*(row - finish_y) + \
                (col - finish_x)*(col - finish_x)));
}


/*Builds path vector with first last step first and first step last
  doesn't include starting coords because they could be float but path vect
  only has rounded numbers*/
void get_path(int row, int col)
{
    int row_tmp;
    if(!path)
    {
        path = vector_init(8);
    }
    while(!(mat_point[row][col].parent_y == row && \
                mat_point[row][col].parent_x == col))
    {
        path = vector_append(path, make_vec2(row, col));
        row_tmp = mat_point[row][col].parent_y;
        col = mat_point[row][col].parent_x;
        row = row_tmp;
    }
}
/*explore point mat_point[row][col] next to mat_point[row_static][col_static]
  and finds end or modifies f, g, h values if needed*/
void explore(struct map *map, int row, int col, double factor)
{
    if(goal)
        return;
    if(in_limit(row, col)) //sanity check
    {
        if(arrived(map, row, col)) //we reached end point
        {
            mat_point[row][col].parent_x = col_static; 
            mat_point[row][col].parent_y = row_static;
            get_path(row, col); //trace back path
            goal = 1; //YAY we arrived at the finish line
            return;
        }

        if (!closed[row][col] && !blocked(map, row, col)) //not a block 
        {                                            //and not explored
            double new_g = mat_point[row][col].g + factor; 
            double new_h = get_h(row, col);
            double new_f = new_g + new_h;
            if(mat_point[row][col].f > new_f)
            {
                open = vector_append(open, make_vec2(row, col)); //to be explored
                mat_point[row][col].parent_x = col_static;
                mat_point[row][col].parent_y = row_static;
                mat_point[row][col].h = new_h;
                mat_point[row][col].f = new_f;
                mat_point[row][col].g = new_g;
            }

        }
    }
}

//explore all 8 directions
void explore_all(struct map *map)
{
    explore(map, row_static - 1, col_static, 1); //above
    explore(map, row_static + 1, col_static, 1); //under
    explore(map, row_static, col_static - 1, 1); //left
    explore(map, row_static, col_static + 1, 1); //right
    explore(map, row_static - 1, col_static - 1, 1.414); //upper-left
    explore(map, row_static - 1, col_static + 1, 1.414); //upper-right
    explore(map, row_static + 1, col_static - 1, 1.414); //lower-left
    explore(map, row_static + 1, col_static + 1, 1.414); //lower-right
}

//initializing some static variables
void init_static()
{
    closed = malloc(map_height * (sizeof(int *)));
    mat_point = malloc(map_height * sizeof(struct point *));
    for(int i = 0; i < map_height; ++i)
    {
        closed[i] = malloc(map_width * sizeof(int));
        mat_point = malloc(map_width * sizeof(struct point));
    }
    zero_fill(closed, map_height, map_width); 
    init_mat_point(mat_point, map_height, map_width);
    open = vector_init(8);

}

void free_all()
{
    for(int i = 0; i < map_height; ++i)
    {
        free(closed[i]);
        free(mat_point[i]);
    }
    free(closed);
    free(mat_point);
    free(finish);
    vector_destroy(open);
}
/* A* path finding algorithm on map
   FIXME modify me to become Theta* any-angle algorithm to reduce actions*/
struct vector *find_path(struct map *map)
{

    get_width(map); //initialize static map_width and map_height


    struct vector2 start; //starting point
    start.x = map_get_start_x(map);
    start.y = map_get_start_y(map);

    find_finish(map); //set static vector2 finish variable;
    //if initial conditions don't make sense
    if(!in_limit_vec(&start) || !in_limit_vec(finish) || blocked_vec(map, \
                &start) || blocked_vec(map, finish) || arrived_vec(map, &start))
        goto fail_init; //frees memory and returns NULL(path didn't change);

    init_static(); //initializing mat_point, closed and open static variables

    row_static = (int)start.y, col_static = (int)start.x;
    //parent=self h=f=g=0
    init_point(&mat_point[row_static][col_static], row_static, col_static);

    //add start point to soon to be opened points
    open = vector_append(open, make_vec2(row_static, col_static));

    while(open->size) //something to be opened
    {
        //FIXME: open smallest one with smallest f in mat_point
        //instead of first one
        struct vector2 cur = open->data[0]; //first element of vect
        open = vector_remove(open, 0); //removing it
        row_static = cur.y, col_static = cur.x; //updating static coordinates
        closed[row_static][col_static] = 1; //explored
        explore_all(map);
        if(goal)
            break;
    }

fail_init: //free allocated stuff and return the path(NULL if not found)
    free_all();
    return path;

}

int is_checkpoint(int y, int x, struct vector *path)
{
    for(size_t i = 0; i < path->size; ++i)
    {
        if((int)path->data[i].y == y && (int)path->data[i].x == x)
            return 1;
    }
    return 0;
}


char floor_to_char(int y, int x, struct map *map)
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

char **create_matrix(struct map *map, struct vector *path)
{
    char **mat = malloc(map->height * sizeof(char *));
    for(int i = 0; i < map->height; ++i)
    {
        malloc(map->width * sizeof(char));
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

void free_mat(char **mat, int height)
{
    for(int i = 0; i < height; ++i)
    {
        free(mat[i]);
    }
    free(mat);
}

void print_cell(char c)
{
    if(c < 'a')
    {
        printf("%s%c", KRED, c);
    }
    switch (c)
    {
        case 'r':
            printf("%s%c", KWHT, c);
            break;
        case 'g':
            printf("%s%c", KGRN, c);
            break;
        case 'b':
            printf("%s%c", KBLU, c);
            break;
        case 'f':
            printf("%s%c", KYEL, c);
            break;
    }
}

void print_map(struct map *map, struct vector *path)
{
    char **mat = create_matrix(map, path);
    for(int i = 0; i < map->height; ++i)
    {
        for(int j = 0; j < map->width; ++j)
        {
            print_cell(mat[i][j]);
        }
        printf("\n");
    }
}

