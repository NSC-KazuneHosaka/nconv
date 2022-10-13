/* include */
#include "hkstr.h"
#include "../hkmem/hkmem.h"
#include <stdbool.h>
#include <string.h>

/* define */
#define MAXSIZE HKSTR_MAXSIZE


/* enum of error */
enum hkstrerr {
    HKSTR_NO_ERROR = 0,
    NOT_INIT = 501,
    HKMEM_ERROR
};

/* grobal variavle */
static bool isInit = false;
static int hkstrerror = HKSTR_NO_ERROR;

/* func */
static int error(int errorCode) {
    hkstrerror = errorCode;
    return hkstrerror;
}

int hkstrGetError(void) {
    return hkstrerror;
}

int hkstrinit(void) {
    if(hkmeminit() != 0) { return error(HKMEM_ERROR); }
    isInit = true;
    return 0;
}

int isHKstrNotError(void) {
    return hkstrerror == HKSTR_NO_ERROR;
}


int hkmalloc_and_cpy(char** target, size_t size, const char* const str) {
    if(hkmalloc(&(*target), size) == 0) { strncpy(*target, str, size); }
    else { return error(HKMEM_ERROR); }
    return 0;
}

int hkcalloc_and_cpy(char** target, size_t want, size_t size, const char* const str) {
    if(hkcalloc(&(*target), want, size) == 0) { strncpy(*target, str, size); }
    else { return error(HKMEM_ERROR); }
    return 0;
}

int hkmalloc_and_cpyW(wchar_t** target, const wchar_t* const str, size_t length) {
    if(hkmalloc(&(*target), sizeof(wchar_t)*(length+1)) == 0) {
        wcsncpy(*target, str, length+1);
    }
    else { return error(HKMEM_ERROR); }
    return 0;
}

int hkcalloc_and_cpyW(wchar_t** target, size_t want, size_t size, const wchar_t* const str) {
    if(hkcalloc(&(*target), want, size) == 0) { wcsncpy(*target, str, size); }
    else { return error(HKMEM_ERROR); }
    return 0;
}



int hkstrmatch(const char* const target, const char* const str) {
    if(isInit == false) { return error(NOT_INIT); }
    
    const char* pt = target;
    const char* ps = str;

    /* target == "" && (ps == "***...." || ps == "") */
    if(*pt == '\0') {
        while(*ps == '*') { ps++; }
        return *ps == '\0';
    }
    /* target != "" && str == "" */
    else if(*ps == '\0') { return 0; }
    /* target != "" && *ps == '*' */
    else if(*ps == '*') {
        return *(ps+1) == '\0'  /* target != "" && str == "*" */
            || hkstrmatch(pt, ps+1)
            || hkstrmatch(pt+1, ps);
    }
    /* maches each other's character */
    else if(*ps == '?' || *pt == *ps) {
        pt++;
        ps++;
        return hkstrmatch(pt, ps);
    }

    return 0;
}

int hkstrmatchW(const wchar_t* const target, const wchar_t* const str) {
    if(isInit == false) { return error(NOT_INIT); }
    
    const wchar_t* pt = target;
    const wchar_t* ps = str;
    int ret = 0;

    /* target == "" && (ps == "***...." || ps == "") */
    if(*pt == L'\0') {        
        while(*ps == L'*') { ps++; }
        
        return *ps == L'\0';
    }
    /* target != "" && str == "" */
    else if(*ps == L'\0') { return 0; }
    /* target != "" && *ps == '*' */
    else if(*ps == L'*') {
        return *(ps+1) == L'\0'  /* target != "" && str == "*" */
            || hkstrmatchW(pt, ps+1)
            || hkstrmatchW(pt+1, ps);
    }
    /* maches each other's character */
    else if(*ps == L'?' || *pt == *ps) {
        pt++;
        ps++;
        return hkstrmatchW(pt, ps);
    }

    return ret;
}

int hkstrmatches(const char* const target, const char** const strs, const unsigned int numOfStrs) {
    int retnum = 0, ret = 0;
    int i=0;
    if(strs != NULL) {
        while(i<numOfStrs) {
            wprintf(L"%ls and %ls\n", target, strs[i]);
            ret == hkstrmatch(target, strs[i]);
            if(ret == 0 || ret == 1) {
                retnum += ret;
            }
            else { return error(ret); }
            i++;
        }
    }
    (void)error(HKSTR_NO_ERROR);
    return retnum;
}


int hkstrmatchesW(const wchar_t* const target, wchar_t** strs, const unsigned int numOfStrs) {
    int retnum = 0, ret = 0;
    int i=0;
    if(strs!=NULL) {
        while(i<numOfStrs) {
            ret = hkstrmatchW(target, strs[i]);
            if(ret == 0 || ret == 1) {
                retnum += ret;
            }
            else { return error(ret); }
            i++;
        }
    }
    (void)error(HKSTR_NO_ERROR);
    return retnum;
}
