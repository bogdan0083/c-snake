//
// Created by dbogd on 04.07.2020.
//

#ifndef C_GAMES_DEFS_H
#define C_GAMES_DEFS_H

#include <stdio.h>
#include "array.h"

enum direction_e { UP, LEFT, DOWN, RIGHT };

struct point_s {
    int x;
    int y;
};

typedef enum direction_e Direction;
typedef struct point_s Point;

GENERATE_ARRAY_HEADER_FOR_TYPE(Point, point);

typedef struct point_array_s PointArray;

#endif //C_GAMES_DEFS_H
