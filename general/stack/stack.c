#include "stack.h"

static int StackReallocUp(struct Stack *stack);
static int StackReallocDown(struct Stack *stack);

struct Stack *StackCtr(size_t memb_sz)
{
    struct Stack *stack = (struct Stack *)malloc(sizeof(struct Stack));
    assert(stack);

    stack->data = calloc(STACK_INIT_CAP, memb_sz);
    assert(stack->data);

    stack->memb_sz  = memb_sz;
    stack->capacity = STACK_INIT_CAP;
    stack->size     = 0;

    return stack;
}

struct Stack *StackDtr(struct Stack *stack)
{
    free(stack->data);
    free(stack);

    return NULL;
}


static int StackReallocUp(struct Stack *stack)
{
    if(stack->size == stack->capacity)
    {
        void *temp = reallocarray(stack->data, stack->capacity * 2, stack->memb_sz);
        if(!temp) return EXIT_FAILURE;

        stack->data   = temp;
        stack->capacity *= 2;
    }
    return EXIT_SUCCESS;
}

int StackPush(struct Stack *stack, void *buffer)
{
    int realloc_status = StackReallocUp(stack);
    if(realloc_status == EXIT_FAILURE) return EXIT_FAILURE;

    void *dest = (char *)stack->data + stack->size * stack->memb_sz;
    memcpy(dest, buffer, stack->memb_sz);
    stack->size++;

    return EXIT_SUCCESS;
}


static int StackReallocDown(struct Stack *stack)
{
    if(stack->capacity == stack->size * 4)
    {
        void *temp = reallocarray(stack->data, stack->capacity / 2, stack->memb_sz);
        if(!temp) return EXIT_FAILURE;

        stack->data   = temp;
        stack->capacity /= 2;
    }
    return EXIT_SUCCESS;
}

int StackPop(struct Stack *stack, void *buffer)
{
    if(stack->size == 0) return EXIT_FAILURE;

    int realloc_status = StackReallocDown(stack);
    if(realloc_status == EXIT_FAILURE) return EXIT_FAILURE;

    if(buffer != NULL)
    {
        void *src = (char *)stack->data + (stack->size - 1) * stack->memb_sz;
        memcpy(buffer, src, stack->memb_sz);
    }
    stack->size--;

    return EXIT_SUCCESS;
}


int StackTop(struct Stack *stack, void *buffer)
{
    if(stack->size == 0) return EXIT_FAILURE;

    void *src = (char *)stack->data + (stack->size - 1) * stack->memb_sz;
    memcpy(buffer, src, stack->memb_sz);

    return EXIT_SUCCESS;
}