#include "assembler.h"



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

bool assemble(const char* filename){
    char* outputFilename = getOutputFilename(filename);
    if (outputFilename == NULL) return false;

    FILE* filePtr = fopen(filename, "r");
    if (filePtr == NULL)
    {
        printErr("Unable to open file\n");
        return false;
    }
    
    /*struct stat sb;
    if (stat(filename, &sb) == -1) 
        printErr("Failed to read file stats");
        return false;
    textFile->size = sb.st_size;
    textFile->buff = (char*)calloc(textFile->size + 2, sizeof(char));*/


    char code[1000];
    int ip = 0;

    char str[100] = "";
    int strLen = 0;
    double constValue = 0;
    while (fscanf(filePtr, "%s", str) == 1) {
        if (stricmp(str, "hlt") == 0) {
            code[ip++] = HLT;
        } else if (stricmp(str, "push") == 0) {
            code[ip++] = PUSH;
            fgetc(filePtr);
            fgets(str, 100, filePtr);
            strLen = strlen(str);
            str[strLen - 1] = '\0';
            --strLen;

            if (str[0] == '[' && str[strLen - 1] == ']') {
                char * plusPos = strchr(str, '+');
                if (plusPos == NULL) {
                    if (str[1] >= 'A' && str[1] <= 'D' && str[2] == 'X') {
                        code[ip++] = PUSH_RAM | PUSH_REGISTER;
                        code[ip++] = str[1] - 'A';
                    } else {
                        char * trash = NULL;
                        constValue = strtod(str + 1, &trash);
                        if (trash < &str[strLen - 1]) {
                            printErr("Incorrect push argument\n");
                            return false;
                        }
                        code[ip++] = PUSH_RAM | PUSH_CONST;
                        *(double*)(code + ip) = constValue;
                        ip += sizeof(double);
                    }
                } else {
                    if (str[1] >= 'A' && str[1] <= 'D' && str[2] == 'X') {
                        char * trash = NULL;
                        constValue = strtod(plusPos + 1, &trash);
                        if (trash < &str[strLen - 1]) {
                            printErr("Incorrect push argument: check if second argument is double\n");
                            return false;
                        }
                        code[ip++] = PUSH_RAM | PUSH_REGISTER | PUSH_CONST;
                        code[ip++] = str[1] - 'A';
                        *(double*)(code + ip) = constValue;
                        ip += sizeof(double);
                    } else {
                        printErr("Incorrect push argument: registry should be first\n");
                        return false;
                    }
                }
            } else {
                char * plusPos = strchr(str, '+');
                if (plusPos == NULL) {
                    if (str[0] >= 'A' && str[0] <= 'D' && str[1] == 'X') {
                        code[ip++] = PUSH_REGISTER;
                        code[ip++] = str[0] - 'A';
                    } else {
                        char * trash = NULL;
                        constValue = strtod(str, &trash);
                        if (trash < &str[strLen - 1]) {
                            printErr("Incorrect push argument\n");
                            printErr("%lg %s", constValue, trash);
                            return false;
                        }
                        code[ip++] = PUSH_CONST;
                        *(double*)(code + ip) = constValue;
                        ip += sizeof(double);
                    }
                } else {
                    if (str[0] >= 'A' && str[0] <= 'D' && str[1] == 'X') {
                        char * trash = NULL;
                        constValue = strtod(plusPos + 1, &trash);
                        if (trash < &str[strLen]) {
                            printErr("Incorrect push argument: check if second argument is double\n");
                            return false;
                        }
                        code[ip++] = PUSH_REGISTER | PUSH_CONST;
                        code[ip++] = str[0] - 'A';
                        *(double*)(code + ip) = constValue;
                        ip += sizeof(double);
                    } else {
                        printErr("Incorrect push argument: registry should be first\n");
                        return false;
                    }
                }
            }
        } else if (stricmp(str, "pop") == 0){
            code[ip++] = POP;
            fscanf(filePtr, "%s", str);
            strLen = strlen(str);
            if (str[0] == '[' && str[strLen - 1] == ']') {
                if (str[1] >= 'A' && str[1] <= 'D' && str[2] == 'X') {
                    code[ip++] = POP_REGISTER | POP_RAM;
                    code[ip++] = str[1] - 'A';
                } else {
                    char * trash = NULL;
                    constValue = strtod(str + 1, &trash);
                    if (trash < &str[strLen - 1]) {
                        printErr("Incorrect pop argument 1\n");
                        return false;
                    }
                    code[ip++] = POP_CONST | POP_RAM;
                    *(double*)(code + ip) = constValue; ip += sizeof(double);
                }
            } else {
                if (str[0] >= 'A' && str[0] <= 'D' && str[1] == 'X') {
                    code[ip++] = POP_REGISTER;
                    code[ip++] = str[0] - 'A';
                } else {
                    char * trash = NULL;
                    constValue = strtod(str, &trash);
                    if (trash < &str[strLen]) {
                        printErr("Incorrect pop argument 2\n");
                        return false;
                    }
                    code[ip++] = POP_CONST;
                    *(double*)(code + ip) = constValue; ip += sizeof(double);
                }
            }
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
            printErr("Invalid command:\n");
            printErr("%s", str);
                fclose(filePtr);
            return false;
        }
    }
    FILE* outFilePtr = fopen(outputFilename, "wb");
    if (outFilePtr == NULL)
    {
        printErr("Unable to open output file\n");
        fclose(filePtr);
        return false;
    }
    fwrite(code, sizeof(char), ip, outFilePtr);
    fclose(filePtr);
    fclose(outFilePtr);
    return true;
}