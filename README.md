# mystack

Реализация защищённого стека.

## Реализовано
- Канарейки по краям структуры стека
- Канарейки по краям данных стека
- Хэш стека
- Множество assert'ов при любых действиях со стеком

## Функции
- int stackIsFailedNoPrint(stack_t *st);
- ErrorStatusStack stackPush(stack_t *st, stackElement_t el);
- stackElement_t      stackPop(stack_t *st);
- stackElement_t stackPopLoyal(stack_t *st);
- ErrorStatusStack stackPopTo(stack_t *st, stackElement_t *el);
- void stackDetor(stack_t *st);
- size_t getStackSize(stack_t *st);
- ErrorStatusStack stackResize(stack_t *st, size_t new_size);

## Макросы
- void stackPrint(stack_t *st);
- ErrorStatusStack stackCtor(stack_t *st);
- void stackfPrint(FILE *stream, stack_t *st);
- ErrorStatusStack stackCtorN(stack_t *st, size_t n, ...);

