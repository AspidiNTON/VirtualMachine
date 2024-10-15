#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errorTypes.h"

enum CMDS{
    HLT,
    PUSH,
    ADD,
    SUB,
    MUL,
    DIV,
    OUT,
    IN,
    SQRT,
    SIN,
    COS,
    DUMP
};


bool assemble(const char* filename);

#endif