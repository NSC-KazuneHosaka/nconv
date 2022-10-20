#ifndef HKFILE_H
#define HKFILE_H

#include <stdio.h>

#define HKFILE_MAXSIZE 1024

int hkfileinit(void);

int hkfileGetError(void);

int hkfileShowErrors(FILE* stream);

int hkOpenFile(FILE** fpp, const char* fileName, const char* openMode);
int hkOpenFileW(FILE** fpp, const wchar_t* fileName, const wchar_t* openMode);

/* set str to filter_in */
/* Able to use ? and * as a wild-card */
/* This uses hkmalloc. */
int hkAddFilter_in(const wchar_t* const str);
/* set str to filter_out */
/* able to use ? and * as a wild-card */
/* This uses hkmalloc. */
int hkAddFilter_out(const wchar_t* const str);

/* delete from filter_in */
int hkDelFilter_in(const wchar_t* const str);
/* delete from filter_out */
int hkDelFilter_out(const wchar_t* const str);

/* make directory named _dirname */
int makedir(const char* _dirpath);

int makedir_(const char* _dirpath, char** madeDir);

int makedirW(const wchar_t* _dirpath);

int makedirW_(const wchar_t* _dirpath, wchar_t** _madeDir);

/* show all files in _dirpath to stream */
int showdir(FILE* stream, const wchar_t* _dirpath);
/* show filtered files in _dirpath to stream */
int showdirWithFilter(FILE* stream, const wchar_t* _dirpath);

/* get all filenames to _filenames in _dirpath
   This uses hkmalloc.
   "\0" will be set next to the last filename.
*/
int getfilenames(wchar_t*** _filenames, const wchar_t* _dirpath);
/* get filtered filenames to _filenames in _dirpath
   Memory in _filenames does not need to be allocated.
   This uses hkmalloc to allocate memory In that case. */
int getfilenamesWithFilter(wchar_t*** _filenames, const wchar_t* _dirpath);

#endif