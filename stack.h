#ifndef _STACK_H
#define _STACK_H

#define STACK_DEBUG
#define STACK_HASH_PROTECTION
#define STACK_CANARY_PROTECTION

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

#ifdef STACK_CANARY_PROTECTION
    #define IF_STACK_CANARY(...) __VA_ARGS__
#else
    #define IF_STACK_CANARY(...)
#endif

#ifdef STACK_HASH_PROTECTION
    #define IF_STACK_HASH(...) __VA_ARGS__
#else
    #define IF_STACK_HASH(...)
#endif

#define stackPrint(st)           _stackPrint(st, #st)
#define stackfPrint(stream, st) _stackfPrint(st, #st)

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

typedef double stackElement_t;
#define STACK_ELEMENT_FORMATER "%lf"

typedef uint64_t canary_t;

typedef struct {
    IF_STACK_CANARY(canary_t canary1;)
    stackElement_t *data;
    IF_STACK_CANARY(stackElement_t *buffer;)
    size_t size;
    size_t capacity;
    IF_STACK_HASH(uint64_t hash;)
    IF_STACK_DEBUG(
        const char *file;
        const char *func;
        const char *name;
        int line;
    )
    IF_STACK_CANARY(canary_t canary2;)
} stack_t;

typedef enum {
    STACK_OK = 0,
    STACK_MEMORY_ERROR,
    STACK_ZERO_SIZE_POP_ERROR,
    STACK_RESIZE_ERROR
} ErrorStatusStack;

typedef enum {
    VERIFY_STACK_OK = 0,
    VERIFY_STACK_NULLPTR,
    VERIFY_STACK_SIZE_MORE_CAPACITY,
    VERIFY_STACK_DATA_NULLPTR,
    VERIFY_STACK_CANARY1,
    VERIFY_STACK_CANARY2,
    VERIFY_STACK_CANARY3,
    VERIFY_STACK_CANARY4,
    VERIFY_STACK_HASH
} ErrorStatusStackVerifier;

ErrorStatusStackVerifier isStackFailed(stack_t *st);

ErrorStatusStack _stackCtor(stack_t *st IF_STACK_DEBUG(, const char *file, const char *func, int line, const char *name));

ErrorStatusStack _stackCtorN(stack_t *st IF_STACK_DEBUG(, const char *file, const char *func, int line, const char *name), size_t n, ...);

ErrorStatusStack stackPush(stack_t *st, stackElement_t el);

stackElement_t      stackPop(stack_t *st);
stackElement_t stackPopLoyal(stack_t *st);

ErrorStatusStack stackPopTo(stack_t *st, stackElement_t *el);

void stackDtor(stack_t *st);

size_t getStackSize(stack_t *st);

ErrorStatusStack stackResize(stack_t *st, size_t new_size);

void _stackPrint(stack_t *st, const char *name);

void _stackfPrint(FILE *stream, stack_t *st, const char *name);

#endif
