#include "processor.h"
#include "assembler.h"

void execute(const char* filename){
    FILE* filePtr = fopen(filename, "r");
    if (filePtr == NULL)
    {
        printErr("Unable to open file\n");
        return;
    }
    Stack* stack;
    initialize(&stack);
    int cmd = 0;
    double x, a, b;
    while (fscanf(filePtr, "%d", &cmd) == 1) {
        switch (cmd)
        {
        case HLT:
            fclose(filePtr);
            return;
        case PUSH:
            fscanf(filePtr, "%lg", &x);
            push(stack, x);
            break;
        case ADD:
            pop(stack, &b);
            pop(stack, &a);
            push(stack, a + b);
            break;
        case SUB:
            pop(stack, &b);
            pop(stack, &a);
            push(stack, a + b);
            break;
        case MUL:
            pop(stack, &b);
            pop(stack, &a);
            push(stack, a * b);
            break;
        case DIV:
            pop(stack, &b);
            pop(stack, &a);
            push(stack, a / b);
            break;
        case OUT:
            top(stack, &a);
            printf("%lg\n", a);
            break;
        default:
            break;
        }
    }
}