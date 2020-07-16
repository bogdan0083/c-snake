//
// Created by dbogd on 03.07.2020.
//

#ifndef C_GAMES_ARRAY_H
#define C_GAMES_ARRAY_H

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#define ARRAY_DEFAULT_CAPACITY 4

#define GENERATE_ARRAY_HEADER_FOR_TYPE(T, PREFIX) \
    struct PREFIX##_array_s { \
        T **buf; \
        size_t capacity; \
        size_t count; \
    }; \
    \
    struct PREFIX##_array_s *PREFIX##_array_new(size_t capacity); \
    bool PREFIX##_array_empty(struct PREFIX##_array_s *a); \
    bool PREFIX##_array_full(struct PREFIX##_array_s *a); \
    void PREFIX##_array_push(struct PREFIX##_array_s *a, T *item); \
    void PREFIX##_array_pop_front(struct PREFIX##_array_s *a); \
    T *PREFIX##_array_first(struct PREFIX##_array_s *a); \
    T *PREFIX##_array_last(struct PREFIX##_array_s *a);

#define GENERATE_ARRAY_SOURCE_FOR_TYPE(T, PREFIX) \
    struct PREFIX##_array_s *PREFIX##_array_new(size_t capacity) { \
        struct PREFIX##_array_s *a; \
        a = (struct PREFIX##_array_s *) malloc(sizeof(struct PREFIX##_array_s)); \
        a->buf = calloc(capacity, sizeof(*a->buf)); \
        a->capacity = capacity; \
        a->count = 0; \
        return a; \
    }; \
    bool PREFIX##_array_empty(struct PREFIX##_array_s *a) { \
        assert(a); \
        return (a->count == 0); \
    }; \
    \
    bool PREFIX##_array_full(struct PREFIX##_array_s *a) { \
        assert(a); \
        return (a->count >= a->capacity); \
    }; \
    \
    T *PREFIX##_array_first(struct PREFIX##_array_s *a) { \
        assert(a); \
        if (a->count > 0) { \
            return a->buf[0]; \
        } \
    }; \
    \
    T *PREFIX##_array_last(struct PREFIX##_array_s *a) { \
        assert(a); \
        if (a->count > 0) { \
            return a->buf[a->count - 1]; \
        } \
    }; \
    \
    /* @TODO: return bool instead of void */ \
    void PREFIX##_array_pop_front(struct PREFIX##_array_s *a) { \
        assert(a); \
        memmove(a->buf, a->buf + 1, \
                (a->count - 1) * sizeof(*a->buf)); \
\
        a->count--; \
    }; \
    /* @TODO: return bool instead of void */ \
    void PREFIX##_array_push(struct PREFIX##_array_s *a, T *item) { \
        assert(a); \
        if (PREFIX##_array_full(a)) { \
            /* @TODO: move buffer resizing to a separate function */ \
            if ( (a->buf = realloc(a->buf, (a->capacity * 2) * sizeof(*a->buf)) ) == NULL) { \
                free(a->buf); \
                free(a); \
                printf("array allocation failed\n"); \
                exit(1); \
            }; \
            \
            a->capacity *= 2; \
        } \
\
        a->buf[a->count++] = item; \
    };

#define GENERATE_ARRAY_FOR_TYPE(T, PREFIX) \
    GENERATE_ARRAY_HEADER_FOR_TYPE(T, PREFIX) \
    GENERATE_ARRAY_SOURCE_FOR_TYPE(T, PREFIX)

#endif //C_GAMES_ARRAY_H
