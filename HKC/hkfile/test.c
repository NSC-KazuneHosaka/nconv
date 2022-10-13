#include "hkfile.h"
#include "../hkmem/hkmem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int userfunc1(char** str);
int userfunc2(char** str);

int main(void) {
    wchar_t** filenames = NULL;
    int ret = 0;
    int i=0;

    hkfileinit();

    hkAddFilter_in(L"*hkfile*");
    hkAddFilter_out(L".*");
    ret = hkgetfilenamesWithFilter(&filenames, L".");
    if(ret == 0) {
        i=0;
        while(filenames[i][0] != '\0') {
            printf("%ls\n", filenames[i]);
            i++;
        }
    }
    else {
        hkfileShowErrors(stdout);
    }

    hkmakedir("dir");


    printf("success\n");
    return 0;
}

int userfunc1(char** str) {
    userfunc2(str);
    return 0;
}

int userfunc2(char** str) {
    char str_[] = "abcdefghijklmnopqrstuvwxyz";
    hkmalloc(&(*str), sizeof(str_));
    strncpy(*str, str_, sizeof(str_));
    return 0;
}
