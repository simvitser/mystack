#include <stdio.h>
#include <stdcountof.h>

int main() {
    int     s[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    double s2[10] = {};

    printf("s: %zu, s1: %zu\n", countof(s), countof(s2));
    // printf("s: %zu, s2: %zu\n", _Countof(s), _Countof(s2));
}
