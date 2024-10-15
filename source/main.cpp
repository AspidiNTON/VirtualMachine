#include <stdlib.h>

#include "assembler.h"
#include "processor.h"

int main() {
    assemble("aboba.asm");
    execute("aboba.txt");
    return 0;
}
