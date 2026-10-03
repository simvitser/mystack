# mystack

Реализация защищённого стека.

## Реализовано
- Канарейки по краям структуры стека
- Канарейки по краям данных стека
- Хэш структуры стека
- Множество assert'ов при любых действиях со стеком

## Функции
```
int isStackFailed(stack_t *st); // проверка стека

ErrorStatusStack stackPush(stack_t *st, stackElement_t el); // добавление элемента в стек

stackElement_t   stackPop(stack_t *st);                       // получение элемента из стека, если стек пустой - abort()
stackElement_t   stackPopLoyal(stack_t *st);                  // получение элемента из стека, если стек пустой - return 0
ErrorStatusStack stackPopTo(stack_t *st, stackElement_t *el); // получение элемента из стека, если стек пустой - return STACK_ZERO_SIZE_POP_ERROR

void stackDtor(stack_t *st); // уничтожает стек

size_t getStackSize(stack_t *st); // получение размера стека

ErrorStatusStack stackResize(stack_t *st, size_t new_size); // изменение размера стека
```
## Макросы
```
void stackPrint(stack_t *st); // вывод стека

void stackfPrint(FILE *stream, stack_t *st); // вывод стека в файл

ErrorStatusStack stackCtor(stack_t *st); // создание стека
ErrorStatusStack stackCtorN(stack_t *st, size_t n, ...); // создание стека с заданым размером и элементами
```

## Пример использования 
```
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "stack.h"
#include "log.h"


int main() {
    logInit("log.txt");
    
    log("Start stack program\n");

    stack_t st = {};
    ErrorStatusStack err = stackCtorN(&st, 3, 11, 12, 13);
    
    if (err == STACK_MEMORY_ERROR) {
        fprintf(stderr, "MEMORY ERR ctor\n");
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
    
    stackPop(&st);

    for (int i = 0; i < 6; i++) {
        printf(STACK_ELEMENT_FORMATER " ", stackPopLoyal(&st));
    }
    printf("\n");


    printf("-----------\n\n");
    stackPrint(&st);

    stackDtor(&st);
    logDestroy();
    return 0;
}
```

## Страшно, очень страшно, я не знаю что это такое, если бы я знал что это такое, но я не знаю что это такое

