#ifndef CLIFE_LIFE_H
#define CLIFE_LIFE_H

#include "src/util.h"

typedef struct Life Life;

typedef struct {
    void (*deinit)(Life *life);
    void (*reset)(Life *life);
    bool (*get)(const Life *life, i64 i, i64 j);
    void (*set)(Life *life, i64 i, i64 j, bool val);
    void (*step)(Life *life);
} LifeVTable;

struct Life {
    const LifeVTable *vtable;
};

static inline void life_deinit(Life *life)
{
    life->vtable->deinit(life);
}

static inline void life_reset(Life *life)
{
    life->vtable->reset(life);
}

static inline bool life_get(const Life *life, i64 i, i64 j)
{
    return life->vtable->get(life, i, j);
}

static inline void life_set(Life *life, i64 i, i64 j, bool val)
{
    life->vtable->set(life, i, j, val);
}

static inline void life_step(Life *life)
{
    life->vtable->step(life);
}

void life_destroy(Life *life);

#endif
