#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "cmds.h"
#include "errorTypes.h"

struct Label{
    char name[25];
    int value;
};

bool assemble(const char* filename);

#endif