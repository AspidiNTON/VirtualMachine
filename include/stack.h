#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "mem.h"
#include "errorTypes.h"

#define max(A, B) ((A) > (B) ? (A) : (B))

typedef double StackElem;

struct Stack_;
typedef struct Stack_ Stack;


stackErrorType initialize(Stack** stack);

stackErrorType destroy(Stack* stack);

stackErrorType push(Stack* stack, StackElem elem);

stackErrorType pop(Stack* stack, StackElem* elem);

stackErrorType top(Stack* stack, StackElem* elem);

stackErrorType printStack(const Stack* stack);

#endif