#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <sys/stat.h>
#include <math.h>

#include "stack.h"
#include "cmds.h"
#include "spu.h"

void execute(const char* filename);

#endif