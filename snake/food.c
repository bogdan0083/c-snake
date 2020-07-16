//
// Created by dbogd on 14.07.2020.
//

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define SDL_DISABLE_IMMINTRIN_H
#include <SDL.h>
#include "defs.h"
#include "snake.h"
#include "food.h"
#include "game.h"

static void draw(Food *food, Game *game, Snake *snake, SDL_Renderer *ren) {
    SDL_SetRenderDrawColor(ren, 200, 0, 0, 255);
    SDL_RenderDrawRect(ren, food->collisionRect);
    SDL_RenderFillRect(ren, food->collisionRect);
}

static void set_position(Food *food, Game *game, Snake *snake, SDL_Renderer *ren) {
    int rand_x = rand() % (WINDOW_WIDTH - FOOD_SIZE);
    int rand_y = rand() % (WINDOW_HEIGHT - FOOD_SIZE);

    food->collisionRect->x = rand_x;
    food->collisionRect->y = rand_y;

    food->boundingRect->x = (rand_x - 15);
    food->boundingRect->y = (rand_y - 15);

    while(food->intersects_with_snake(food->boundingRect, snake)) {
        int new_rand_x = rand() % (WINDOW_WIDTH - FOOD_SIZE);
        int new_rand_y = rand() % (WINDOW_HEIGHT - FOOD_SIZE);
        food->collisionRect->x = new_rand_x;
        food->collisionRect->y = new_rand_y;

        food->boundingRect->x = (new_rand_x - 15);
        food->boundingRect->y = (new_rand_y - 15);
    }
}

static bool intersects_with_snake(SDL_Rect *foodBoundingRect, Snake *snake) {

    bool ret = false;

    for (int i = 0; i < snake->parts->count; ++i) {
        SnakePart *part = snake->parts->buf[i];
        SDL_bool has_intersection = SDL_HasIntersection(foodBoundingRect, part->rect);
        SDL_bool has_clone_intersection = part->show_clone && SDL_HasIntersection(foodBoundingRect, part->rect);

        if (has_intersection || has_clone_intersection) {
            ret = true;
            break;
        }
    }

    return ret;
}

Food *food_new() {
    Food *food = calloc(1, sizeof(Food));

    food->is_active = false;
    food->collisionRect = calloc(1, sizeof(SDL_Rect));
    food->boundingRect = calloc(1, sizeof(SDL_Rect));
    food->collisionRect->w = FOOD_SIZE;
    food->collisionRect->h = FOOD_SIZE;
    food->boundingRect->w = FOOD_SIZE * 4;
    food->boundingRect->h = FOOD_SIZE * 4;

    food->draw = &draw;
    food->set_position = &set_position;
    food->intersects_with_snake = &intersects_with_snake;

    return food;
}

