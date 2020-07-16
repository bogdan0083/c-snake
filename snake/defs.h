//
// Created by dbogd on 04.07.2020.
//

#ifndef C_GAMES_DEFS_H
#define C_GAMES_DEFS_H

#include <stdio.h>
#include "game.h"
#include "snake.h"
#include "food.h"
#include "array.h"

enum direction_e { UP = 1, LEFT = 2, DOWN = 3, RIGHT = 4 };

struct point_s {
    int x;
    int y;
};

typedef struct game_s Game;
typedef struct food_s Food;
typedef struct snake_s Snake;
typedef enum direction_e Direction;
typedef struct point_s Point;
typedef enum gamestatus_e GameStatus;
typedef enum gamescreen_e GameScreen;
typedef struct snakepart_s SnakePart;
typedef union collision_target_u CollisionTarget;
typedef struct collision_s Collision;

GENERATE_ARRAY_HEADER_FOR_TYPE(SnakePart, snkprt);
GENERATE_ARRAY_HEADER_FOR_TYPE(Point, point);

typedef struct snkprt_array_s SnakePartArray;
typedef struct point_array_s PointArray;

#endif //C_GAMES_DEFS_H
