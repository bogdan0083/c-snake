//
// Created by dbogd on 26.06.2020.
//

// @TODO: split the code into separate files

#include <stdio.h>
#define SDL_DISABLE_IMMINTRIN_H
#include <SDL.h>
#include "array.h"
#include "game.h"
#include "snake.h"
#include "defs.h"
#include <time.h>
#include <stdlib.h>

GENERATE_ARRAY_SOURCE_FOR_TYPE(SnakePart, snkprt);
GENERATE_ARRAY_SOURCE_FOR_TYPE(Point, point);

int main() {
    srand(time(NULL));   // Initialization, should only be called once.

    SDL_Window *win;
    SDL_Renderer *ren;
    SDL_Event event;

    // Initialize game object
    Game *game = game_new();


    // Initialize SDL
    game->sdl_init(&win, &ren);

    game->set_background(game, ren);

    for (;;) {
        game->poll_keyboard_events(game, &event);

        if (game->status == PAUSED) {
            game->draw_pause_screen(game, ren);
        }

        if (game->status == NOT_ACTIVE) {
            game->draw_initial_screen(game, ren);
        }

        if (game->status == ACTIVE) {
            game->run(game, ren);
        }

        if (game->status == GAME_OVER) {
            game->draw_gameover_screen(game, ren);
        }

        SDL_RenderPresent(ren);
        SDL_Delay(1000 / FRAMES_PER_SEC);
    }
}

