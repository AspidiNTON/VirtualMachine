#ifndef CMDS_H
#define CMDS_H

enum CMDS{
    HLT,
    PUSH,
    POP,
    ADD,
    SUB,
    MUL,
    DIV,
    OUT,
    IN,
    SQRT,
    SIN,
    COS,
    DUMP,
    JMP,
    JA,
    JB,
    JAE,
    JBE,
    JE,
    JNE
};

enum PushMask {
    PUSH_CONST = 1,
    PUSH_REGISTER = 2,
    PUSH_RAM = 4
};

enum PopMask{
    POP_CONST = 1,
    POP_REGISTER = 2,
    POP_RAM = 4
};

#endif