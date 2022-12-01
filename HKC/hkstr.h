#ifndef HKSTR_H
#define HKSTR_H

#include "argsize.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <limits.h>

#define HKSTR_MAXSIZE 1024
#define HKSTR_STRLIMIT INT_MAX

/* マクロ */

/* ++++++++++ メモリ確保系 ++++++++++ */

/* ppptrに二次元のchar型配列のメモリを動的に割り当て、第二引数以降の文字列を各行にそれぞれ格納する。 */
/* 最後の文字列が格納された後、'\0'が格納された配列を追加する。 */
/* 一文字のサイズは <sizeof(char)>。また各要素に割り当てられるメモリのサイズは <文字数+1> である。 */
/* 関数内でメモリを動的に確保するため、ppptrには参照渡しをする。 */
/*  例) make2dchar(&(char**), str1, str2, ...); */
#define make2dchar(ppptr, ...) _make2dchar(ppptr, VA_ARGS_SIZE(__VA_ARGS__), __VA_ARGS__)
/* ++++++++++++++++++++++++++++ */


/* ++++++++++ 操作系 ++++++++++ */

/* -----exclutionChar----- */
/* 第二引数以降に指定された文字を文字列strから取り除く。 */
/* 取り除いた文字数を返す。 */
#define exclutionChars(str, ...) _exclutionChars(str, VA_ARGS_SIZE(__VA_ARGS__), __VA_ARGS__)

/* ------------------------ */
/* +++++++++++++++++++++++++++ */


/* 関数 */

/* -----error----- */

int hkstrGetError(void);

bool isHKstrNotError(void);
/* --------------- */


/* -----init----- */

int hkstrinit(void);
/* -------------- */



/* ++++++++++ メモリ確保系 ++++++++++ */

/* -----alloc_and_cpy----- */

/* malloc()を使い<target>に<size>サイズのメモリを動的に割り当て、文字列<str>の要素をコピーする。 */
/* 関数内でメモリを動的に確保するため、targetには参照渡しをする。 */
/*  例) hkmalloc_and_cpy(&(char*), size, str); */
int hkmalloc_and_cpy(char** target, size_t size, const char* const str);

/* wchar_t型を使用する hkmalloc_and_cpy()。 */
int hkmalloc_and_cpyW(wchar_t** target, const wchar_t* const str, size_t length);

/* <target>に<size>サイズのメモリを動的に<want>個割り当て、文字列strの要素をコピーする。 */
/* 関数内でメモリを動的に確保するため、targetには参照渡しをする。 */
/*  例) hkmalloc_and_cpy(&(char*), want, size, str); */
int hkcalloc_and_cpy(char** target, size_t want, size_t size, const char* const str);

/* wchar_t型を使用する hkcalloc_and_cpy()。 */
int hkcalloc_and_cpyW(wchar_t** target, size_t want, size_t size, const wchar_t* const str);
/* ----------------------- */


/* -----2dchar----- */

/* ppptrに二次元のchar型配列のメモリを動的に割り当て、第二引数以降の文字列を各列にそれぞれ格納する。 */
/* 最後の文字列が格納された後、'\0'が格納された配列を追加する。 */
/* 一文字のサイズは <sizeof(char)>。また各要素に割り当てられるメモリのサイズは <文字数+1> である。 */
/* 関数内でメモリを動的に確保するため、ppptrには参照渡しをする。 */
/*  例) make2dchar(&(char**), numOfStr, str1, str2, ...); */
int _make2dchar(char*** ppptr, size_t numOfStr, ...);

/* 2次元文字配列ppptrのメモリを開放する。 */
/* numOfStrにはppprtに含まれる文字列の数(=列数)を渡す。 */
/* 関数内でメモリを動的に解放するため、ppptrには参照渡しをする。 */
/* 例) free2dchar(&(char**), numOfStr) */
int free2dchar(char*** ppptr, const size_t numOfStr);

int free2dcharToNullc(char*** ppptr);
/* ---------------- */
/* ++++++++++++++++++++++++++++ */



/* ++++++++++ 判定系 ++++++++++ */

/* -----isNullEndOrProcessable----- */

/* 文字列strがsize文字目までにNULL文字 ('\0') を含んでいるかを判定する。 */
/* NULL文字が見つかればその座標を返す。*/
/* 見つからなかった場合及び0文字目がNULL文字だった場合は0を返す。 */
int isNullEndOrProcessable(const char* const str, size_t size);
/* ------------------- */


/* -----endOfStr----- */

/* 文字列str中のNULL文字 ('\0') の位置を返す。 */
int endOfStr(const char* const str);
/* ------------------ */


/* -----strmatch----- */

/* 文字列<target>と<str>の内容が一致するかを判定する。 */
/* 一致した場合は0以外を、相違がある場合は0を返す。 */
/* エラーチェックにはisHKStrNotError()を使用する。 */
/* 例) if(strmatch(target, str) && isHKStrNotError()) {...} */
int strmatch(const char* const target, const char* const str);

/* wchar_t型を使用するstrmatch()。 */
int strmatchW(const wchar_t* const target, const wchar_t* const str);

/* フォーマット指定子を使用するstrmatch()。 */
int strmatchf(const char* const target, const char* const format, ...);

/* wchar_t型を使用するstrmatchf()。 */
int strmatchfW(const wchar_t* const target, const wchar_t* const _format, ...);

/* 文字列<target>が二次元文字列<strs>の各要素の文字列のいずれかと一致するかを判定する。 */
/* 一つでも一致する文字列があった場合は0以外を、一致する文字列が無かった場合は0を返す。 */
/* エラーチェックにはisHKStrNotError()を使用する。 */
/* 例) if(strmatches(target, strs, numOfStrs) && isHKStrNotError()) {...} */
int strmatches(const char* const target, const char** const strs, const unsigned int numOfStrs);

/* wchar_t型を使用するstrmatchs()。 */
int strmatchesW(const wchar_t* const target, wchar_t** strs, const unsigned int numOfStrs);
/* ------------------ */


/* -----contains----- */

/* 文字列<target>に文字列<str>が含まれているかを判定する。 */
/* 含まれていた場合は0以外を、含まれていなかった場合は0を返す。 */
/* この関数は strmatchf(target, "*%s*", str) を実行した時と同じ結果が返ってくる。 */
int contains(const char* const target, const char* const str);

/* wchar_t型を使用するcontains()。 */
int containsW(const wchar_t* const target, const wchar_t* const str);

/* 二次元文字配列<strList>の各行の文字列が<target>に含まれているかを判定する。 */
/* 一つでも含まれていた場合は0以外を、一つも含まれていなかった場合は0を返す。 */
/* この関数は strmatchesf(target, "*%s*", strList) を実行した時と同じ結果が返ってくる。 */
int containsList(const char* const target, char** const strList, const unsigned int numOfStr);

/* wchar_t型を使用するcontainsList()。 */
int containsListW(const wchar_t* const target, wchar_t** const strList, const unsigned int numOfStr);
/* ------------------ */
/* +++++++++++++++++++++++++ */




/* ++++++++++ 操作系 ++++++++++ */

/* -----exclutionChar----- */

/* 第三引数に指定された文字を、文字列strから取り除く。 */
/* 取り除いた文字数を返す。 */
int _exclutionChars(char* const str, size_t numOfChar, ...);
/* ------------------------ */
/* +++++++++++++++++++++++++++ */

#endif /* HKSTR_H */