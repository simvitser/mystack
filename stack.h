#ifndef _STACK_H
#define _STACK_H

#define STACK_DEBUG

#ifdef STACK_DEBUG
#define stackCtor(st)           _stackCtor(st, __FILE__, __PRETTY_FUNCTION__, __LINE__, #st)
#define stackCtorN(st, n, ...) _stackCtorN(st, __FILE__, __PRETTY_FUNCTION__, __LINE__, #st, n, __VA_ARGS__)
#else
#define stackCtor(st) _stackCtor(st)
#define stackCtorN(st, n, ...) _stackCtorN(st, n, __VA_ARGS__)
#endif

#ifdef STACK_DEBUG
    #define IF_STACK_DEBUG(...) __VA_ARGS__
#else
    #define IF_STACK_DEBUG(...)
#endif

#define MIN(a, b) ((a) > (b) ? (b) : (a))


#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

typedef double StackElement_t;
#define STACK_ELEMENT_FORMATER "%lf"

typedef struct {
    StackElement_t *data;
    size_t size;
    size_t capacity;
    IF_STACK_DEBUG(
        const char *file;
        const char *func;
        const char *name;
        int line;
    )
} Stack_t;

typedef enum {
    STACK_OK = 0,
    STACK_MEMORY_ERROR,
    STACK_ZERO_SIZE_POP_ERROR,
    STACK_RESIZE_ERROR
} ErrorStatusStack;

bool stackVerifierNoPrint(Stack_t *st);

ErrorStatusStack _stackCtor(Stack_t *st IF_STACK_DEBUG(, const char *file, const char *func, int line, const char *name));

ErrorStatusStack _stackCtorN(Stack_t *st IF_STACK_DEBUG(, const char *file, const char *func, int line, const char *name), size_t n, ...);

ErrorStatusStack stackPush(Stack_t *st, StackElement_t el);

StackElement_t      stackPop(Stack_t *st);
StackElement_t stackPopLoyal(Stack_t *st);

ErrorStatusStack stackPopTo(Stack_t *st, StackElement_t *el);

void stackDetor(Stack_t *st);

size_t getStackSize(Stack_t *st);

ErrorStatusStack stackResize(Stack_t *st, size_t new_size);

void stackPrint(Stack_t *st);

void stackfPrint(FILE *stream, Stack_t *st);

#endif
