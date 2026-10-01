#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "stack.h"
#include "log.h"

// void sort2(size_t *s) {
//     assert(s);
//
//     if (s[0] < s[1]) {
//         size_t tmp = s[0];
//         s[0] = s[1];
//         s[1] = tmp;
//     }
// }

void rewrite_string_reverse(char *s_end) {
    int max_len = 7;
    while (*s_end != '\n' && max_len) {
        *s_end = 'w';
        s_end--;
        max_len--;
    }
}

int main() {
    logInit("log.txt");
    
    log("Start stack program\n");

    stack_t st = {};
    ErrorStatusStack err = stackCtorN(&st, 3, 11.0, 12.0, 13.0);
    
    if (err == STACK_MEMORY_ERROR) {
        fprintf(stderr, "MEEEEEEEEEMORY ERR ctor\n");
        return 1;
    }

    for (int i = 0; i < 6; i++) {
        stackPush(&st, i + 1);
    }
    stackPrint(&st);
    printf("-----------\n\n");

    for (int i = 0; i < 6; i++) {
        printf(STACK_ELEMENT_FORMATER " ", stackPop(&st));
    }
    printf("\n");

    // for (int i = 0; i < 3; i++) {
    //     ((uint64_t*)st.data)[i - 1] = 999;
    // }
   
    logPuts("portim");
    rewrite_string_reverse((char*)st.data);

    // st.size = st.capacity + 2;
    // stackPrint(&st);
    // sort2(&(st.size));
    // stackPrint(&st);
    // st.canary1 = 1;
    // stackPush(&st, 1);
    stackPop(&st);
    logPuts("proshlo");

    for (int i = 0; i < 6; i++) {
        printf(STACK_ELEMENT_FORMATER " ", stackPopLoyal(&st));
    }
    printf("\n");

    // stackPop(&st);

    printf("-----------\n\n");
    stackPrint(&st);

    stackDetor(&st);
    logDestroy();
    return 0;
}
