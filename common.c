#include <math.h>
#include <assert.h>
#include <stdio.h>

#include "common.h"

void clearInputBuffer() {
    int ch = '\0';
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
}

bool isZero(double n) { return fabs(n) < EPS; }

bool isEqual(double a, double b) { return isZero(a - b); }

double randDouble() { return (double)rand() / (rand() + 1); }

uint64_t getHash(const void *buffer, const size_t size) {
    assert(buffer);

    const uint8_t *buf = (const uint8_t*)buffer;
    uint64_t ans = 5381;
    for (size_t i = 0; i < size; i++) {
        ans ^= (uint64_t)buf[i];
        ans *= 33;
    }
    return ans;
}
