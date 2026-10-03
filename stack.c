#include <assert.h>
#include <stdarg.h>
#include <stdint.h>

#include "stack.h"
#include "common.h"
#include "log.h"

#ifndef STACK_ELEMENT_FORMATER
#define STACK_ELEMENT_FORMATER "%lld"
#endif

#define isNeedStackResize(st) (st->size > 0 && st->capacity / st->size >= 4 && st->capacity / 2 >= BASE_STACK_SIZE)

static const size_t BASE_STACK_SIZE = 2;

static const canary_t CANARY1 = 0xDED32DEDDED32DED;
static const canary_t CANARY2 = 0xBA0BAB00EDA0DEDA;
static const canary_t CANARY3 = 0xFACEDEDAFACEDEDA;
static const canary_t CANARY4 = 0xC0CEDDED333C0CA1;

static bool stackVerifier(stack_t *st);

static void stackDump(stack_t *st);

static void recountHash(stack_t *st);

ErrorStatusStack _stackCtor(stack_t *st IF_STACK_DEBUG(, const char *file, const char *func, int line, const char *name)) {
    assert(st);
    assert(st->data == NULL);
    assert(st->capacity == 0);
    assert(st->size == 0);
    IF_STACK_CANARY(
        assert(st->canary1 == 0);
        assert(st->canary2 == 0);
    )
    IF_STACK_HASH(
        assert(st->hash == 0);
    )
 
    log("Init a stack %p\n", st);

    if (st->data == NULL) {
        logError("Can't create stack %p: no memory\n", st);
        return STACK_MEMORY_ERROR;
    }

    #ifdef STACK_CANARY_PROTECTION
    size_t data_size = BASE_STACK_SIZE * sizeof(stackElement_t);
    if (data_size % sizeof(canary_t)) data_size += (sizeof(canary_t) - data_size % sizeof(canary_t));

    canary_t* ptr = (canary_t*)calloc(data_size + 2 * sizeof(canary_t), 1);
    if (ptr == NULL) {
        logError("Can't create stack %p: no memory\n", st);
        return STACK_MEMORY_ERROR;
    }
    
    ptr[0] = CANARY3;
    ptr[data_size / sizeof(canary_t) + 1] = CANARY4;

    st->data =   (stackElement_t*)(ptr + 1);
    st->buffer = (stackElement_t*)(ptr);
    
    st->capacity = BASE_STACK_SIZE;
    st->canary1 = CANARY1;
    st->canary2 = CANARY2;
    #else
    st->data = (stackElement_t*)calloc(BASE_STACK_SIZE, sizeof(stackElement_t));
    
    if (st->data == NULL) {
        logError("Can't create stack %p: no memory\n", st);
        return STACK_MEMORY_ERROR;
    }
    
    st->capacity = BASE_STACK_SIZE;
    #endif

    IF_STACK_DEBUG(
        st->file = file;
        st->func = func;
        st->line = line;
        st->name = name;
    )

    IF_STACK_HASH(st->hash = getHash(st, sizeof(stack_t));)

    assert(stackVerifier(st));

    logOk("succes init stack %p\n");
    
    return STACK_OK;
}

ErrorStatusStack _stackCtorN(stack_t *st IF_STACK_DEBUG(, const char *file, const char *func, int line, const char *name), size_t n, ...) {
    assert(st);
    assert(st->data == NULL);
    assert(st->capacity == 0);
    assert(st->size == 0);
    IF_STACK_CANARY(
        assert(st->canary1 == 0);
        assert(st->canary2 == 0);
    )
    IF_STACK_HASH(
        assert(st->hash == 0);
    )

    log("Init a stack at %p\n", st);

    va_list ap = {};
    va_start(ap, n);

    #ifdef STACK_CANARY_PROTECTION
    size_t data_size = n * sizeof(stackElement_t);
    if (data_size % sizeof(canary_t)) data_size += (sizeof(canary_t) - data_size % sizeof(canary_t));

    canary_t* ptr = (canary_t*)calloc(data_size + 2 * sizeof(canary_t), 1);
    if (ptr == NULL) {
        logError("Can't create stack %p: no memory\n", st);
        return STACK_MEMORY_ERROR;
    }

    ptr[0] = CANARY3;
    ptr[data_size / sizeof(canary_t) + 1] = CANARY4;

    st->data =   (stackElement_t*)(ptr + 1);
    st->buffer = (stackElement_t*)(ptr);
    
    st->capacity = n;
    st->size = n;
    st->canary1 = CANARY1;
    st->canary2 = CANARY2;
    #else
    st->data = (stackElement_t*)calloc(n, sizeof(stackElement_t));
    
    if (st->data == NULL) {
        return STACK_MEMORY_ERROR;
    }
    
    st->capacity = n;
    st->size = n;
    #endif

    for (size_t i = 0; i < n; i++) {
        if (sizeof(stackElement_t) != 1) st->data[i] = va_arg(ap, stackElement_t);
        else st->data[i] = va_arg(ap, int);
    }

    va_end(ap);

    IF_STACK_DEBUG(
        st->file = file;
        st->func = func;
        st->line = line;
        st->name = name;
    )

    IF_STACK_HASH(st->hash = getHash(st, sizeof(stack_t));)

    assert(stackVerifier(st));
    
    return STACK_OK;
}

ErrorStatusStack stackPush(stack_t *st, stackElement_t el) {
    assert(stackVerifier(st));

    log("push el: " STACK_ELEMENT_FORMATER " to %p\n", el, st);

    if (st->size == st->capacity) {
        ErrorStatusStack err = stackResize(st, st->capacity * 2);
        if (err == STACK_MEMORY_ERROR) {
            logError("Can't resize stack %p: no memory\n", st);
            return STACK_MEMORY_ERROR;
        }
    }

    st->data[st->size++] = el;

    recountHash(st);
    assert(stackVerifier(st));

    return STACK_OK;
}

stackElement_t stackPop(stack_t *st) {
    assert(stackVerifier(st));

    if (st->size == 0) {
        logError("try pop from zero-size stack %p\n", st);
        stackDump(st);
        abort();
    }

    stackElement_t return_value = st->data[--st->size];
    recountHash(st);

    log("pop el: " STACK_ELEMENT_FORMATER " from stack %p\n", return_value, st);

    if (isNeedStackResize(st)) stackResize(st, st->capacity / 2);

    recountHash(st);
    assert(stackVerifier(st));

    return return_value;
}

stackElement_t stackPopLoyal(stack_t *st) {
    assert(stackVerifier(st));

    if (st->size == 0) {
        log("pop loyal 0 from stack %p\n", st);
        return (stackElement_t){0};
    }

    stackElement_t return_value = st->data[--st->size];
    recountHash(st);

    log("pop el: " STACK_ELEMENT_FORMATER " from stack %p\n", return_value, st);

    if (isNeedStackResize(st)) stackResize(st, st->capacity / 2);

    recountHash(st);
    assert(stackVerifier(st));

    return return_value;
}

ErrorStatusStack stackPopTo(stack_t *st, stackElement_t *el) {
    assert(stackVerifier(st));
    assert(el);

    if (st->size == 0) {
        logError("try pop from zero-size stack %p\n", st);
        return STACK_ZERO_SIZE_POP_ERROR;
    }

    *el = st->data[--st->size];
    recountHash(st);

    log("pop el: " STACK_ELEMENT_FORMATER " from stack %p\n", *el, st);
    
    if (isNeedStackResize(st)) stackResize(st, st->capacity / 2);

    recountHash(st);
    assert(stackVerifier(st));
    
    return STACK_OK;
}

void stackDtor(stack_t *st) {
    assert(stackVerifier(st));

    log("destroy stack %p\n", st);

    #ifdef STACK_CANARY_PROTECTION
    free(st->buffer);
    #else
    free(st->data);
    #endif
    *st = (stack_t){0};
}

ErrorStatusStackVerifier isStackFailed(stack_t *st) {
    if (st == NULL) {
        logError("pointer st is NULL\n");
        return VERIFY_STACK_NULLPTR;
    }
    if (st->size > st->capacity) {
        logError("stack size > stack capacity\n");
        return VERIFY_STACK_SIZE_MORE_CAPACITY;
    } 
    if (st->data == NULL) {
        logError("stack data is NULL\n");
        return VERIFY_STACK_DATA_NULLPTR;
    }
    IF_STACK_CANARY(
        if (st->canary1 != CANARY1) {
            logError("canary1 was changed\n");
            return VERIFY_STACK_CANARY1;
        }
        if (st->canary2 != CANARY2) {
            logError("canary2 was changed\n");
            return VERIFY_STACK_CANARY2;
        }

        size_t data_size = st->capacity * sizeof(stackElement_t);
        if (data_size % sizeof(canary_t)) {
            data_size += (sizeof(canary_t) - data_size % sizeof(canary_t));
        }
        
        if (((canary_t*)st->buffer)[0] != CANARY3) {
            logError("canary3 was changed\n");
            return VERIFY_STACK_CANARY3;
        }
        if (((canary_t*)st->buffer)[data_size / sizeof(canary_t) + 1] != CANARY4) {
            logError("canary4 was changed\n");
            return VERIFY_STACK_CANARY4;
        }
    )
    
    IF_STACK_HASH(
        uint64_t hash = st->hash;
        st->hash = 0;
        if (getHash(st, sizeof(stack_t)) != hash) return VERIFY_STACK_HASH;
        st->hash = hash;
    )

    return VERIFY_STACK_OK;
}

#ifdef STACK_DEBUG
static bool stackVerifier(stack_t *st) {
    int failed = isStackFailed(st);
    if (failed) {
        stackDump(st);
    }
    return !failed;
}

static void stackDump(stack_t *st) {
    if (st == NULL) return;
 
    log("Dump stack made in file %s by function %s in line %d:\n", st->file, st->func, st->line);
    log("Stack_t %s [%p] {\n", st->name + 1, st); // + 1 - убрать & в начале
    log("    size     = %zu\n",  st->size);
    log("    capacity = %zu\n",  st->capacity);
    
    IF_STACK_CANARY(
    log("    canary1  = %lx\n", st->canary1);
    log("    canary2  = %lx\n", st->canary2);
    )

    IF_STACK_HASH(
    log("    hash     = %lu\n", st->hash);
    )

    if (st->data == NULL) return;
    
    IF_STACK_CANARY(
        size_t data_size = st->capacity * sizeof(stackElement_t);
        if (data_size % sizeof(canary_t)) data_size += (sizeof(canary_t) - data_size % sizeof(canary_t));

        log("    canary3  = %lx\n", ((canary_t*)st->buffer)[0]);
        log("    canary4  = %lx\n", ((canary_t*)st->buffer)[data_size / sizeof(canary_t) + 1]);
    )
    log("    data[] = {\n");
    for (size_t i = 0; i < MIN(st->size, st->capacity); i++) {
        log("         *[%2zu] = " STACK_ELEMENT_FORMATER "\n", i, st->data[i]);
    }
    for (size_t i = st->size; i < st->capacity; i++) {
        log("          [%2zu] = " STACK_ELEMENT_FORMATER "\n", i, st->data[i]);
    }
    log("    }\n");
    log("}\n");
}
#else
static bool stackVerifier(stack_t *st) {return true;}
static void stackDump(stack_t *st) {}
#endif

size_t getStackSize(stack_t *st) {
    assert(stackVerifier(st));

    return st->size;
}

ErrorStatusStack stackResize(stack_t *st, size_t new_size) {
    assert(stackVerifier(st));

    log("resize a stack %p to size %zu\n", st, new_size);

    if (new_size < st->size) {
        logError("resize to new_size < stack size of %p\n", st);
        stackDump(st);
        return STACK_RESIZE_ERROR;
    }

    #ifdef STACK_CANARY_PROTECTION
    size_t data_size_now = st->capacity * sizeof(stackElement_t);
    if (data_size_now % sizeof(canary_t)) data_size_now += (sizeof(canary_t) - data_size_now % sizeof(canary_t));

    size_t data_size = new_size * sizeof(stackElement_t);
    if (data_size % sizeof(canary_t)) data_size += (sizeof(canary_t) - data_size % sizeof(canary_t));

    st->buffer[data_size_now / sizeof(canary_t) + 1] = (canary_t){0};

    canary_t* ptr = (canary_t*)realloc(st->buffer, data_size + 2 * sizeof(canary_t));
    if (ptr == NULL) {
        logError("Can't create stack %p: no memory\n", st);
        st->buffer[data_size_now / sizeof(canary_t) + 1] = (canary_t){0};
        return STACK_MEMORY_ERROR;
    }

    ptr[0] = CANARY3;
    ptr[data_size / sizeof(canary_t) + 1] = CANARY4;

    st->data =   (stackElement_t*)(ptr + 1);
    st->buffer = (stackElement_t*)(ptr);
    #else
    stackElement_t *ptr = (stackElement_t*)realloc(st->data, new_size * sizeof(stackElement_t));
    if (ptr == NULL) {
        return STACK_MEMORY_ERROR;
    }
    
    st->capacity = new_size;
    st->data = ptr;
    #endif

    st->capacity = new_size;

    recountHash(st);

    assert(stackVerifier(st));

    return STACK_OK;
}

void _stackfPrint(FILE *stream, stack_t *st, const char *name) {
    assert(stackVerifier(st));

    fprintf(stream, "Stack_t %s [%p] {\n", name + 1, st);
    fprintf(stream, "    size = %zu\n", st->size);
    fprintf(stream, "    capacity = %zu\n", st->capacity);
    fprintf(stream, "    data[] = {\n");
    for (size_t i = 0; i < MIN(st->size, st->capacity); i++) {
        fprintf(stream, "         *[%2zu] = " STACK_ELEMENT_FORMATER "\n", i, st->data[i]);
    }
    for (size_t i = st->size; i < st->capacity; i++) {
        fprintf(stream, "          [%2zu] = " STACK_ELEMENT_FORMATER "\n", i, st->data[i]);
    }
    fprintf(stream, "    }\n}\n");
}

void _stackPrint(stack_t *st, const char *name) {
    assert(stackVerifier(st));
    _stackfPrint(stdout, st, name);
}

#ifdef STACK_HASH_PROTECTION
static void recountHash(stack_t *st) {
    assert(st);

    st->hash = 0;
    uint64_t hash = getHash(st, sizeof(stack_t));
    st->hash = hash;
}
#else
static void recountHash(stack_t *st) {return;}
#endif

