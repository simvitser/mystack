#include <stdio.h>
#include <stdbool.h>

#include "stack.h"

int main() {
    Stack_t st = {};
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

    // st.size = st.capacity + 2;
    // stackPush(&st, 1);

    for (int i = 0; i < 6; i++) {
        printf(STACK_ELEMENT_FORMATER " ", stackPopLoyal(&st));
    }
    printf("\n");

    // stackPop(&st);

    printf("-----------\n\n");
    stackPrint(&st);

    stackDetor(&st);
    return 0;
}
