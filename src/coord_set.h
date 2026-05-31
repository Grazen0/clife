#ifndef CLIFE_COORD_SET_H
#define CLIFE_COORD_SET_H

#include "util.h"
#include <stddef.h>

typedef struct CoordSetEntry CoordSetNode;

typedef struct {
    CoordSetNode **buckets;
    size_t count;
    size_t buckets_size;
    size_t used_buckets;
} CoordSet;

typedef struct {
    CoordSet *set;
    size_t next_bucket;
    CoordSetNode *cur_node;
} CoordSetIter;

CoordSet coord_set_init();

void coord_set_deinit(CoordSet *set);

bool coord_set_contains(const CoordSet *set, Coord coord);

bool coord_set_insert(CoordSet *set, Coord coord);

bool coord_set_remove(CoordSet *set, Coord coord);

void coord_set_clear(CoordSet *set);

CoordSetIter coord_set_iter_init(CoordSet *set);

Coord *coord_set_iter_next(CoordSetIter *iter);

#endif
