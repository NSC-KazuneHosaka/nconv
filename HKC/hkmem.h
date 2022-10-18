#ifndef HKMEM_H
#define HKMEM_H

#include <stdio.h>

#define HKMEM_MAXSIZE 2048

int hkmeminit(void);

int hkmemGetError(void);

/* Please call by reference of pointer
*  ex. hkcalloc(&(void*)), n, size); */
int hkcalloc(void* ptr, size_t want, size_t size);

/* Please call by reference of pointer
*  ex. hkmalloc(&(void*), size); */
int hkmalloc(void* ptr, size_t size);

/* Please call by reference of pointer
*  ex. hkrealloc(&(void*)), size); */
int hkrealloc(void* ptr, size_t size);

/* Please call by reference of pointer
*  ex. hkfree(&(void*)); */
int hkfree(void* ptr);

/* record pointers of allocated memory until execute the function "hkmemrecend". */
int hkmemrecstart(void);

/* stop record pointers of allocated memory. */
int hkmemrecend(void);

/* get the last recorded pointers of allocated memory. */
int hkgetmemrec(void* ptr, size_t* ptrsize);


int hkmemreset(void);

#endif /* HKMEM_H */