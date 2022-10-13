#include "hkstr.h"
#include <stdio.h>

int hkstrmatchW___(const wchar_t* const target, const wchar_t* const str) {
    wprintf(L"%ls and %ls\n", target, str);
    
    const wchar_t* pt = target;
    const wchar_t* ps = str;
    int ret = 0;

    /* target == "" && (ps == "***...." || ps == "") */
    if(*pt == L'\0') {        
        while(*ps == L'*') { ps++; }
        
        ret = *ps == L'\0';
        printf("return %d\n", ret);
        return ret;
    }
    /* target != "" && str == "" */
    else if(*ps == L'\0') { return 0; }
    /* target != "" && *ps == '*' */
    else if(*ps == L'*') {
        ret = *(ps+1) == L'\0'  /* target != "" && str == "*" */
            || hkstrmatchW(pt, ps+1)
            || hkstrmatchW(pt+1, ps);

        printf("return %d\n", ret);
        return ret;
    }
    /* maches each other's character */
    else if(*ps == L'?' || *pt == *ps) {
        pt++;
        ps++;
        ret = hkstrmatchW(pt, ps);
        printf("return %d\n", ret);
        return ret;
    }

    ret = 0;
    printf("return %d\n", ret);
    return ret;
}

int main(void) {
    printf("init : %d\n", hkstrinit());

    printf("%d\n", hkstrmatchW___(L"test.csv", L"*.csv"));
    printf("%d\n", hkstrmatchW___(L"はひふへほ", L"*ほあ"));
    
    char* target = NULL;
    hkmalloc_and_cpy(&target, sizeof(char)*1, "");
    printf("target : %p\n", target);
    if(target[0] == '\0') {
        printf("true\n");
    }
    else {
        printf("false\n");
    }

    return 0;
}