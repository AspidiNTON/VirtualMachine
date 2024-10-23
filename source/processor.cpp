#include "processor.h"

struct SPU{
    Stack* stack;
    char* code;
    double regs[4];
    double ram[1000];
    int ip = 0;
};

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

void pushCommand(SPU * proc) {
    char mask = proc->code[++proc->ip]; ++proc->ip;
    double res = 0;
    if (mask & PUSH_REGISTER) res += proc->regs[(int)proc->code[proc->ip++]];
    if (mask & PUSH_CONST) { res += *(double*)(proc->code + proc->ip); proc->ip += sizeof(double); }
    if (mask & PUSH_RAM) res = proc->ram[(int)res];
    push(proc->stack, res);
}

void popCommand(SPU * proc) {
    double x = 0;
    if (pop(proc->stack, &x) != NO_ERROR) {
        printErr("Ummm, akshually stack is empty you silly goober\n");
        exit(0);
        return;
    }
    char mask = proc->code[++proc->ip]; ++proc->ip;
    if (mask & POP_CONST && mask & POP_RAM) { proc->ram[(int)(*(double*)(proc->code + proc->ip))] = x; proc->ip += sizeof(double); }
    if (mask & POP_REGISTER && mask & POP_RAM) proc->ram[(int)proc->regs[(int)proc->code[proc->ip++]]] = x;
    if (mask & POP_REGISTER && !(mask & POP_RAM)) proc->regs[(int)proc->code[proc->ip++]] = x;}

void execute(const char* filename){
    FILE* filePtr = fopen(filename, "rb");
    if (filePtr == NULL)
    {
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

    //int line = 0;
    double a, b;
    while (true) {
        //printf("Line %d; ip: %d\n", line++, proc.ip);
        //printSPU(&proc);
        switch (proc.code[proc.ip])
        {
        case HLT: fclose(filePtr); return;
        case PUSH: pushCommand(&proc); break;
        case POP: popCommand(&proc); break;
        case JMP: proc.ip = *(int*)(proc.code + proc.ip + 1); break;
        case ADD:
            pop(proc.stack, &b), pop(proc.stack, &a);
            push(proc.stack, a + b);
            ++proc.ip; break;
        case SUB:
            pop(proc.stack, &b), pop(proc.stack, &a);
            push(proc.stack, a - b);
            ++proc.ip; break;
        case MUL:
            pop(proc.stack, &b), pop(proc.stack, &a);
            push(proc.stack, a * b);
            ++proc.ip; break;
        case DIV:
            pop(proc.stack, &b), pop(proc.stack, &a);
            push(proc.stack, a / b);
            ++proc.ip; break;
        case IN:
            scanf("%lg", &a);
            push(proc.stack, a);
            ++proc.ip; break;
        case OUT:
            pop(proc.stack, &a);
            printf("%lg\n", a);
            ++proc.ip; break;
        case SQRT:
            pop(proc.stack, &a);
            push(proc.stack, sqrt(a));
            ++proc.ip; break;
        case JA:
            pop(proc.stack, &b), pop(proc.stack, &a);
            if (b > a) proc.ip = *(int*)(proc.code + proc.ip + 1);
            else proc.ip += 1 + sizeof(int);
            break;
        case JB:
            pop(proc.stack, &b), pop(proc.stack, &a);
            if (b < a) proc.ip = *(int*)(proc.code + proc.ip + 1);
            else proc.ip += 1 + sizeof(int);
            break;
        case JAE:
            pop(proc.stack, &b), pop(proc.stack, &a);
            if (b >= a) proc.ip = *(int*)(proc.code + proc.ip + 1);
            else proc.ip += 1 + sizeof(int);
            break;
        case JBE:
            pop(proc.stack, &b), pop(proc.stack, &a);
            if (b <= a) proc.ip = *(int*)(proc.code + proc.ip + 1);
            else proc.ip += 1 + sizeof(int);
            break;
        case JE:
            pop(proc.stack, &b), pop(proc.stack, &a);
            if (b == a) proc.ip = *(int*)(proc.code + proc.ip + 1);
            else proc.ip += 1 + sizeof(int);
            break;
        case JNE:
            pop(proc.stack, &b), pop(proc.stack, &a);
            if (b != a) proc.ip = *(int*)(proc.code + proc.ip + 1);
            else proc.ip += 1 + sizeof(int);
            break;
        default:
            break;
        }
    }
}