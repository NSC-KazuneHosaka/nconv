#include "hkmem.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// #define debug

/* enum of error */
enum hkmemerr {
    HKMEM_NO_ERROR = 0,
    NOT_INIT = 201,
    INDEX_PTR_ISNULL,
    FULL_OF_AP,
    EMPTY_OF_AP,
    OUT_OF_APARR,
    UNKNOWN_ALLOCMODE,
    MALLOC_RET_NULL,
    CALLOC_RET_NULL,
    REALLOC_RET_NULL
};

#define APSIZE HKMEM_MAXSIZE

static bool isInit = false;
static int hkmemerror = HKMEM_NO_ERROR;
/* use APSIZE as size of ALLOCATED_PTRS */
static void* ALLOCATED_PTRS[APSIZE];
static int numOfAP = 0;
static bool recording = false;

static int seachNullInAP(int* index);

static int searchPtrInAP(void* ptr, int* index);

static int error(int errorCode) {
    hkmemerror = errorCode;
    return hkmemerror;
}

int hkmemGetError(void) {
    return hkmemerror;
}

int hkmeminit(void) {
    if(isInit == false) {
        int i=0;
        while(i<APSIZE) {
            ALLOCATED_PTRS[i] = NULL;
            i++;
        }
        isInit = true;
    }
    return 0;
}

/* 
hkalloc
allocmode:
calloc  : "calloc" or "c"
malloc  : "malloc" or "m"
realloc : "realloc" or "r"
*/
static int hkalloc(void* ptr, const char* allocmode, size_t size, size_t want) {
    void** pptr = NULL;
    int i=0;

    if(isInit == false) { return error(NOT_INIT); }
    if(numOfAP == APSIZE) { return error(FULL_OF_AP); }

    while(ALLOCATED_PTRS[i] != NULL && i<APSIZE) {
        i++;
        if(i > APSIZE) { return error(OUT_OF_APARR); }
    }

    switch (allocmode[0]) {
        case 'c':
            pptr = (void**)calloc(want, size);
            if(pptr == NULL) { return error(CALLOC_RET_NULL); }
            break;

        case 'm':
            pptr = (void**)malloc(size);            
            if(pptr == NULL) { return error(MALLOC_RET_NULL); }
            break;

        // case 'r':
        //     realloc;
        //     break;

        default:
            return error(UNKNOWN_ALLOCMODE);
    }
    *(void**)ptr = pptr;
    ALLOCATED_PTRS[i] = pptr;
    numOfAP++;

#ifdef debug
printf("alloced:%p, size:%d\n", ALLOCATED_PTRS[i], size);
#endif    

    return 0;
}

int hkcalloc(void* ptr, size_t want, size_t size) {
    return hkalloc(ptr, "calloc", size, want);
}

int hkmalloc(void* ptr, size_t size) {
    return hkalloc(ptr, "malloc", size, 0);
}


int hkfree(void* ptr) {
    if(isInit == false) { return error(NOT_INIT); }
    if(ptr == NULL) { return HKMEM_NO_ERROR; }
    if(numOfAP == 0) { return error(EMPTY_OF_AP); }

    int i=0;
    while(ALLOCATED_PTRS[i] != *(void**)ptr && i<APSIZE) {
        i++;
        if(i > APSIZE) { return error(OUT_OF_APARR); }
    }

    if(i<APSIZE) {        
        free(ALLOCATED_PTRS[i]);
        numOfAP--;
        ALLOCATED_PTRS[i] = NULL;
        *(void**)ptr = ALLOCATED_PTRS[i];
    }
    else {
        free(ptr);
        *(void**)ptr = NULL;
    }
    

    return 0;
}


int hkmemrecstart(void) {
    recording = true;
    return 0;
}

int hkmemrecend(void) {
    recording = false;
    return 0;
}

int hkgetmemrec(void* ptr, size_t* ptrsize) {
    
}


int hkmemreset(void) {
    if(isInit == false) { return error(NOT_INIT); }
    if(numOfAP == 0) { return error(EMPTY_OF_AP); }

    void* ptr = NULL;
    int i=0;
    while(i<APSIZE && numOfAP > 0) {
        ptr = ALLOCATED_PTRS[i];
        if(ptr != NULL) { hkfree(ptr); }
        i++;
        if(i > APSIZE) { return error(OUT_OF_APARR); }
    }
    
    isInit = false;
    return hkmeminit();
}
