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
    
    FILE* outFilePtr = fopen(outputFilename, "w");
    if (outFilePtr == NULL)
    {
        printErr("Unable to open output file\n");
        fclose(filePtr);
        return false;
    }
    char str[100] = "";
    double number = 0;
    while (fscanf(filePtr, "%s", str) == 1) {
        if (stricmp(str, "hlt") == 0) {
            fprintf(outFilePtr, "%d\n", HLT);
        } else if (stricmp(str, "push") == 0) {
            if (fscanf(filePtr, "%lg", &number) != 1) {
                printErr("No number after push command\n");
                fclose(filePtr);
                fclose(outFilePtr);
                return false;
            }
            fprintf(outFilePtr, "%d %lg\n", PUSH, number);
        } else if (stricmp(str, "add") == 0) {
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
        } else {
            printErr("Invalid command:\n");
            printErr("%s", str);
                fclose(filePtr);
                fclose(outFilePtr);
            return false;
        }
    }
    fclose(filePtr);
    fclose(outFilePtr);
    return true;
}