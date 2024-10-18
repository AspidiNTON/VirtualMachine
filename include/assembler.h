#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmds.h"
#include "errorTypes.h"

bool assemble(const char* filename);

#endif