#ifndef STACK_H
#define STACK_H

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define STACK_INIT_CAP 1

struct Stack
{
    void *data;
    size_t memb_sz;

    size_t capacity;
    size_t size;
};

struct Stack *StackCtr(size_t memb_sz);
struct Stack *StackDtr(struct Stack *stack);

int StackPush(struct Stack *stack, void *buffer);
int StackPop(struct Stack *stack, void *buffer);
int StackTop(struct Stack *stack, void *buffer);

#endif //STACK_H