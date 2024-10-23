#include "assembler.h"


typedef uint64_t hash_t;


struct Label{
    char name[40];
    int ip;
    hash_t hash;
    int entries[10];
    int entriesCount = 0;
};

struct Assembler{
    FILE* filePtr;
    char code[1000];
    int ip = 0;
    Label labels[100] = {};
    int labelCount = 0;
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


/*bool readLabelArray(Label *labels, int* labelCount, const char* filename){
    FILE* filePtr = fopen(filename, "r");
    if (filePtr == NULL)
    {
        printErr("Unable to open file\n");
        return false;
    }

}*/

hash_t calcHash(const char* str){
    hash_t hash = 5381;
    while (*str != '\0') {
        hash = hash * 33 ^ *str;
        ++str;
    }
    return hash;
}


bool checkJump(Assembler* ass){
    char str[100] = "";
    fgetc(ass->filePtr);
    fgets(str, 100, ass->filePtr);
    int strLen = strlen(str);
    str[strLen - 1] = '\0';
    --strLen;
    hash_t hash = calcHash(str);
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
        strcpy(ass->labels[ass->labelCount].name, str);
        ++ass->labelCount;
    }
    ass->ip += sizeof(int);
    return true;
}

bool assemblePush(Assembler* ass){
    ass->code[ass->ip++] = PUSH;
    char str[100] = "";
    fgetc(ass->filePtr);
    fgets(str, 100, ass->filePtr);
    int strLen = strlen(str);
    str[strLen - 1] = '\0';
    --strLen;
    if (str[0] == '[' && str[strLen - 1] == ']') {
        char * plusPos = strchr(str, '+');
        if (plusPos == NULL) {
            if (str[1] >= 'A' && str[1] <= 'D' && str[2] == 'X') {
                ass->code[ass->ip++] = PUSH_RAM | PUSH_REGISTER;
                ass->code[ass->ip++] = str[1] - 'A';
            } else {
                char * trash = NULL;
                double constValue = strtod(str + 1, &trash);
                if (trash < &str[strLen - 1]) {
                        printErr("Incorrect push argument\n");
                        return false;
                }
                    ass->code[ass->ip++] = PUSH_RAM | PUSH_CONST;
                    *(double*)(ass->code + ass->ip) = constValue;
                    ass->ip += sizeof(double);
            }
        } else {
            if (str[1] >= 'A' && str[1] <= 'D' && str[2] == 'X') {
                char * trash = NULL;
                double constValue = strtod(plusPos + 1, &trash);
                if (trash < &str[strLen - 1]) {
                    printErr("Incorrect push argument: check if second argument is double\n");
                    return false;
                }
                ass->code[ass->ip++] = PUSH_RAM | PUSH_REGISTER | PUSH_CONST;
                ass->code[ass->ip++] = str[1] - 'A';
                *(double*)(ass->code + ass->ip) = constValue;
                ass->ip += sizeof(double);
            } else {
                printErr("Incorrect push argument: registry should be first\n");
                return false;
            }
        }
    } else {
        char * plusPos = strchr(str, '+');
        if (plusPos == NULL) {
            if (str[0] >= 'A' && str[0] <= 'D' && str[1] == 'X') {
                ass->code[ass->ip++] = PUSH_REGISTER;
                ass->code[ass->ip++] = str[0] - 'A';
            } else {
                char * trash = NULL;
                double constValue = strtod(str, &trash);
                if (trash < &str[strLen - 1]) {
                    printErr("Incorrect push argument\n");
                    printErr("%lg %s", constValue, trash);
                    return false;
                }
                ass->code[ass->ip++] = PUSH_CONST;
                *(double*)(ass->code + ass->ip) = constValue;
                ass->ip += sizeof(double);
            }
        } else {
            if (str[0] >= 'A' && str[0] <= 'D' && str[1] == 'X') {
                char * trash = NULL;
                double constValue = strtod(plusPos + 1, &trash);
                if (trash < &str[strLen]) {
                    printErr("Incorrect push argument: check if second argument is double\n");
                    return false;
                }
                ass->code[ass->ip++] = PUSH_REGISTER | PUSH_CONST;
                ass->code[ass->ip++] = str[0] - 'A';
                *(double*)(ass->code + ass->ip) = constValue;
                ass->ip += sizeof(double);
            } else {
                printErr("Incorrect push argument: registry should be first\n");
                return false;
            }
        }
    }
    return true;
}

bool assemblePop(Assembler* ass){
    ass->code[ass->ip++] = POP;
    char str[100] = "";
    fscanf(ass->filePtr, "%s", str);
    int strLen = strlen(str);
    if (str[0] == '[' && str[strLen - 1] == ']') {
        if (str[1] >= 'A' && str[1] <= 'D' && str[2] == 'X') {
            ass->code[ass->ip++] = POP_REGISTER | POP_RAM;
            ass->code[ass->ip++] = str[1] - 'A';
        } else {
            char * trash = NULL;
            double constValue = strtod(str + 1, &trash);
            if (trash < &str[strLen - 1]) {
                printErr("Incorrect pop argument 1\n");
                return false;
            }
            ass->code[ass->ip++] = POP_CONST | POP_RAM;
            *(double*)(ass->code + ass->ip) = constValue;
            ass->ip += sizeof(double);
        }
    } else {
        if (str[0] >= 'A' && str[0] <= 'D' && str[1] == 'X') {
            ass->code[ass->ip++] = POP_REGISTER;
            ass->code[ass->ip++] = str[0] - 'A';
        } else {
            char * trash = NULL;
            double constValue = strtod(str, &trash);
            if (trash < &str[strLen]) {
                printErr("Incorrect pop argument 2\n");
                return false;
            }
            ass->code[ass->ip++] = POP_CONST;
            *(double*)(ass->code + ass->ip) = constValue;
            ass->ip += sizeof(double);
        }
    }
    return true;
}


bool assemble(const char* filename){
    Assembler ass = {};
    char* outputFilename = getOutputFilename(filename);
    if (outputFilename == NULL) return false;
    ass.filePtr = fopen(filename, "r");
    FILE* filePtr = ass.filePtr;
    if (ass.filePtr == NULL)
    {
        printErr("Unable to open file\n");
        return false;
    }
    char str[100];
    while (fscanf(filePtr, "%s", str) == 1) {
        if (stricmp(str, "hlt") == 0) {
            ass.code[ass.ip++] = HLT;
        } else if (stricmp(str, "push") == 0) {
            if (!assemblePush(&ass)) return false;
        } else if (stricmp(str, "pop") == 0){
            if (!assemblePop(&ass)) return false;
        } else if (stricmp(str, "add") == 0) {
            ass.code[ass.ip++] = ADD;
        } else if (stricmp(str, "sub") == 0) {
            ass.code[ass.ip++] = SUB;
        } else if (stricmp(str, "mul") == 0) {
            ass.code[ass.ip++] = MUL;
        } else if (stricmp(str, "div") == 0) {
            ass.code[ass.ip++] = DIV;
        } else if (stricmp(str, "in") == 0) {
            ass.code[ass.ip++] = IN;
        } else if (stricmp(str, "out") == 0) {
            ass.code[ass.ip++] = OUT;
        } else if (stricmp(str, "sqrt") == 0) {
            ass.code[ass.ip++] = SQRT;
        } else if (stricmp(str, "jmp") == 0) {
            ass.code[ass.ip++] = JMP;
            if (!checkJump(&ass)) return false;
        } else if (stricmp(str, "ja") == 0) {
            ass.code[ass.ip++] = JA;
            if (!checkJump(&ass)) return false;
        } else if (stricmp(str, "jb") == 0) {
            ass.code[ass.ip++] = JB;
            if (!checkJump(&ass)) return false;
        } else if (stricmp(str, "jae") == 0) {
            ass.code[ass.ip++] = JAE;
            if (!checkJump(&ass)) return false;
        } else if (stricmp(str, "jbe") == 0) {
            ass.code[ass.ip++] = JBE;
            if (!checkJump(&ass)) return false;
        } else if (stricmp(str, "je") == 0) {
            ass.code[ass.ip++] = JE;
            if (!checkJump(&ass)) return false;
        } else if (stricmp(str, "jne") == 0) {
            ass.code[ass.ip++] = JNE;
            if (!checkJump(&ass)) return false;
        }
        /* else if (stricmp(str, "add") == 0) {
            fprintf(outFilePtr, "%d\n", ADD);
        } else if (stricmp(str, "sub") == 0) {
            fprintf(outFilePtr, "%d\n", SUB);
        } else if (stricmp(str, "mul") == 0) {
            fprintf(outFilePtr, "%d\n", MUL);
        } else if (stricmp(str, "div") == 0) {
            fprintf(outFilePtr, "%d\n", DIV);
        } else if (stricmp(str, "out") == 0) {
            fprintf(outFilePtr, "%d\n", OUT);
        } else if (stricmp(str, "in") == 0) {
            fprintf(outFilePtr, "%d\n", IN);
        } else if (stricmp(str, "sqrt") == 0) {
            fprintf(outFilePtr, "%d\n", SQRT);
        } else if (stricmp(str, "sin") == 0) {
            fprintf(outFilePtr, "%d\n", SIN);
        } else if (stricmp(str, "cos") == 0) {
            fprintf(outFilePtr, "%d\n", COS);
        } else if (stricmp(str, "dump") == 0) {
            fprintf(outFilePtr, "%d\n", DUMP);
        }*/ else {
            if (str[strlen(str) - 1] != ':') {
                printErr("Invalid command:\n");
                printErr("%s", str);
                fclose(filePtr);
                return false;
            } else {
                str[strlen(str) - 1] = '\0';
                hash_t hash = calcHash(str);
                bool f = true;
                for (int i = 0; i < ass.labelCount; ++i) {
                    if (ass.labels[i].hash == hash) {
                        ass.labels[i].ip = ass.ip;
                        f = false;
                        for (int j = 0; j < ass.labels[i].entriesCount; ++j) {
                            *(int*)(ass.code + ass.labels[i].entries[j]) = ass.ip;
                        }
                    }
                }
                if (f) {
                    ass.labels[ass.labelCount].hash = hash;
                    ass.labels[ass.labelCount].ip = ass.ip;
                    strcpy(ass.labels[ass.labelCount].name, str);
                    ++ass.labelCount;
                }
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