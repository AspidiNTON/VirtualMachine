#ifndef SPU_H
#define SPU_H

#include <math.h>

#include "stack.h"
#include "cmds.h"

const int RAM_SIZE = 1000;

enum SPUStatus{
    SPU_RUNNING,
    SPU_HALT,
    SPU_ERROR_DIVISION_BY_ZERO,
    SPU_ERROR_UNDEFINED_RAM_ADDRESS,
    SPU_ERROR_EMPTY_STACK
};

struct SPU{
    Stack* stack;
    char* code;
    double regs[4];
    double ram[RAM_SIZE];
    int ip = 0;
    int status;
};

#endif