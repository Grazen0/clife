#include "naive_life.h"
#include "coord_set.h"
#include "util.h"
#include <stddef.h>
#include <stdlib.h>

static void naive_life_vtbl_deinit(Life *life)
{
    naive_life_deinit((NaiveLife *)life);
}

static void naive_life_vtbl_reset(Life *life)
{
    naive_life_reset((NaiveLife *)life);
}

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

static size_t count_neighbors(const NaiveLife *life, i64 i, i64 j)
{
    size_t count = 0;

    for (i64 si = -1; si <= 1; ++si) {
        for (i64 sj = -1; sj <= 1; ++sj) {
            if (si != 0 || sj != 0) {
                Coord c = {i + si, j + sj};
                if (coord_set_contains(&life->active, c))
                    ++count;
            }
        }
    }

    return count;
}

NaiveLife naive_life_init()
{
    static const LifeVTable vtable = {
        .reset = naive_life_vtbl_reset,
        .get = naive_life_vtbl_get,
        .set = naive_life_vtbl_set,
        .step = naive_life_vtbl_step,
        .deinit = naive_life_vtbl_deinit,
    };

    return (NaiveLife){
        .base = {.vtable = &vtable},
        .active = coord_set_init(),
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
    coord_set_deinit(&life->active);
}

void naive_life_reset(NaiveLife *life)
{
    coord_set_clear(&life->active);
}

bool naive_life_get(const NaiveLife *life, i64 i, i64 j)
{
    return coord_set_contains(&life->active, (Coord){i, j});
}

void naive_life_set(NaiveLife *life, i64 i, i64 j, bool val)
{
    if (val)
        coord_set_insert(&life->active, (Coord){i, j});
    else
        coord_set_remove(&life->active, (Coord){i, j});
}

void naive_life_step(NaiveLife *life)
{
    CoordSet affected = coord_set_init();

    CoordSetIter iter = coord_set_iter_init(&life->active);

    Coord *coord = {};
    while ((coord = coord_set_iter_next(&iter)) != nullptr) {
        for (i64 si = -1; si <= 1; ++si) {
            for (i64 sj = -1; sj <= 1; ++sj) {
                coord_set_insert(&affected,
                                 (Coord){coord->i + si, coord->j + sj});
            }
        }
    }

    CoordSet new_active = coord_set_init();
    CoordSetIter affected_iter = coord_set_iter_init(&affected);

    while ((coord = coord_set_iter_next(&affected_iter)) != nullptr) {
        bool active = coord_set_contains(&life->active, *coord);
        size_t n = count_neighbors(life, coord->i, coord->j);

        bool active_next = active ? (n >= 2 && n <= 3) : n == 3;
        if (active_next)
            coord_set_insert(&new_active, *coord);
    }

    coord_set_deinit(&life->active);
    life->active = new_active;

    coord_set_deinit(&affected);
}
