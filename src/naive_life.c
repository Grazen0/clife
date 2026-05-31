#include "naive_life.h"
#include "util.h"
#include <stdlib.h>

static bool naive_life_vtbl_get(const Life *life, i64 i, i64 j)
{
    return naive_life_get((const NaiveLife *)life, i, j);
}

static void naive_life_vtbl_set(Life *life, i64 i, i64 j, bool val)
{
    naive_life_set((NaiveLife *)life, i, j, val);
}

static void naive_life_vtbl_step(Life *life)
{
    naive_life_step((NaiveLife *)life);
}

static void naive_life_vtbl_deinit(Life *life)
{
    naive_life_deinit((NaiveLife *)life);
}

NaiveLife naive_life_init()
{
    static const LifeVTable vtable = {
        .get = naive_life_vtbl_get,
        .set = naive_life_vtbl_set,
        .step = naive_life_vtbl_step,
        .deinit = naive_life_vtbl_deinit,
    };

    return (NaiveLife){
        .base = {.vtable = &vtable},
        .cur_i = 0,
        .cur_j = 0,
    };
}

NaiveLife *naive_life_create()
{
    NaiveLife *life = calloc(1, sizeof(*life));
    if (life != nullptr)
        *life = naive_life_init();

    return life;
}

void naive_life_deinit(NaiveLife *life)
{
}

bool naive_life_get(const NaiveLife *life, i64 i, i64 j)
{
    // TODO, this is just a random stub i came up with
    return i == life->cur_i && j == life->cur_j;
}

void naive_life_set(NaiveLife *life, i64 i, i64 j, bool val)
{
    life->cur_i = i;
    life->cur_j = j;
}

void naive_life_step(NaiveLife *life)
{
    life->cur_i++;
    life->cur_j++;
}
