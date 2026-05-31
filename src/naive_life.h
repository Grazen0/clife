#ifndef CLIFE_NAIVE_LIFE_H
#define CLIFE_NAIVE_LIFE_H

#include "coord_set.h"
#include "life.h"
#include "util.h"

typedef struct {
    Life base;
    CoordSet active;
} NaiveLife;

NaiveLife naive_life_init();

NaiveLife *naive_life_create();

void naive_life_deinit(NaiveLife *life);

void naive_life_reset(NaiveLife *life);

bool naive_life_get(const NaiveLife *life, i64 i, i64 j);

void naive_life_set(NaiveLife *life, i64 i, i64 j, bool val);

void naive_life_step(NaiveLife *life);

#endif
