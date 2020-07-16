//
// Created by dbogd on 26.06.2020.
//

#include <stdio.h>
#define SDL_DISABLE_IMMINTRIN_H
#include <SDL.h>
#include <SDL_ttf.h>
#include <assert.h>
#include "game.h"
#include "defs.h"
#include "snake.h"
#include "food.h"

/*
 * Run the game. Initializie snake and food objects (if not already initialized)
 */
static void run(Game *game, SDL_Renderer *ren) {

    if (game->current_screen != BOARD) {
        return;
    }

    SDL_RenderClear(ren);

    game->set_background(game, ren);

    if (!game->initialized) {
        game->food = food_new();
        game->snake = snake_new();
        game->initialized = true;
    }

    Food *food = game->food;
    Snake *snake = game->snake;

    if (!food->is_active) {
        food->set_position(food, game, snake, ren);
        food->is_active = true;
    }

    snake->check_for_collision(snake, game, food);
    snake->update_position(snake, ren);
    food->draw(game->food, game, snake, ren);
}

/*
 * Init gameover part of the game. Free snake and food objects
 */
static void run_gameover(Game *game) {
    game->status = GAME_OVER;
    free(game->snake);
    free(game->food);
    game->initialized = false;
}

static void poll_keyboard_events(Game *game, SDL_Event *event) {
    while (SDL_PollEvent(event)) {
        switch (event->type) {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                game->handle_keydown(game, game->snake, event);
                break;
        }
    }
}

static void handle_keydown(Game *game, Snake *snake, SDL_Event *event) {
    switch (event->key.keysym.sym) {
        case SDLK_q:
            SDL_Quit();
            break;
        case SDLK_w:
        case SDLK_UP:
            snake->change_direction(snake, UP);
            break;
        case SDLK_d:
        case SDLK_RIGHT:
            snake->change_direction(snake, RIGHT);
            break;
        case SDLK_s:
        case SDLK_DOWN:
            snake->change_direction(snake, DOWN);
            break;
        case SDLK_a:
        case SDLK_LEFT:
            snake->change_direction(snake, LEFT);
            break;
        case SDLK_SPACE:
            game->handle_space_keydown(game);
            break;
    }
}

static void handle_space_keydown(Game *game) {
    if (game->status == NOT_ACTIVE || game->status == PAUSED) {
        game->status = ACTIVE;
        game->current_screen = BOARD;
        return;
    }

    if (game->status == ACTIVE) {
        game->status = PAUSED;
        return;
    }

    if (game->status == GAME_OVER) {
        game->status = ACTIVE;
        game->current_screen = BOARD;
        return;
    }
}

static void set_background(Game *game, SDL_Renderer *ren) {
    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    SDL_RenderClear(ren);
}

static void sdl_init(SDL_Window **win, SDL_Renderer **ren) {

    /*
     * Initialize SDL components
     */
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        exit(0);
    }

    /*
     * Initialize SDL window
     */
    *win = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
                            SDL_WINDOW_SHOWN);

    if (*win == NULL) {
        exit(0);
    }

    /*
     * Initialize SDL renderer
     */
    *ren = SDL_CreateRenderer(*win, -1, SDL_RENDERER_ACCELERATED);

    if (*ren == NULL) {
        exit(0);
    }
}

static void draw_initial_screen(Game *game, SDL_Renderer *ren) {
    SDL_RenderClear(ren);
    game->draw_text(game, ren, "Snake Game", 0, WINDOW_WIDTH / 2, 200);
    game->draw_text(game, ren, "press SPACE to play", 0, WINDOW_WIDTH / 2, 250);
}

/*
 * Draw text to our renderer
 */
static void draw_text(Game *game, SDL_Renderer *ren, char *fstr, int value, int x, int y) {
    assert(game->font);
    int w, h;
    char msg[80];
    snprintf(msg, 80, fstr, value);
    TTF_SizeText(game->font, msg, &w, &h);
    SDL_Color color = {0, 0, 0};
    SDL_Surface *msgsurf = TTF_RenderText_Blended(game->font, msg, color);
    SDL_Texture *msgtex = SDL_CreateTextureFromSurface(ren, msgsurf);
    SDL_SetTextureColorMod(msgtex, 0, 0, 0);
    SDL_Rect fromrec = {0, 0, msgsurf->w, msgsurf->h};
    SDL_Rect torec = {x - (w / 2), y, msgsurf->w, msgsurf->h};
    SDL_RenderCopy(ren, msgtex, &fromrec, &torec);
    SDL_DestroyTexture(msgtex);
    SDL_FreeSurface(msgsurf);
}

static void draw_pause_screen(Game *game, SDL_Renderer *ren) {
    assert(game->font);
    if (game->current_screen != PAUSE) {
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 10);
        SDL_Rect *overlay = malloc(sizeof(SDL_Rect));
        overlay->x = 0;
        overlay->y = 0;
        overlay->w = WINDOW_WIDTH;
        overlay->h = WINDOW_HEIGHT;

        SDL_RenderDrawRect(ren, overlay);
        SDL_RenderFillRect(ren, overlay);

        game->draw_text(game, ren, "PAUSE", 0, WINDOW_WIDTH / 2, 200);
        game->draw_text(game, ren, "press SPACE to resume", 0, WINDOW_WIDTH / 2, 250);
        game->current_screen = PAUSE;
    }
}

static void draw_gameover_screen(Game *game, SDL_Renderer *ren) {
    assert(game->font);
    if (game->current_screen != GAMEOVER) {
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 10);
        SDL_Rect *overlay = malloc(sizeof(SDL_Rect));
        overlay->x = 0;
        overlay->y = 0;
        overlay->w = WINDOW_WIDTH;
        overlay->h = WINDOW_HEIGHT;

        SDL_RenderDrawRect(ren, overlay);
        SDL_RenderFillRect(ren, overlay);

        game->draw_text(game, ren, "GAME OVER", 0, WINDOW_WIDTH / 2, 200);
        game->draw_text(game, ren, "press SPACE to play again", 0, WINDOW_WIDTH / 2, 250);
        game->current_screen = GAMEOVER;
    }
}

/*
 * Create Game object
 */
Game *game_new() {
    Game *game;
    game = (Game *) malloc(sizeof(Game));

    game->food = malloc(sizeof(Food));
    game->snake = malloc(sizeof(Snake));
    game->score = 0;

    TTF_Init();
    game->font = TTF_OpenFont("res/LiberationSans-Regular.ttf", 28);

    game->status = NOT_ACTIVE;
    game->current_screen = INITIAL;

    game->initialized = false;
    game->food_initialized = false;

    game->run = &run;
    game->sdl_init = &sdl_init;
    game->poll_keyboard_events = &poll_keyboard_events;
    game->handle_keydown = &handle_keydown;
    game->handle_space_keydown = &handle_space_keydown;
    game->set_background = &set_background;
    game->draw_initial_screen = &draw_initial_screen;
    game->draw_text = &draw_text;
    game->draw_pause_screen = &draw_pause_screen;
    game->draw_gameover_screen = &draw_gameover_screen;
    game->run_gameover = &run_gameover;

    return game;
}

