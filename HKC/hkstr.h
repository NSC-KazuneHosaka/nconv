#ifndef HKSTR_H
#define HKSTR_H

#include "argsize.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#define HKSTR_MAXSIZE 1024

/* This make two dimentions char array. */
/* Last of str will be "\0". */
/* Please call by reference of pointer. */
/*  ex. make2dchar(&(char**), "str1", "str2", ...); */
#define make2dchar(ppptr, ...) _make2dchar(ppptr, VA_ARGS_SIZE(__VA_ARGS__), __VA_ARGS__)


int hkstrinit(void);

bool isHKstrNotError(void);

int hkstrGetError(void);

bool isNullEnd(const char* const str, size_t size);


/* This uses hkmalloc.
   Please call by reference of pointer
*  ex. hkmalloc(&(char*), size, str); */
int hkmalloc_and_cpy(char** target, size_t size, const char* const str);

/* hkmalloc_and_cpy(), using wchat_t. */
int hkmalloc_and_cpyW(wchar_t** target, const wchar_t* const str, size_t length);


/* This uses hkcalloc.
   Please call by reference of pointer
*  ex. hkmalloc(&(char*), want, size, str); */
int hkcalloc_and_cpy(char** target, size_t want, size_t size, const char* const str);

/* hkcalloc_and_cpy(), using wchar_t. */
int hkcalloc_and_cpyW(wchar_t** target, size_t want, size_t size, const wchar_t* const str);


/* This make two dimentions char array. */
/* Last of str will be "\0". */
/* Please call by reference of pointer. */
/*  ex. _make2dchar(&(char**), size, "str1", "str2", ...); */
int _make2dchar(char*** ppptr, size_t numOfStr, ...);

int free2dchar(char*** ppptr, const unsigned int numOfStr);

int free2dcharToNullc(char*** ppptr);


/* If match this returns not 0. */
/* If unmatch this returns 0. */
int strmatch(const char* const target, const char* const str);

/* strmatch(), using wchar_t. */
int strmatchW(const wchar_t* const target, const wchar_t* const str);

/* strmatch(), can use format designator. */
int strmatchf(const char* const target, const char* const format, ...);

/* strmatchW(), can use format defignator. */
int strmatchfW(const wchar_t* const target, const wchar_t* const _format, ...);

/*
If all matches or occures error this returns 1 or more.
If any one of them does not match this return 0.
Use isHKstrNotError() to see if an error occurred.
*/
int strmatches(const char* const target, const char** const strs, const unsigned int numOfStrs);

/* strmatches(), using wchar_t. */
int strmatchesW(const wchar_t* const target, wchar_t** strs, const unsigned int numOfStrs);


/* judge whether the string str is included in target. */
/* This is same to execute "strmatchf(target, "*%s*", str)". */
int contains(const char* const target, const char* const str);

/* contains(), using wchar_t. */
int containsW(const wchar_t* const target, const wchar_t* const str);

/* contains(), can use char** as list of str to strList. */
int containsList(const char* const target, char** const strList, const unsigned int numOfStr);

/* containsW(), can use char** as list of str to strList. */
int containsListW(const wchar_t* const target, wchar_t** const strList, const unsigned int numOfStr);

#endif