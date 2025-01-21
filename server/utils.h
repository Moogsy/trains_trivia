#pragma once

#include <stdlib.h>

int random_bound(int upper) {
    return rand() % upper;
}