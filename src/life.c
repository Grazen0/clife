#include "life.h"
#include <stdlib.h>

void life_destroy(Life *life)
{
    if (life != nullptr) {
        life_deinit(life);
        free(life);
    }
}
