#include "util.h"

bool coord_eq(Coord lhs, Coord rhs)
{
    return lhs.i == rhs.i && lhs.j == rhs.j;
}
