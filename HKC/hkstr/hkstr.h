#ifndef HKSTR_H
#define HKSTR_H

#include <stdio.h>

#define HKSTR_MAXSIZE 1024

int hkstrinit(void);

int hkstrGetError(void);

int isHKstrNotError(void);

/* This uses hkmalloc.
   Please call by reference of pointer
*  ex. hkmalloc(&(char*), size, str); */
int hkmalloc_and_cpy(char** target, size_t size, const char* const str);

/* This uses hkcalloc.
   Please call by reference of pointer
*  ex. hkmalloc(&(char*), want, size, str); */
int hkcalloc_and_cpy(char** target, size_t want, size_t size, const char* const str);

/* This uses hkmalloc.
   Please call by reference of pointer
   ex. hkmalloc(&(wchar_t*), size, str); */
int hkmalloc_and_cpyW(wchar_t** target, const wchar_t* const str, size_t length);

/* This uses hkcalloc.
   Please call by reference of pointer
   ex. hkmalloc(&(wchar_t*), want, size, str); */
int hkcalloc_and_cpyW(wchar_t** target, size_t want, size_t size, const wchar_t* const str);

/* If match this returns not 0. */
/* If unmatch this returns 0. */
int hkstrmatch(const char* const target, const char* const str);
/* If match this returns not 0. */
/* If unmatch this returns 0. */
int hkstrmatchW(const wchar_t* const target, const wchar_t* const str);

/*
If all matches or occures error this returns 1 or more.
If any one of them does not match this return 0.
Use isHKstrNotError() to see if an error occurred.
*/
int hkstrmatches(const char* const target, const char** const strs, const unsigned int numOfStrs);
/*
If all matches or occures error this returns 1 or more.
If any one of them does not match this return 0.
Use isHKstrNotError() to see if an error occurred.
*/
int hkstrmatchesW(const wchar_t* const target, wchar_t** strs, const unsigned int numOfStrs);

#endif