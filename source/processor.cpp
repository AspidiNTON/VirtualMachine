#include "processor.h"

void printSPU(SPU* proc) {
    printf("Stack: ");
    printStack(proc->stack);
    printf("Registers: ");
    for (int i = 0; i < 4; ++i) printf("%lg ", proc->regs[i]);
    printf("\nRAM: ");
    for (int i = 0; i < 20; ++i) printf("%lg ", proc->ram[i]);
    printf("\nip: ");
    printf("%d\n\n", proc->ip);
}

void execute(const char* filename){
    FILE* filePtr = fopen(filename, "rb");
    if (filePtr == NULL){
        printErr("Unable to open file\n");
        return;
    }
    struct stat sb;
    if (stat(filename, &sb) == -1) {
        printErr("Unable to read file stats\n");
        return;
    }
    SPU proc = {};
    proc.code = (char*)calloc(sb.st_size + 1, sizeof(char));
    if (proc.code == NULL) {
        printErr("Unable to calloc\n");
        return;
    }
    if ((int)fread(proc.code, sizeof(char), sb.st_size, filePtr) != sb.st_size) {
        printErr("Binary file read failed\n");
        return;
    }
    initialize(&proc.stack);

    proc.status = SPU_RUNNING;
    while (true) {
        commands[(int)proc.code[proc.ip]].runCommand(&proc);
        if (proc.status != SPU_RUNNING) {
            switch(proc.status){
            case SPU_ERROR_DIVISION_BY_ZERO:
                printErr("Runtime error: division by zero\n");
                break;
            case SPU_ERROR_EMPTY_STACK:
                printErr("Runtime error: empty stack\n");
                break;
            case SPU_ERROR_UNDEFINED_RAM_ADDRESS:
                printErr("Runtime error: undefined RAM address\n");
                break;
            }
        }
    }
}