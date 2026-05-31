#include "coord_set.h"
#include "util.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

static constexpr float MAX_FILL_FACTOR = 0.75F;
static constexpr size_t INIT_BUCKETS = 8;

struct CoordSetEntry {
    CoordSetNode *next;
    size_t hash;
    Coord coord;
};

typedef CoordSetNode Node;

static inline size_t hash_coord(Coord coord)
{
    return ((uintmax_t)coord.i << (4 * sizeof(uintmax_t))) ^ coord.j;
}

static inline float fill_factor(CoordSet *set)
{
    return (float)set->used_buckets / (float)set->buckets_size;
}

static inline void rehash(CoordSet *set)
{
    size_t new_buckets_size =
        set->buckets_size == 0 ? INIT_BUCKETS : 2 * set->buckets_size;

    Node **new_buckets = calloc(new_buckets_size, sizeof(*new_buckets));
    assert(new_buckets != nullptr);

    size_t new_used_buckets = 0;

    for (size_t i = 0; i < set->buckets_size; ++i) {
        Node *cur = set->buckets[i];

        while (cur != nullptr) {
            size_t new_idx = cur->hash % new_buckets_size;

            if (new_buckets[new_idx] == nullptr)
                ++new_used_buckets;

            Node *next = cur->next;

            cur->next = new_buckets[new_idx];
            new_buckets[new_idx] = cur;

            cur = next;
        }
    }

    free((void *)set->buckets);
    set->buckets = new_buckets;
    set->buckets_size = new_buckets_size;
    set->used_buckets = new_used_buckets;
}

CoordSet coord_set_init()
{
    return (CoordSet){
        .buckets = nullptr,
        .buckets_size = 0,
        .used_buckets = 0,
    };
}

void coord_set_deinit(CoordSet *set)
{
    for (size_t i = 0; i < set->buckets_size; ++i) {
        Node *cur = set->buckets[i];

        while (cur != nullptr) {
            Node *next = cur->next;
            free(cur);
            cur = next;
        }
    }

    free(set->buckets);
    *set = coord_set_init();
}

bool coord_set_contains(const CoordSet *set, Coord coord)
{
    if (set->buckets_size == 0)
        return false;

    size_t hash = hash_coord(coord);
    size_t idx = hash % set->buckets_size;

    Node *cur = set->buckets[idx];

    while (cur != nullptr) {
        if (coord_eq(cur->coord, coord))
            return true;

        cur = cur->next;
    }

    return false;
}

bool coord_set_insert(CoordSet *set, Coord coord)
{
    if (set->buckets_size == 0 || fill_factor(set) > MAX_FILL_FACTOR)
        rehash(set);

    assert(set->buckets_size > 0);

    size_t hash = hash_coord(coord);
    size_t idx = hash % set->buckets_size;

    Node **cur = &set->buckets[idx];

    while (*cur != nullptr && !coord_eq((*cur)->coord, coord))
        cur = &(*cur)->next;

    if (*cur != nullptr)
        return false;

    if (set->buckets[idx] == nullptr)
        ++set->used_buckets;

    *cur = calloc(1, sizeof(**cur));
    **cur = (Node){
        .next = nullptr,
        .hash = hash,
        .coord = coord,
    };

    ++set->count;
    return true;
}

bool coord_set_remove(CoordSet *set, Coord coord)
{
    if (set->buckets_size == 0)
        return false;

    size_t hash = hash_coord(coord);
    size_t idx = hash % set->buckets_size;

    Node **cur = &set->buckets[idx];

    while (*cur != nullptr && !coord_eq((*cur)->coord, coord))
        cur = &(*cur)->next;

    if (*cur == nullptr)
        return false;

    Node *next = (*cur)->next;
    free(*cur);
    *cur = next;

    if (set->buckets[idx] == nullptr)
        --set->used_buckets;

    --set->count;
    return true;
}

void coord_set_clear(CoordSet *set)
{
    coord_set_deinit(set);
}

CoordSetIter coord_set_iter_init(CoordSet *set)
{
    return (CoordSetIter){
        .set = set,
        .next_bucket = 0,
        .cur_node = nullptr,
    };
}

Coord *coord_set_iter_next(CoordSetIter *iter)
{
    while (iter->cur_node == nullptr) {
        if (iter->next_bucket >= iter->set->buckets_size)
            return nullptr;

        iter->cur_node = iter->set->buckets[iter->next_bucket];
        ++iter->next_bucket;
    }

    Coord *coord = &iter->cur_node->coord;
    iter->cur_node = iter->cur_node->next;
    return coord;
}
