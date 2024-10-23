#include <stdlib.h>

#include "assembler.h"
#include "processor.h"

int main() {
    assemble("square.asm");
    execute("square.txt");
    return 0;
}
