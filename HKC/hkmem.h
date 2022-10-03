#ifndef HKMEM_H
#define HKMEM_H

#include <stdio.h>

#define HKMEM_MAXSIZE 1024

int hkmeminit(void);

/* Please call by reference of pointer
*  ex. hkcalloc(&ptr, n, size); */
int hkcalloc(void* ptr, size_t want, size_t size);

/* Please call by reference of pointer
*  ex. hkmalloc(&ptr, size); */
int hkmalloc(void* ptr, size_t size);

/* Please call by reference of pointer
*  ex. hkrealloc(&ptr, size); */
int hkrealloc(void* ptr, size_t size);

/* Please call by reference of pointer
*  ex. hkfree(&ptr); */
int hkfree(void* ptr);


int hkmemreset(void);

#endif /* HKMEM_H */