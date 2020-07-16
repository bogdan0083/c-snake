//
// Created by dbogd on 14.07.2020.
//

#ifndef C_GAMES_FOOD_H
#define C_GAMES_FOOD_H

#include <stdbool.h>
#include <stdio.h>
#define SDL_DISABLE_IMMITRIN_H
#include <SDL.h>
#include "snake.h"
#include "game.h"

#define FOOD_SIZE 10
#define FOOD_BOUNDING_BOX_SIZE 40

struct food_s {
    bool is_active;
    SDL_Rect *collisionRect;
    SDL_Rect *boundingRect;
    void (*draw)(struct food_s *food, struct game_s *game, struct snake_s *snake, SDL_Renderer *ren);
    void (*set_position)(struct food_s *food, struct game_s *game, struct snake_s *snake, SDL_Renderer *ren);
    void (*intersects_with_snake)(SDL_Rect *foodBoundingRect, struct snake_s *snake);
};

#endif //C_GAMES_FOOD_H
