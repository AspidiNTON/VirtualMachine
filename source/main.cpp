#include <stdlib.h>

#include "assembler.h"
#include "processor.h"

int main(int argc, const char *argv[]) {
    if (argc > 2) {
        printErr("Too many arguments");
        return 1;
    }
    if (argc > 1) {
        int strLen = strlen(argv[1]);
        if (strcmp(argv[1] + strLen - 4, ".asm") == 0) {
            assemble(argv[1]);
        } else if (strcmp(argv[1] + strLen - 4, ".txt") == 0) {
            execute(argv[1]);
        } else {
            char asmFilename[80];
            strcpy(asmFilename, argv[1]);
            strcpy(asmFilename + strLen, ".asm");
            char procFilename[80];
            strcpy(procFilename, argv[1]);
            strcpy(procFilename + strLen, ".txt");
            assemble(asmFilename);
            execute(procFilename);
        }
    } else {
        assemble("square.asm");
        execute("square.txt");
    }

    return 0;
}
