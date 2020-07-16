#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#define SDL_DISABLE_IMMINTRIN_H
#include <SDL.h>
#include <math.h>
#include "snake.h"
#include "defs.h"

static SnakePart *snakepart_new() {

    SnakePart *p = malloc(sizeof(SnakePart));
    Point *from = malloc(sizeof(Point));
    Point *to = malloc(sizeof(Point));
    p->clone = malloc(sizeof(SnakePart));
    p->clone->from = malloc(sizeof(Point));
    p->clone->to = malloc(sizeof(Point));
    p->clone->rect = malloc(sizeof(SDL_Rect));
    p->clone->collisionRect = malloc(sizeof(SDL_Rect));

    p->from = from;
    p->to = to;

    p->from->x = INITIAL_SNAKE_POSITION_X;
    p->from->y = INITIAL_SNAKE_POSITION_Y;

    p->to->x = INITIAL_SNAKE_POSITION_X + INITIAL_SNAKE_LENGTH;
    p->to->y = INITIAL_SNAKE_POSITION_Y + SNAKE_THICKNESS;

    // init rect
    p->rect = malloc(sizeof(SDL_Rect));
    p->rect->x = p->from->x;
    p->rect->y = p->from->y;
    p->rect->w = INITIAL_SNAKE_LENGTH;
    p->rect->h = SNAKE_THICKNESS;

    // init collision rect
    p->collisionRect = malloc(sizeof(SDL_Rect));
    p->collisionRect->x = -100;
    p->collisionRect->y = -100;
    p->collisionRect->w = 1;
    p->collisionRect->h = 1;

    // init clone collision rect
    p->clone->collisionRect = malloc(sizeof(SDL_Rect));
    p->clone->collisionRect->x = p->from->x;
    p->clone->collisionRect->y = p->from->y;
    p->clone->collisionRect->w = INITIAL_SNAKE_LENGTH;
    p->clone->collisionRect->h = SNAKE_THICKNESS;

    p->show_clone = false;

    /* @TODO pass direction as a parameter */
    p->direction = RIGHT; // default

    return p;
}

static void draw_parts(Snake *snake, SDL_Renderer *ren) {
    assert(snake);

    for (int i = 0; i < (int) snake->parts->count; ++i) {
        SnakePart *part;
        part = snake->parts->buf[i];

        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);

        int width = part->to->x - part->from->x;
        int height = part->to->y - part->from->y;
        int rect_x = part->from->x;
        int rect_y = part->from->y;
        int rect_clone_x = part->clone->from->x;
        int rect_clone_y = part->clone->from->y;

        Direction direction = part->direction;

        if (height < 0) {
            rect_y = part->to->y;
            rect_clone_y = part->clone->to->y;
            height = abs(height);
        }

        if (width < 0) {
            rect_x = part->to->x;
            rect_clone_x = part->clone->to->x;
            width = abs(width);
        }

        // set main rect size and position
        part->rect->w = width;
        part->rect->h = height;
        part->rect->x = rect_x;
        part->rect->y = rect_y;

        SDL_RenderDrawRect(ren, part->rect);
        SDL_RenderFillRect(ren, part->rect);

        // set clone rect size and position
        if (part->show_clone) {
            SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);

            part->clone->rect->w = width;
            part->clone->rect->h = height;
            part->clone->rect->x = rect_clone_x;
            part->clone->rect->y = rect_clone_y;

            SDL_RenderDrawRect(ren, part->clone->rect);
            SDL_RenderFillRect(ren, part->clone->rect);
        }

        if (snake->is_head(snake, part)) {
            snake->draw_collision_rect(snake, part, ren);
        }
    }
}

static void draw_collision_rect(Snake *snake, SnakePart *part, SDL_Renderer *ren) {
    assert(snake);

    int width = part->to->x - part->from->x;
    int height = part->to->y - part->from->y;

    part->collisionRect->x = part->from->x;
    part->collisionRect->y = part->from->y;
    part->collisionRect->w = width;
    part->collisionRect->h = height;

    if (snake->is_head(snake, part)) {
        if (part->direction == UP) {
            part->collisionRect->h = 1;
            part->collisionRect->y = part->to->y;
        }

        if (part->direction == DOWN) {
            part->collisionRect->h = 1;
            part->collisionRect->y = part->to->y;
        }

        if (part->direction == RIGHT) {
            part->collisionRect->w = 1;
            part->collisionRect->x = part->to->x;
        }

        if (part->direction == LEFT) {
            part->collisionRect->w = 1;
            part->collisionRect->x = part->to->x;
        }
    }

    SDL_RenderDrawRect(ren, part->collisionRect);
    SDL_RenderFillRect(ren, part->collisionRect);
}

static void update_position(Snake *snake, SDL_Renderer *ren) {
    assert(snake);

    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);

    bool is_solid_snake = snake->parts->count == 1;
    bool skip_parts_change = false;
    bool skip_clone_update = false;

    for (int i = 0; i < (int) snake->parts->count; ++i) {
        SnakePart *part = snake->parts->buf[i];

        if (snake->is_snakepart_out_of_bounds(snake, part)) {
            snake->replace_snakepart_with_clone(snake, part);
            skip_clone_update = true;
        }

        if (is_solid_snake) {
            snake->move_head(snake);
            skip_parts_change = true;
        }

        if (!skip_parts_change && snake->is_tail(snake, part)) {
            if (snake->is_tail_shrinked(snake, part)) {
                snkprt_array_pop_front(snake->parts);
                skip_parts_change = true;
            } else {
                snake->shrink_tail(snake, part);
            }
        }

        if (!skip_parts_change && snake->is_head(snake, part)) {
            snake->expand_head(snake, part);
        }

        if (!skip_clone_update) {
            snake->update_clone_position(snake, part);
        }
    }

    snake->draw_parts(snake, ren);
}

static void expand_head(Snake *snake, SnakePart *head) {
    assert(snake);

    if (snake->direction == DOWN) {
        head->to->y += SNAKE_VELOCITY;
    }

    if (snake->direction == RIGHT) {
        head->to->x += SNAKE_VELOCITY;
    }

    if (snake->direction == LEFT) {
        head->to->x -= SNAKE_VELOCITY;
    }

    if (snake->direction == UP) {
        head->to->y -= SNAKE_VELOCITY;
    }
}

static void check_for_collision(Snake *snake, Game *game, Food *food) {
    assert(snake);

    SnakePart *head = snake->get_head(snake);

    // Check for the snake parts collision
    for (int i = 0; i < (int) snake->parts->count - 2; ++i) {
        SnakePart *p = snake->parts->buf[i];

        // @TODO: should check the clone parts as well
        if (SDL_HasIntersection(head->collisionRect, p->rect)) {
            game->run_gameover(game);
            break;
        }

        if (head->show_clone && SDL_HasIntersection(head->clone->rect, p->rect)) {
            game->run_gameover(game);
            break;
        }
    }

    // check for food collision
    SDL_bool has_food_collision = SDL_HasIntersection(head->collisionRect, food->collisionRect);
    SDL_bool has_clone_food_collision =
            head->show_clone && SDL_HasIntersection(head->clone->rect, food->collisionRect);

    if (has_food_collision || has_clone_food_collision) {
        snake->grow_compensation += 30;
        food->is_active = false;
    }
}

static void replace_snakepart_with_clone(Snake *snake, SnakePart *part) {
    assert(snake);
    part->from->x = part->clone->from->x;
    part->to->x = part->clone->to->x;
    part->from->y = part->clone->from->y;
    part->to->y = part->clone->to->y;
    part->show_clone = false;
}

static void move_head(Snake *snake) {
    assert(snake);
    SnakePart *head = snake->get_head(snake);

    bool snake_should_grow = snake->grow_compensation > 0;

    if (snake_should_grow) {
        snake->grow_compensation -= SNAKE_VELOCITY;
    }

    if (snake->direction == UP) {
        head->from->y -= snake_should_grow ? 0 : SNAKE_VELOCITY;
        head->to->y -= SNAKE_VELOCITY;
    }

    if (snake->direction == RIGHT) {
        head->from->x += snake_should_grow ? 0 : SNAKE_VELOCITY;
        head->to->x += SNAKE_VELOCITY;
    }

    if (snake->direction == DOWN) {
        head->from->y += snake_should_grow ? 0 : SNAKE_VELOCITY;
        head->to->y += SNAKE_VELOCITY;
    }

    if (snake->direction == LEFT) {
        head->from->x -= snake_should_grow ? 0 : SNAKE_VELOCITY;
        head->to->x -= SNAKE_VELOCITY;
    }
}

static void shrink_tail(Snake *snake, SnakePart *tail) {
    assert(snake);

    if (snake->grow_compensation > 0) {
        snake->grow_compensation -= SNAKE_VELOCITY;
        return;
    }

    if (tail->direction == RIGHT) {
        tail->from->x += SNAKE_VELOCITY;
    }

    if (tail->direction == LEFT) {
        tail->from->x -= SNAKE_VELOCITY;
    }

    if (tail->direction == UP) {
        tail->from->y -= SNAKE_VELOCITY;
    }

    if (tail->direction == DOWN) {
        tail->from->y += SNAKE_VELOCITY;
    }
}

static bool is_tail_shrinked(Snake *snake, SnakePart *tail) {
    assert(snake);
    SnakePart *head = snake->get_head(snake);

    if ((tail->direction == RIGHT) && (tail->from->x >= tail->to->x - SNAKE_THICKNESS)) {
        return true;
    }

    if ((tail->direction == LEFT) && (tail->from->x <= tail->to->x + SNAKE_THICKNESS)) {
        return true;
    }


    if ((tail->direction == DOWN) && (tail->from->y >= tail->to->y - SNAKE_THICKNESS)) {
        return true;
    }

    if ((tail->direction == UP) && (tail->from->y <= tail->to->y + SNAKE_THICKNESS)) {
        return true;
    }

    return false;
}

static bool is_snakepart_out_of_bounds(Snake *snake, SnakePart *snake_part) {
    assert(snake);
    int from_x = snake_part->from->x;
    int from_y = snake_part->from->y;
    int to_x = snake_part->to->x;
    int to_y = snake_part->to->y;
    Direction direction = snake_part->direction;

    bool out_of_bounds_up = from_y < 0 && to_y < 0;
    bool out_of_bounds_right = from_x > WINDOW_WIDTH && to_x > WINDOW_WIDTH;
    bool out_of_bounds_down = from_y > WINDOW_HEIGHT && to_y > WINDOW_HEIGHT;
    bool out_of_bounds_left = from_x < 0 && to_x < 0;

    return out_of_bounds_up || out_of_bounds_right || out_of_bounds_down || out_of_bounds_left;
}

static int get_snakepart_clone_x(Snake *snake, SnakePart *snake_part) {
    assert(snake);
    int x = snake_part->to->x;

    if (x < (WINDOW_WIDTH / 2)) {
        return x + WINDOW_WIDTH;
    } else {
        return x - WINDOW_WIDTH;
    }
}

static int get_snakepart_clone_y(Snake *snake, SnakePart *snake_part) {
    assert(snake);
    int y = snake_part->to->y;

    if (y < (WINDOW_HEIGHT / 2)) {
        return y + WINDOW_HEIGHT;
    } else {
        return y - WINDOW_HEIGHT;
    }
}

static void update_clone_position(Snake *snake, SnakePart *part) {
    assert(snake);

    if ((part->from->x < 0 || part->to->x < 0)) {
        part->clone->from->x = part->from->x + WINDOW_WIDTH;
        part->clone->to->x = part->to->x + WINDOW_WIDTH;
        part->clone->from->y = part->from->y;
        part->clone->to->y = part->to->y;

        if (!part->show_clone) {
            part->show_clone = true;
        }
    }

    if ((part->from->x > WINDOW_WIDTH || part->to->x > WINDOW_WIDTH)) {
        part->clone->from->x = part->from->x - WINDOW_WIDTH;
        part->clone->to->x = part->to->x - WINDOW_WIDTH;
        part->clone->from->y = part->from->y;
        part->clone->to->y = part->to->y;

        if (!part->show_clone) {
            part->show_clone = true;
        }
    }

    if ((part->from->y < 0 || part->to->y < 0)) {
        part->clone->from->y = part->from->y + WINDOW_HEIGHT;
        part->clone->to->y = part->to->y + WINDOW_HEIGHT;
        part->clone->from->x = part->from->x;
        part->clone->to->x = part->to->x;

        if (!part->show_clone) {
            part->show_clone = true;
        }
    }

    if ((part->from->y > WINDOW_HEIGHT || part->to->y > WINDOW_HEIGHT)) {
        part->clone->from->y = part->from->y - WINDOW_HEIGHT;
        part->clone->to->y = part->to->y - WINDOW_HEIGHT;
        part->clone->from->x = part->from->x;
        part->clone->to->x = part->to->x;

        if (!part->show_clone) {
            part->show_clone = true;
        }
    }
}

static void change_direction(Snake *snake, Direction new_direction) {
    assert(snake);

    if (snake->direction != new_direction && !snake->is_opposite_direction(snake, new_direction)) {
        snake->add_part(snake, new_direction);
        snake->direction = new_direction;
    }
}

static void add_part(Snake *snake, Direction new_direction) {
    assert(snake);

    SnakePart *head = snake->get_head(snake);
    SnakePart *p = snakepart_new();

    if (snake->direction == RIGHT && new_direction == DOWN) {
        p->from->x = head->to->x - SNAKE_THICKNESS;
        p->to->x = head->to->x;
        p->from->y = head->from->y;
        p->to->y = head->to->y;
    }

    if (snake->direction == RIGHT && new_direction == UP) {
        p->from->x = head->to->x - SNAKE_THICKNESS;
        p->to->x = head->to->x;
        p->from->y = head->to->y;
        p->to->y = head->from->y;
    }

    if (snake->direction == UP && new_direction == RIGHT) {
        p->from->x = head->from->x;
        p->to->x = head->to->x;
        p->from->y = head->to->y - 1;
        p->to->y = head->to->y + SNAKE_THICKNESS - 1;
    }

    if (snake->direction == UP && new_direction == LEFT) {
        p->from->x = head->to->x;
        p->to->x = head->from->x;
        p->from->y = head->to->y - 1;
        p->to->y = head->to->y + SNAKE_THICKNESS - 1;
    }

    if (snake->direction == DOWN && new_direction == RIGHT) {
        p->from->x = head->from->x;
        p->to->x = head->to->x;
        p->from->y = head->to->y - SNAKE_THICKNESS;
        p->to->y = head->to->y;
    }

    if (snake->direction == DOWN && new_direction == LEFT) {
        p->from->x = head->to->x;
        p->to->x = head->from->x;
        p->from->y = head->to->y - SNAKE_THICKNESS;
        p->to->y = head->to->y;
    }

    if (snake->direction == LEFT && new_direction == UP) {
        p->from->x = head->to->x - 1;
        p->to->x = head->to->x + SNAKE_THICKNESS - 1;
        p->from->y = head->to->y;
        p->to->y = head->from->y;
    }

    if (snake->direction == LEFT && new_direction == DOWN) {
        p->from->x = head->to->x - 1;
        p->to->x = head->to->x + SNAKE_THICKNESS - 1;
        p->from->y = head->from->y;
        p->to->y = head->to->y;
    }

    snake->update_clone_position(snake, p);

    p->direction = new_direction;

    snkprt_array_push(snake->parts, p);
}

static void change_part_direction(Snake *snake, SnakePart *snake_part, Direction new_direction) {
    assert(snake);
    snake_part->direction = new_direction;
}

static bool is_opposite_direction(Snake *snake, Direction new_direction) {
    assert(snake);
    return abs(snake->direction - new_direction) == 2;
}

static bool is_head(Snake *snake, SnakePart *snake_part) {
    assert(snake);
    return snkprt_array_last(snake->parts) == snake_part;
}

// @TODO: not sure if this is needed
static bool is_head_clone(Snake *snake, SnakePart *snake_part) {
    assert(snake);

    SnakePart *head = snake->get_head(snake);
    return head->clone == snake_part;
}

static bool is_tail(Snake *snake, SnakePart *snake_part) {
    assert(snake);
    return snkprt_array_first(snake->parts) == snake_part;
}

static SnakePart *get_head(Snake *snake) {
    assert(snake);
    return snake->parts->buf[snake->parts->count - 1];
}

Snake *snake_new() {
    Snake *snake;
    snake = (Snake *) malloc(sizeof(Snake));

    snake->direction = RIGHT;
    snake->grow_compensation = 0;

    snake->parts = snkprt_array_new(3);

    SnakePart *p = snakepart_new();

    snkprt_array_push(snake->parts, p);

    snake->draw_parts = &draw_parts;
    snake->draw_collision_rect = &draw_collision_rect;
    snake->update_position = &update_position;
    snake->update_clone_position = &update_clone_position;
    snake->change_direction = &change_direction;
    snake->replace_snakepart_with_clone = &replace_snakepart_with_clone;
    snake->is_opposite_direction = &is_opposite_direction;
    snake->change_part_direction = &change_part_direction;
    snake->is_head = &is_head;
    snake->is_head_clone = &is_head_clone;
    snake->is_tail = &is_tail;
    snake->get_head = &get_head;
    snake->expand_head = &expand_head;
    snake->check_for_collision = &check_for_collision;
    snake->move_head = &move_head;
    snake->shrink_tail = &shrink_tail;
    snake->is_tail_shrinked = &is_tail_shrinked;
    snake->is_snakepart_out_of_bounds = &is_snakepart_out_of_bounds;
    snake->get_snakepart_clone_x = &get_snakepart_clone_x;
    snake->get_snakepart_clone_y = &get_snakepart_clone_y;
    snake->add_part = &add_part;

    return snake;
}


