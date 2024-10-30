#include "assembler.h"

typedef uint64_t hash_t;

struct Label{
    int ip;
    hash_t hash;
    int entries[10];
    int entriesCount = 0;
};

struct Assembler{
    FILE* filePtr;
    char code[3000];
    int ip = 0;
    char str[200];
    Label labels[100] = {};
    int labelCount = 0;
    hash_t hashTable[sizeof(commands) / sizeof(commands[0])];
};

char* getOutputFilename(const char* filename){
    const char* inputExtension = ".asm";
    const char* outputExtension = ".txt";
    int inputExtensionLength = strlen(inputExtension);
    int outputExtensionLength = strlen(outputExtension);
    int filenameLength = strlen(filename);
    if (strcmp(filename + filenameLength - inputExtensionLength, inputExtension) != 0) {
        printErr("Wrong file extension\n");
        return NULL;
    }
    char* outputFilename = (char*)calloc(filenameLength -
            inputExtensionLength + outputExtensionLength + 1, sizeof(char));
    if (outputFilename == NULL) {
        printErr("Unable to calloc\n");
        return NULL;
    } 
    strncpy(outputFilename, filename, filenameLength - inputExtensionLength);
    strncpy(outputFilename + filenameLength - inputExtensionLength, outputExtension, outputExtensionLength);
    return outputFilename;
}

hash_t calcHash(const char* str){
    hash_t hash = 5381;
    while (*str != '\0') {
        hash = hash * 33 ^ *str;
        ++str;
    }
    return hash;
}

FILE* initAssembler(Assembler* ass, const char* filename){
    ass->filePtr = fopen(filename, "r");
    if (ass->filePtr == NULL)
    {
        printErr("Unable to open file\n");
        return NULL;
    }
    for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); ++i) {
        ass->hashTable[i] = calcHash(commands[i].name);
    }
    return ass->filePtr;
}

int getCommandCodeFromHash(Assembler* ass, hash_t hash){
    for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); ++i) {
        if (hash == ass->hashTable[i]) return i;
    }
    return -1;
}

char* skipSpaces(char* str){
    while (*str == ' ' || *str == '\t') ++str;
    return str;
}

int identifyCommandArguments(Assembler* ass) {
    char* start = skipSpaces(ass->str);
    if (*start == '\n' || *start == ';') return NOTHING;
    char* plus = NULL;
    char* end = start;
    while (*end != ' ' && *end != '\t' && *end != '\n' && *end != ';') {
        if (*end == '+' && plus == NULL) plus = end;
        ++end;
    }

    --end;
    // [] - rize ram bit
    // ...
    if (*start == '[' && *end == ']') {
        if (plus) { // tryPlus(...)
            if (start[2] == 'X' && start[1] >= 'A' && start[1] <= 'D') {
                char * trash = NULL;
                double constValue = strtod(++plus, &trash);
                if (trash < end) return 0;
                ass->code[ass->ip++] = HAS_CONST | HAS_REGISTER | IS_RAM;
                ass->code[ass->ip] = start[1] - 'A';
                ass->ip++;
                *(double*)(ass->code + ass->ip) = constValue;
                ass->ip += sizeof(double);
                return RAM_NUMBER_AND_REGISTER;
             } else return 0;
        } else {
            if (start[2] == 'X' && start[1] >= 'A' && start[1] <= 'D' && end - start == 3) {
                ass->code[ass->ip++] = HAS_REGISTER | IS_RAM;
                ass->code[ass->ip] = start[1] - 'A';
                ass->ip++;
                return RAM_REGISTER;
            } else {
                char * trash = NULL;
                double constValue = strtod(++start, &trash);
                if (trash < end) return 0;
                ass->code[ass->ip++] = HAS_CONST | IS_RAM;
                *(double*)(ass->code + ass->ip) = constValue;
                ass->ip += sizeof(double);
                return RAM_NUMBER;
            }
        }
    } else {
        if (plus) {
            if (start[1] == 'X' && start[0] >= 'A' && start[0] <= 'D') {
                char * trash = NULL;
                double constValue = strtod(++plus, &trash);
                if (trash < end) return 0;
                ass->code[ass->ip++] = HAS_CONST | HAS_REGISTER;
                ass->code[ass->ip] = start[0] - 'A';
                ass->ip++;
                *(double*)(ass->code + ass->ip) = constValue;
                ass->ip += sizeof(double);
                return NUMBER_AND_REGISTER;
             } else return 0;
        } else {
            if (start[1] == 'X' && start[0] >= 'A' && start[0] <= 'D' && end - start == 1) {
                ass->code[ass->ip++] = HAS_REGISTER;
                ass->code[ass->ip] = start[0] - 'A';
                ass->ip++;
                return REGISTER;
            } else {
                char * trash = NULL;
                double constValue = strtod(start, &trash);
                if (trash < end) {
                    *(end + 1) = '\0';
                    hash_t hash = calcHash(start);
                    bool f = true;
                    for (int i = 0; i < ass->labelCount; ++i) {
                        if (ass->labels[i].hash == hash) {
                            if (ass->labels[i].ip == -1) {
                                ass->labels[i].entries[ass->labels[i].entriesCount] = ass->ip;
                                ++ass->labels[i].entriesCount;
                            } else {
                                *(int*)(ass->code + ass->ip) = ass->labels[i].ip;
                            }
                            f = false;
                            break;
                        }
                    }
                    if (f) {
                        ass->labels[ass->labelCount].hash = hash;
                        ass->labels[ass->labelCount].ip = -1;
                        ass->labels[ass->labelCount].entriesCount = 1;
                        ass->labels[ass->labelCount].entries[0] = ass->ip;
                        ++ass->labelCount;
                    }
                    ass->ip += sizeof(int);
                    return LABEL;
                }
                ass->code[ass->ip++] = HAS_CONST;
                *(double*)(ass->code + ass->ip) = constValue;
                ass->ip += sizeof(double);
                return NUMBER;
            }
        }
    }
}

void addNewLabel(Assembler* ass){
    ass->str[strlen(ass->str) - 1] = '\0';
    hash_t hash = calcHash(ass->str);
    bool f = true;
    for (int i = 0; i < ass->labelCount; ++i) {
        if (ass->labels[i].hash == hash) {
            ass->labels[i].ip = ass->ip;
            f = false;
            for (int j = 0; j < ass->labels[i].entriesCount; ++j) {
                *(int*)(ass->code + ass->labels[i].entries[j]) = ass->ip;
            }
        }
    }
    if (f) {
        ass->labels[ass->labelCount].hash = hash;
        ass->labels[ass->labelCount].ip = ass->ip;
        ++ass->labelCount;
    }
}


bool assemble(const char* filename){
    char* outputFilename = getOutputFilename(filename);
    if (outputFilename == NULL) return false;
    Assembler ass = {};
    FILE* filePtr = initAssembler(&ass, filename);
    if (filePtr == NULL) return false;

    while (true) {
        if (fscanf(filePtr, "%199s", ass.str) != 1) break;
        int commandCode = getCommandCodeFromHash(&ass, calcHash(ass.str));
        if (commandCode == -1) {
            if (ass.str[strlen(ass.str) - 1] == ':') {
                addNewLabel(&ass);
            } else {
                if (*skipSpaces(ass.str) != ';'){       
                    printErr("Invalid command:\n");
                    printErr("%s\n", ass.str);
                    return false;
                } else {
                    fgets(ass.str, 199, ass.filePtr);
                }
            }
        } else {
            ass.code[ass.ip++] = commandCode;
            int commandArguments = NOTHING;
            fgets(ass.str, 200, ass.filePtr);
            commandArguments = identifyCommandArguments(&ass);
            if (commandArguments) {
                if (!(commandArguments & commands[commandCode].argumentMask)) {
                    printErr("Command does not accept that argument type:\n");
                    printErr("%s\n", ass.str);
                    return false;
                }
            } else {
                printErr("Invalid arguments:\n");
                printErr("%s\n", ass.str);
                return false;
            }
        }
    }
    FILE* outFilePtr = fopen(outputFilename, "wb");
    if (outFilePtr == NULL)
    {
        printErr("Unable to open output file\n");
        fclose(filePtr);
        return false;
    }
    fwrite(ass.code, sizeof(char), ass.ip, outFilePtr);
    fclose(filePtr);
    fclose(outFilePtr);
    return true;
}