#ifndef CMDS_H
#define CMDS_H

enum AcceptedArgumentsMask {
    NOTHING             = 1 << 0,
    NUMBER              = 1 << 1,
    REGISTER            = 1 << 2,
    NUMBER_AND_REGISTER = 1 << 3,
    RAM_NUMBER = 16,
    RAM_REGISTER = 32,
    RAM_NUMBER_AND_REGISTER = 64,
    LABEL = 128
};

enum ArgumentsInfoMask {
    HAS_CONST = 1,
    HAS_REGISTER = 2,
    IS_RAM = 4
};

struct SPU;

typedef void (*executeFunction)(SPU*);

struct Command{
    const char* name;
    int argumentMask;
    executeFunction runCommand;
};

void hltCommand(SPU * proc);
void pushCommand(SPU * proc);
void popCommand(SPU * proc);
void jmpCommand(SPU * proc);

void inCommand(SPU * proc);
void outCommand(SPU * proc);
void addCommand(SPU * proc);
void subCommand(SPU * proc);
void mulCommand(SPU * proc);
void divCommand(SPU * proc);

void jaCommand(SPU * proc);
void jaeCommand(SPU * proc);
void jbCommand(SPU * proc);
void jbeCommand(SPU * proc);
void jeCommand(SPU * proc);
void jneCommand(SPU * proc);
void callCommand(SPU * proc);
void retCommand(SPU * proc);

void sqrtCommand(SPU * proc);
void sinCommand(SPU * proc);
void cosCommand(SPU * proc);


const Command commands[] = {
    {"hlt", NOTHING, hltCommand},
    {"push", NUMBER | REGISTER | NUMBER_AND_REGISTER | RAM_NUMBER | RAM_REGISTER | RAM_NUMBER_AND_REGISTER, pushCommand},
    {"pop", REGISTER | RAM_NUMBER | RAM_REGISTER, popCommand},
    {"jmp", LABEL, jmpCommand},

    {"in", NOTHING, inCommand},
    {"out", NOTHING, outCommand},
    {"add", NOTHING, addCommand},
    {"sub", NOTHING, subCommand},
    {"mul", NOTHING, mulCommand},
    {"div", NOTHING, divCommand},
    {"sqrt", NOTHING, sqrtCommand},
    {"sin", NOTHING, sinCommand},
    {"cos", NOTHING, cosCommand},

    {"ja", LABEL, jaCommand},
    {"jae", LABEL, jaeCommand},
    {"jb", LABEL, jbCommand},
    {"jbe", LABEL, jbeCommand},
    {"je", LABEL, jeCommand},
    {"jne", LABEL, jneCommand},
    {"call", LABEL, callCommand},
    {"ret", NOTHING, retCommand}
};

#endif