//
// Created by dbogd on 25.06.2020.
//

#ifndef C_GAMES_SNAKE_H
#define C_GAMES_SNAKE_H

#include <stdbool.h>
#include <stdio.h>
#define SDL_DISABLE_IMMINTRIN_H
#include <SDL.h>
#include "array.h"
#include "defs.h"
#include "game.h"

#define INITIAL_SNAKE_POSITION_X 40
#define INITIAL_SNAKE_POSITION_Y 40
#define INITIAL_SNAKE_LENGTH 40

// velocity per frame
#define SNAKE_VELOCITY 2

#define SNAKE_THICKNESS 10

struct snakepart_s {
    SDL_Rect *rect;
    SDL_Rect *collisionRect;
    struct point_s *from;
    struct point_s *to;
    enum direction_e direction;
    struct snakepart_s *clone;
    bool show_clone;
};

struct snake_s {
    struct snkprt_array_s *parts;
    enum direction_e direction;
    int grow_compensation;

    void (*draw_parts)(struct snake_s *snake, struct SDL_Renderer *ren);

    void (*draw_collision_rect)(struct snake_s *snake, struct snakepart_s *part, struct SDL_Renderer *ren);

    void (*add_part)(struct snake_s *snake, enum direction_e direction);

    void (*update_position)(struct snake_s *snake, struct SDL_Renderer *ren);

    void (*update_clone_position)(struct snake_s *snake, struct snakepart_s *snake_part);

    void (*update_collision_rect)(struct snake_s *snake, struct snakepart_s *snake_part);

    void (*change_direction)(struct snake_s *snake, enum direction_e new_direction);

    void
    (*change_part_direction)(struct snake_s *snake, struct snakepart_s *snake_part, enum direction_e new_direction);

    void (*replace_snakepart_with_clone)(struct snake_s *snake, struct snakepart_s *snake_part);

    void (*add_joint)(struct snake_s *snake);

    void (*expand_head)(struct snake_s *snake, struct snakepart_s *head);

    void (*check_for_collision)(struct snake_s *snake, struct game_s *game, struct food_s *food);

    void (*move_head)(struct snake_s *snake);

    void (*shrink_tail)(struct snake_s *snake, struct snakepart_s *tail);

    bool (*is_tail_shrinked)(struct snake_s *snake, struct snakepart_s *tail);

    bool (*is_head)(struct snake_s *snake, struct snakepart_s *snake_part);

    bool (*is_head_clone)(struct snake_s *snake, struct snakepart_s *snake_part);

    bool (*is_tail)(struct snake_s *snake, struct snakepart_s *snake_part);

    bool (*is_opposite_direction)(struct snake_s *snake, enum direction_e new_direction);

    bool (*is_snakepart_out_of_bounds)(struct snake_s *snake, struct snakepart_s *snake_part);

    int (*get_snakepart_clone_x)(struct snake_s *snake, struct snakepart_s *snake_part);

    int (*get_snakepart_clone_y)(struct snake_s *snake, struct snakepart_s *snake_part);

    struct snakepart_s *(*get_head)(struct snake_s *snake);
};

struct snake_s *snake_new();

typedef struct snake_s Snake;
typedef struct snakepart_s SnakePart;

/* TODO: rename snkprt to snakepart */
GENERATE_ARRAY_HEADER_FOR_TYPE(SnakePart, snkprt);

typedef struct snkprt_array_s SnakePartArray;

#endif //C_GAMES_SNAKE_H
