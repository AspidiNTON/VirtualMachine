#include "stack.h"

typedef uint64_t canary_t;

const double RESIZE_COEFF = 1.4;
const int MIN_STACK_SIZE = 8;
const canary_t CANARY = 133769420;
const uint32_t IS_INIT = 0xDD;

#define CANARY_PROTECT

//ON_CANARY(*(canary_t)stack->data = CANARY);

//#define ON_CANARY(...) // либо раскрываться в пустоту, либо 
//#define ON_HASH(...)

// Зачем нужен static если вроде как и так не видно
//static stackErrorType updateStackCapacity(Stack* stack);


static stackErrorType updateStackCapacity(Stack* stack);

static uint64_t calcHash(const void* ptr, size_t size);

static uint64_t calcStackHash(const Stack* stack);

static uint64_t calcStructHash(const Stack* stack);

static stackErrorType checkStackHash(const Stack* stack);

static stackErrorType checkCanary(const Stack* stack);

static stackErrorType checkStack(const Stack* stack);



struct Stack_ {
    //canary_t leftCanary;
    uint32_t isInit;
    char* data;
    int size;
    int capacity;
    uint64_t hash;
    uint64_t structHash;
    //canary_t rightCanary;
};


stackErrorType initialize(Stack** stack) {

    *stack = (Stack*)malloc(sizeof(Stack));

    /*if ((*stack)->isInit == IS_INIT) {
        printErr("Error: stack already initialized\n");
        return REINITIALISATION_ERROR;
    }*/

    (*stack)->capacity = MIN_STACK_SIZE;
    (*stack)->size = 0;
    (*stack)->data = (char*)calloc((*stack)->capacity * sizeof(StackElem) + sizeof(CANARY) * 2, 1);
    if ((*stack)->data == NULL) {
        printErr("Error: unable to init memory\n");
        return INITIALISATION_ERROR;
    }
    *(canary_t*)(*stack)->data = CANARY;
    *(canary_t*)((*stack)->data + (*stack)->capacity * sizeof(StackElem) + sizeof(CANARY)) = CANARY;
    (*stack)->hash = calcStackHash((*stack));
    (*stack)->isInit = IS_INIT;
    (*stack)->structHash = calcStructHash(*stack);
    return NO_ERROR;
}

stackErrorType destroy(Stack* stack) {
    stackErrorType err = checkStack(stack);
    if (err != NO_ERROR) return err;
    memset(stack->data, 0, stack->capacity * sizeof(StackElem) + 2 * sizeof(CANARY));
    free(stack->data);
    stack->data = NULL;
    stack->isInit = 0;
    stack->structHash = calcStructHash(stack);
    return NO_ERROR;
}

stackErrorType push(Stack* stack, StackElem elem) {
    stackErrorType err = checkStack(stack);
    if (err != NO_ERROR) return err;
    ++stack->size;
    err = updateStackCapacity(stack);
    if (err != NO_ERROR) return err;
    *(StackElem*)((stack->data + sizeof(StackElem) * (stack->size - 1) + sizeof(CANARY))) = elem;
    stack->hash = calcStackHash(stack);
    stack->structHash = calcStructHash(stack);
    return NO_ERROR;
}

stackErrorType pop(Stack* stack, StackElem* elem) {
    if (elem == NULL) {
        printErr("Error: nullptr recieved\n");
        return NULLPTR_ERROR;
    }
    stackErrorType err = checkStack(stack);
    if (err != NO_ERROR) return err;
    if (stack->size < 1) {
        printErr("Error: stack is empty\n");
        return EMPTY_ERROR;
    }
    *elem = *(StackElem*)(stack->data + sizeof(StackElem) * (stack->size - 1) + sizeof(CANARY));
    --stack->size;
    err = updateStackCapacity(stack);
    if (err != NO_ERROR) return err;
    stack->hash = calcStackHash(stack);
    stack->structHash = calcStructHash(stack);
    return NO_ERROR;
}

stackErrorType top(Stack* stack, StackElem* elem){
    stackErrorType err = checkStack(stack);
    if (err != NO_ERROR) return err;
    if (stack->size < 1) {
        printErr("Error: stack is empty\n");
        return EMPTY_ERROR;
    }
    *elem = *(StackElem*)(stack->data + sizeof(StackElem) * (stack->size - 1) + sizeof(CANARY));stack->hash = calcStackHash(stack);
    stack->structHash = calcStructHash(stack);
    return NO_ERROR;
}

/*stackErrorType printStack(const Stack* stack) {
    stackErrorType err = checkStack(stack);
    if (err != NO_ERROR) return err;
    printf("Stack [0x%p]\n", stack);
    printf("Size = %d\n", stack->size);
    printf("Capacity = %d\n", stack->capacity);
    printf("Data [0x%p]\n", stack->data);
    printf("Hash = %llu\n", stack->hash);
    printf("Canary: %llu\n", *(canary_t*)stack->data);
    for (int i = 0; i < stack->size; ++i) {
        printf("[%d] = %d\n", i, *(StackElem*)(stack->data + i * sizeof(StackElem) + sizeof(CANARY)));
    }
    for (int i = stack->size; i < stack->capacity; ++i) {
        printf("*[%d] = %d\n", i, *(StackElem*)(stack->data + i * sizeof(StackElem) + sizeof(CANARY)));
    }
    printf("Canary: %llu\n", *(canary_t*)(stack->data + stack->capacity * sizeof(StackElem) + sizeof(CANARY)));
    putchar('\n');
    return NO_ERROR;
}*/


static stackErrorType updateStackCapacity(Stack* stack) {
    if (stack->size > stack->capacity) {
        void* tmp = recalloc(stack->data, stack->capacity * sizeof(StackElem) + sizeof(CANARY) * 2, (stack->size * RESIZE_COEFF) * sizeof(StackElem) + sizeof(CANARY) * 2);
        if (tmp == NULL) {
            printErr("Error: unable to recalloc stack\n");
            return RESIZE_ERROR;
        }
        stack->data = (char*)tmp;
        stack->capacity = stack->size * RESIZE_COEFF;
        *(canary_t*)(stack->data + stack->capacity * sizeof(StackElem) + sizeof(CANARY)) = CANARY;
    }
    if (max(stack->size * RESIZE_COEFF, MIN_STACK_SIZE) < stack->capacity) {
        void* tmp = recalloc(stack->data, stack->capacity * sizeof(StackElem) + sizeof(CANARY) * 2, max(stack->size, MIN_STACK_SIZE) * sizeof(StackElem) + sizeof(CANARY) * 2);
        if (tmp == NULL) {
            printErr("Error: unable to recalloc stack\n");
            return RESIZE_ERROR;
        }
        stack->data = (char*)tmp;
        stack->capacity = max(stack->size, MIN_STACK_SIZE);
        *(canary_t*)(stack->data + stack->capacity * sizeof(StackElem) + sizeof(CANARY)) = CANARY;
    }
    return NO_ERROR;
}

// uint64_t calcHash(const void*, size_t)

static uint64_t calcHash(const void* ptr, size_t size){
    uint64_t hash = 5381;
    for (int i = 0; i < (int)size; ++i) {
        hash = hash * 33 ^ *((char*)ptr + i);
    }
    return hash;
}

// calcStackStructHash
static uint64_t calcStackHash(const Stack* stack) {
    return calcHash(stack->data + sizeof(CANARY), stack->size * sizeof(StackElem));
}

static uint64_t calcStructHash(const Stack* stack) {
    return calcHash(stack, sizeof(Stack) - sizeof(uint64_t));
}

static stackErrorType checkStackHash(const Stack* stack) {
    if (calcStackHash(stack) != stack->hash) {
        printErr("Error: hash was modified\n");
        return HASH_ERROR;
    }
    return NO_ERROR;
}

static stackErrorType checkStructHash(const Stack* stack) {
    if (calcStructHash(stack) != stack->structHash) {
        printErr("Error: struct hash was modified\n");
        return STRUCT_HASH_ERROR;
    }
    return NO_ERROR;
}

static stackErrorType checkCanary(const Stack* stack){
    if (*(canary_t*)stack->data != CANARY) {
        printErr("Error: wrong left canary\n");
        return CANARY_ERROR;
    }
    if (*(canary_t*)(stack->data + stack->capacity * sizeof(StackElem) + sizeof(canary_t)) != CANARY) {
        printErr("Error: wrong right canary\n");
        return CANARY_ERROR;
    }
    return NO_ERROR;
}

static stackErrorType checkStack(const Stack* stack){
    if ((size_t)stack <= 10000) {
        printErr("Error: nullptr recieved\n");
        return NULLPTR_ERROR;
    }
    if (checkStructHash(stack) != NO_ERROR) return HASH_ERROR;
    if (stack->isInit != IS_INIT) {
        printErr("Error: stack is unitialized\n");
        return UNINITIALIZED_STACK_ERROR;
    }
    if (stack->size > stack->capacity) {
        printErr("Error: size is bigger than capacity\n");
        return CAPACITY_ERROR;
    }
    if (checkCanary(stack) != NO_ERROR) return CANARY_ERROR;
    if (checkStackHash(stack) != NO_ERROR) return HASH_ERROR;
    return NO_ERROR;
}