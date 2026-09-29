#define BLACK     "\033[30m"
#define RED       "\033[31m"
#define GREEN     "\033[32m"
#define YELLOW    "\033[33m"
#define BLUE      "\033[34m"
#define PURPLE    "\033[35m"
#define LIGHTBLUE "\033[36m"
#define WHITE     "\033[37m"
#define STANDART  "\033[0m"

#include <assert.h>
#include <stdarg.h>

#include "stack.h"

#ifndef STACK_ELEMENT_FORMATER
#define STACK_ELEMENT_FORMATER "%lld"
#endif

const size_t BASE_STACK_SIZE = 2;

static void         stackDump(Stack_t *st);
static bool     stackVerifier(Stack_t *st);
static bool isNeedStackResize(Stack_t *st);

ErrorStatusStack _stackCtor(Stack_t *st IF_STACK_DEBUG(, const char *file, const char *func, int line, const char *name)) {
    assert(st);
    assert(st->data == NULL);
    assert(st->capacity == 0);
    assert(st->size == 0);

    st->data = (StackElement_t*)calloc(BASE_STACK_SIZE, sizeof(StackElement_t));
    
    if (st->data == NULL) {
        return STACK_MEMORY_ERROR;
    }
    
    st->capacity = BASE_STACK_SIZE;

    IF_STACK_DEBUG(
        st->file = file;
        st->func = func;
        st->line = line;
        st->name = name;
    )

    assert(stackVerifier(st));
    
    return STACK_OK;
}

ErrorStatusStack _stackCtorN(Stack_t *st IF_STACK_DEBUG(, const char *file, const char *func, int line, const char *name), size_t n, ...) {
    assert(st);
    assert(st->data == NULL);
    assert(st->capacity == 0);
    assert(st->size == 0);

    va_list ap;
    va_start(ap, n);

    st->data = (StackElement_t*)calloc(n, sizeof(StackElement_t));
    
    if (st->data == NULL) {
        return STACK_MEMORY_ERROR;
    }
    
    st->capacity = n;
    st->size = n;

    for (size_t i = 0; i < n; i++) {
        st->data[i] = va_arg(ap, StackElement_t);
    }

    va_end(ap);

    IF_STACK_DEBUG(
        st->file = file;
        st->func = func;
        st->line = line;
        st->name = name;
    )

    assert(stackVerifier(st));
    
    return STACK_OK;
}

ErrorStatusStack stackPush(Stack_t *st, StackElement_t el) {
    assert(stackVerifier(st));

    if (st->size == st->capacity) {
        StackElement_t *ptr = (StackElement_t*)realloc(st->data, st->capacity * 2 * sizeof(StackElement_t));
        if (ptr == NULL) {
            return STACK_MEMORY_ERROR;
        }
        st->capacity *= 2;
        st->data = ptr;
    }

    st->data[st->size++] = el;

    assert(stackVerifier(st));

    return STACK_OK;
}

static bool isNeedStackResize(Stack_t *st) {
    assert(stackVerifier(st));
    return st->size > 0 && st->capacity / st->size >= 4 && st->capacity / 2 >= BASE_STACK_SIZE;
}


StackElement_t stackPop(Stack_t *st) {
    assert(stackVerifier(st));

    if (st->size == 0) {
        fprintf(stderr, RED "try pop from zero-size stack\n" STANDART);
        stackDump(st);
        abort();
    }

    StackElement_t return_value = st->data[--st->size]; 

    if (isNeedStackResize(st)) stackResize(st, st->capacity / 2);

    assert(stackVerifier(st));

    return return_value;
}

StackElement_t stackPopLoyal(Stack_t *st) {
    assert(stackVerifier(st));

    if (st->size == 0) {
        return (StackElement_t){0};
    }

    StackElement_t return_value = st->data[--st->size]; 

        if (isNeedStackResize(st)) stackResize(st, st->capacity / 2);

    assert(stackVerifier(st));

    return return_value;
}

ErrorStatusStack stackPopTo(Stack_t *st, StackElement_t *el) {
    assert(stackVerifier(st));
    assert(el);

    if (st->size == 0) {
        return STACK_ZERO_SIZE_POP_ERROR;
    }

    *el = st->data[--st->size];
    
    if (isNeedStackResize(st)) stackResize(st, st->capacity / 2);

    assert(stackVerifier(st));
    
    return STACK_OK;
}

void stackDetor(Stack_t *st) {
    assert(stackVerifier(st));

    free(st->data);
    st->size = 0;
    st->capacity = 0;
}

bool stackVerifierNoPrint(Stack_t *st) {
    if (st == NULL)                     return 0;
    if (st->size > st->capacity)        return 0;
    if (st->capacity < BASE_STACK_SIZE) return 0;
    if (st->data == NULL)               return 0;
    return 1;
}

static bool stackVerifier(Stack_t *st) {
    bool verified = stackVerifierNoPrint(st);
    if (!verified) stackDump(st);
    return verified;
}

#ifdef STACK_DEBUG
static void stackDump(Stack_t *st) {
    if (st == NULL) return;

    fprintf(stderr, "Dump stack made in file %s by function %s in line %d:\n", st->file, st->func, st->line);
    fprintf(stderr, "Stack_t %s [%p] {\n", st->name + 1, st); // + 1 - убрать & в начале
    fprintf(stderr, "    size = %zu\n", st->size);
    fprintf(stderr, "    capacity = %zu\n", st->capacity);

    if (st->data == NULL) return;

    fprintf(stderr, "    data[] = {\n");
    for (size_t i = 0; i < MIN(st->size, st->capacity); i++) {
        fprintf(stderr, "         *[%zu] = " STACK_ELEMENT_FORMATER "\n", i, st->data[i]);
    }
    for (size_t i = st->size; i < st->capacity; i++) {
        fprintf(stderr, "          [%zu] = " STACK_ELEMENT_FORMATER "\n", i, st->data[i]);
    }
    fprintf(stderr, "    }\n}\n");
}
#else
static void stackDump(Stack_t *st) {}
#endif

size_t getStackSize(Stack_t *st) {
    assert(stackVerifier(st));

    return st->size;
}

ErrorStatusStack stackResize(Stack_t *st, size_t new_size) {
    assert(stackVerifier(st));

    if (new_size < st->size) {
        IF_STACK_DEBUG(fprintf(stderr, RED "ERROR, new_size < stack size\n"));
        stackDump(st);
        return STACK_RESIZE_ERROR;
    }

    StackElement_t *ptr = (StackElement_t*)realloc(st->data, new_size * sizeof(StackElement_t));
    if (ptr == NULL) {
        return STACK_MEMORY_ERROR;
    }
    
    st->capacity = new_size;
    st->data = ptr;

    assert(stackVerifier(st));

    return STACK_OK;
}

void _stackfPrint(FILE *stream, Stack_t *st, const char *name) {
    assert(stackVerifier(st));

    fprintf(stream, "Stack_t %s [%p] {\n", name + 1, st);
    fprintf(stream, "    size = %zu\n", st->size);
    fprintf(stream, "    capacity = %zu\n", st->capacity);
    fprintf(stream, "    data[] = {\n");
    for (size_t i = 0; i < MIN(st->size, st->capacity); i++) {
        fprintf(stream, "         *[%zu] = " STACK_ELEMENT_FORMATER "\n", i, st->data[i]);
    }
    for (size_t i = st->size; i < st->capacity; i++) {
        fprintf(stream, "          [%zu] = " STACK_ELEMENT_FORMATER "\n", i, st->data[i]);
    }
    fprintf(stream, "    }\n}\n");
}

void _stackPrint(Stack_t *st, const char *name) {
    assert(stackVerifier(st));
    _stackfPrint(stdout, st, name);
}
