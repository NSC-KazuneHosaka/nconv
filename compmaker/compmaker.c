#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_SIZE 1024

int openFile(FILE** fpp, const char* fileName, const char* openMode);
bool isNullEnd(const char* str, size_t maxsize);

int main(int argc, char* argv[]){
    char input[MAX_SIZE] = {0};
    char output[MAX_SIZE] = {0};
    char buf[MAX_SIZE] = {0};
    FILE* fpin=NULL;
    FILE* fpout = NULL;
    int i=0, j=0;

    /* 入力ファイル名 */
    if (argc <= 1){
        printf("入力ファイルを指定してください：");
        fgets(input, sizeof(input), stdin);

        i=0;
        while(i<sizeof(input)){
            if(input[i] == '\n'){
                input[i] = '\0';
                break;
            }
            i++;
        }
    }
    else{
        strncpy(input, argv[1], sizeof(input));
        if(!isNullEnd(input, sizeof(input))) {
            printf("The input file is incorrect or name is too long.\n");
            return 1;
        }
    }

    /* ファイルオープン */
    if(openFile(&fpin, input, "r") != 0) {
        printf("openFile() error\n");
        return 1;
    }

    /* 出力ファイルオープン */
    if(argc >= 3){
        strncpy(output, argv[2], sizeof(output));
        if(!isNullEnd(output, sizeof(output))) {
            printf("The output file is incorrect or name is too long.\n");
            printf("make output.csv\n");
            strncpy(output, "output.csv", sizeof(output));
        }
    }
    else {
        snprintf(output, sizeof(output)-1, "%s%s", "out_", input);
    }
    if(openFile(&fpout, output, "w") != 0) {
        printf("openFile() error\n");
        return 1;
    }

    /* ファイルオープン */
    if(openFile(&fpout, output, "w") != 0) {
        printf("openFile() error\n");
        return 1;
    }

    while(fgets(buf, sizeof(buf), fpin)) {
        /* 最後のカンマ以外をコピー */
        i=0;
        while(buf[i] == ',') {
            output[i] = buf[i];
            i++;
        }
        while(buf[i] != ',' && buf[i] != '\n') {
            output[i] = buf[i];
            i++;
        }
        output[i] = '\n';
        output[i+1] = '\0';

        fprintf(fpout, "%s", output);
    }

    fclose(fpin);
    fclose(fpout);
    return 0;
}

int openFile(FILE** fpp, const char* fileName, const char* openMode){
    int i=0;

    *fpp = fopen(fileName, openMode);

    if (fpp == NULL) {
        return 1;
    }

    return 0;
}

bool isNullEnd(const char* str, size_t maxsize) {
    int i=0;
    while (i<maxsize) {
        if(str[i] == '\0'){ return true; }
        i++;
    }

    return false;
    
}