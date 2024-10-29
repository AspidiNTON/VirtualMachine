#include "spu.h"

void hltCommand(SPU * proc){
    proc->status = SPU_HALT;
}

void pushCommand(SPU * proc) {
    char mask = proc->code[++proc->ip]; ++proc->ip;
    double res = 0;
    if (mask & HAS_REGISTER) res += proc->regs[(int)proc->code[proc->ip++]];
    if (mask & HAS_CONST) { res += *(double*)(proc->code + proc->ip); proc->ip += sizeof(double); }
    if (mask & IS_RAM) {
        if ((int)res >= RAM_SIZE || (int)res < 0) {
            proc->status = SPU_ERROR_UNDEFINED_RAM_ADDRESS;
            return;
        }
        res = proc->ram[(int)res];
    }
    push(proc->stack, res);
}

void popCommand(SPU * proc) {
    double x = 0;
    if (pop(proc->stack, &x) != NO_ERROR) {
        proc->status = SPU_ERROR_EMPTY_STACK;
        return;
    }
    char mask = proc->code[++proc->ip]; ++proc->ip;
    if (mask & HAS_CONST && mask & IS_RAM) {
        int address = *(double*)(proc->code + proc->ip);
        if (address >= RAM_SIZE || address < 0) {
            proc->status = SPU_ERROR_UNDEFINED_RAM_ADDRESS;
            return;
        }
        proc->ram[address] = x;
        proc->ip += sizeof(double);
    }
    if (mask & HAS_REGISTER && mask & IS_RAM) {
        int address = proc->regs[(int)proc->code[proc->ip++]];
        if (address >= RAM_SIZE || address < 0) {
            proc->status = SPU_ERROR_UNDEFINED_RAM_ADDRESS;
            return;
        }
        proc->ram[address] = x;
    }
    if (mask & HAS_REGISTER && !(mask & IS_RAM)) proc->regs[(int)proc->code[proc->ip++]] = x;
}

void jmpCommand(SPU * proc) {
    proc->ip = *(int*)(proc->code + proc->ip + 1);
}

void inCommand(SPU * proc){
    double x;
    scanf("%lg", &x);
    push(proc->stack, x);
    ++proc->ip;
}

void outCommand(SPU * proc){
    double x;
    if (pop(proc->stack, &x) != NO_ERROR) {
        proc->status = SPU_ERROR_EMPTY_STACK;
        return;
    }
    printf("%lg\n", x);
    ++proc->ip;
}

void addCommand(SPU * proc){
    double a, b;
    pop(proc->stack, &b);
    if (pop(proc->stack, &a) != NO_ERROR) {
        proc->status = SPU_ERROR_EMPTY_STACK;
        return;
    }
    push(proc->stack, a + b);
    ++proc->ip;
}

void subCommand(SPU * proc){
    double a, b;
    pop(proc->stack, &b);
    if (pop(proc->stack, &a) != NO_ERROR) {
        proc->status = SPU_ERROR_EMPTY_STACK;
        return;
    }
    push(proc->stack, a - b);
    ++proc->ip;
}

void mulCommand(SPU * proc){
    double a, b;
    pop(proc->stack, &b);
    if (pop(proc->stack, &a) != NO_ERROR) {
        proc->status = SPU_ERROR_EMPTY_STACK;
        return;
    }
    push(proc->stack, a * b);
    ++proc->ip;
}

void divCommand(SPU * proc){
    double a, b;
    pop(proc->stack, &b);
    if (pop(proc->stack, &a) != NO_ERROR) {
        proc->status = SPU_ERROR_EMPTY_STACK;
        return;
    }
    if (b == 0) {
        proc->status = SPU_ERROR_DIVISION_BY_ZERO;
        return;
    }
    push(proc->stack, a / b);
    ++proc->ip;
}

void jaCommand(SPU * proc){
    double a, b;
    pop(proc->stack, &b);
    if (pop(proc->stack, &a) != NO_ERROR) {
        proc->status = SPU_ERROR_EMPTY_STACK;
        return;
    }
    if (b > a) proc->ip = *(int*)(proc->code + proc->ip + 1);
    else proc->ip += 1 + sizeof(int);
}

void jaeCommand(SPU * proc){
    double a, b;
    pop(proc->stack, &b);
    if (pop(proc->stack, &a) != NO_ERROR) {
        proc->status = SPU_ERROR_EMPTY_STACK;
        return;
    }
    if (b >= a) proc->ip = *(int*)(proc->code + proc->ip + 1);
    else proc->ip += 1 + sizeof(int);
}

void jbCommand(SPU * proc){
    double a, b;
    pop(proc->stack, &b);
    if (pop(proc->stack, &a) != NO_ERROR) {
        proc->status = SPU_ERROR_EMPTY_STACK;
        return;
    }
    if (b < a) proc->ip = *(int*)(proc->code + proc->ip + 1);
    else proc->ip += 1 + sizeof(int);
}

void jbeCommand(SPU * proc){
    double a, b;
    pop(proc->stack, &b);
    if (pop(proc->stack, &a) != NO_ERROR) {
        proc->status = SPU_ERROR_EMPTY_STACK;
        return;
    }
    if (b <= a) proc->ip = *(int*)(proc->code + proc->ip + 1);
    else proc->ip += 1 + sizeof(int);
}

void jeCommand(SPU * proc){
    double a, b;
    pop(proc->stack, &b);
    if (pop(proc->stack, &a) != NO_ERROR) {
        proc->status = SPU_ERROR_EMPTY_STACK;
        return;
    }
    if (b == a) proc->ip = *(int*)(proc->code + proc->ip + 1);
    else proc->ip += 1 + sizeof(int);
}

void jneCommand(SPU * proc){
    double a, b;
    pop(proc->stack, &b);
    if (pop(proc->stack, &a) != NO_ERROR) {
        proc->status = SPU_ERROR_EMPTY_STACK;
        return;
    }
    if (b != a) proc->ip = *(int*)(proc->code + proc->ip + 1);
    else proc->ip += 1 + sizeof(int);
}

void sqrtCommand(SPU * proc){
    double x;
    if (pop(proc->stack, &x) != NO_ERROR) {
        proc->status = SPU_ERROR_EMPTY_STACK;
        return;
    }
    push(proc->stack, sqrt(x));
    ++proc->ip;
}

void sinCommand(SPU * proc){
    double x;
    if (pop(proc->stack, &x) != NO_ERROR) {
        proc->status = SPU_ERROR_EMPTY_STACK;
        return;
    }
    push(proc->stack, sin(x));
    ++proc->ip;
}

void cosCommand(SPU * proc){
    double x;
    if (pop(proc->stack, &x) != NO_ERROR) {
        proc->status = SPU_ERROR_EMPTY_STACK;
        return;
    }
    push(proc->stack, cos(x));
    ++proc->ip;
}

void callCommand(SPU * proc){
    push(proc->stack, proc->ip + 1 + sizeof(int));
    proc->ip = *(int*)(proc->code + proc->ip + 1);
}
void retCommand(SPU * proc){
    double x;
    if (pop(proc->stack, &x) != NO_ERROR) {
        proc->status = SPU_ERROR_EMPTY_STACK;
        return;
    }
    proc->ip = (int)x;
}