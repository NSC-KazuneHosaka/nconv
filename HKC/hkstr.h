#ifndef HKSTR_H
#define HKSTR_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#define HKSTR_MAXSIZE 1024

int hkstrinit(void);

int hkstrGetError(void);

bool isHKstrNotError(void);

bool isNullEnd(const char* const str, size_t size);

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
int strmatch(const char* const target, const char* const str);
/* If match this returns not 0. */
/* If unmatch this returns 0. */
int strmatchW(const wchar_t* const target, const wchar_t* const str);

/*
If all matches or occures error this returns 1 or more.
If any one of them does not match this return 0.
Use isHKstrNotError() to see if an error occurred.
*/
int strmatches(const char* const target, const char** const strs, const unsigned int numOfStrs);
/*
If all matches or occures error this returns 1 or more.
If any one of them does not match this return 0.
Use isHKstrNotError() to see if an error occurred.
*/
int strmatchesW(const wchar_t* const target, wchar_t** strs, const unsigned int numOfStrs);

/* hkstrmatch, can use format designator. */
int strmatchf(const char* const target, const char* const format, ...);

/* hkstrmatchW, can use format defignator. */
int strmatchfW(const wchar_t* const target, const wchar_t* const _format, ...);

/* This is same to execute "strmatchf(target, "*%s*", str)". */
int contains(const char* const target, const char* const str);

/* This is same to execute "strmatchfW(target, L"*%ls*", str)". */
int containsW(const wchar_t* const target, const wchar_t* const str);

int containsList(const char* const target, const char** const strList, const unsigned int numOfStr);
int containsListW(const wchar_t* const target, const wchar_t** const strList, const unsigned int numOfStr);

#endif