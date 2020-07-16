//
// Created by dbogd on 26.06.2020.
//

#ifndef C_GAMES_GAME_H
#define C_GAMES_GAME_H

#include <stdio.h>
#define SDL_DISABLE_IMMINTRIN_H
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include "defs.h"
#include "snake.h"
#include "food.h"

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500
#define FRAMES_PER_SEC 40

enum gamestatus_e {
    NOT_ACTIVE, ACTIVE, GAME_OVER, PAUSED
};

enum gamescreen_e {
    INITIAL, BOARD, PAUSE, GAMEOVER
};

enum collision_target_kind_e {
    SNAKEPART, FOOD, NOTHING
};

union collision_target_u {
    struct snakepart_s *snakepart;
};

struct collision_s {
    union collision_target_u *target;
    enum collision_target_kind_e kind;
};

struct game_s {
    int score;
    enum gamestatus_e status;
    enum gamescreen_e current_screen;
    struct food_s *food;
    struct snake_s *snake;
    TTF_Font *font;

    bool initialized;
    bool food_initialized;

    void (*run)(struct game_s *game, SDL_Renderer *ren);
    void (*run_gameover)(struct game_s *game);

    void (*draw_text)(struct game_s *game, SDL_Renderer *ren, char *fstr, int value, int x, int y);
    void (*draw_initial_screen)(struct game_s *game, SDL_Renderer *ren);
    void (*draw_pause_screen)(struct game_s *game, SDL_Renderer *ren);
    void (*draw_gameover_screen)(struct game_s *game, SDL_Renderer *ren);

    void (*sdl_init)(SDL_Window **win, SDL_Renderer **ren);

    void (*poll_keyboard_events)(struct game_s *game, SDL_Event *event);

    void (*handle_keydown)(struct game_s *game, struct snake_s *snake, SDL_Event *event);
    void (*handle_space_keydown)(struct game_s *game);

    void (*set_background)(struct game_s *game, SDL_Renderer *ren);

};

struct game_s *game_new();

#endif //C_GAMES_GAME_H
