#include "hkmem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int malloc__(void* ptr, size_t size) {
    void** pptr = malloc(size);
    printf("pptr = %p\n", pptr);
    *(void**)ptr = pptr;
    return 0;
}

/* malloc_(&ptr, size); */
int malloc_(void* ptr, size_t size) {
    return malloc__(ptr, size);
}


int main(void) {
    char** pptr = NULL;
    char* str = NULL;
    int size = 10;
    int strlen = 10;
    int ret = 0;
    int i=0;
    int lens[] = {5,3,7,1,2,5,8,5,3,2};

    hkmeminit();

    printf("pptr:%p\n", pptr);
    ret = hkmalloc(&pptr, sizeof(char*)*size);
    printf("pptr:%p, ret=%d\n", pptr, ret);

    while(i<size) {
        ret = hkmalloc(&pptr[i], sizeof(char)*(lens[i]+1));
        printf("pptr[%d]:%p, ret=%d\n", i, pptr[0], ret);
        i++;
    }
    pptr[0][0] = 'a';
    pptr[0][1] = '\0';
    printf("pptr[0] : %s\n", pptr[0]);

    ret = hkfree(&pptr);
    printf("freed pptr=%p, ret=%d\n", pptr, ret);

    return 0;
}