#include "hkfile.h"
#include "../hkmem/hkmem.h"
#include "../hkstr/hkstr.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <direct.h>
#include <Windows.h>
#include <locale.h>

#define MAXSIZE HKFILE_MAXSIZE

/* enum of error */
enum hkfileerr {
    HKFILE_NO_ERROR = 0,
    INIT_FAILED = 401,
    INVALID_HANDLE_VAL,
    OUT_OF_ARR,
    HKMEM_ERROR,
    HKSTR_ERROR
};


static bool isInit = false;
static int hkfileerror = HKFILE_NO_ERROR;
static wchar_t** filter_in = NULL;
static wchar_t** filter_out = NULL;
static unsigned int numOfFilter_in = 0;
static unsigned int numOfFilter_out = 0;

static int error(int errorCode) {
    hkfileerror = errorCode;
    return hkfileerror;
}

int hkfileGetError(void) {
    return hkfileerror;
}

int hkfileShowErrors(FILE* stream) {
    fprintf(stream, "hkfile : %d\n", hkfileerror);
    fprintf(stream, "hkmem : %d\n", hkmemGetError());
    fprintf(stream, "hkstr : %d\n", hkstrGetError());
    return 0;
}

int hkfileinit(void) {
    if(hkmeminit() != 0) { return error(HKMEM_ERROR); }
    else if(hkstrinit() != 0) { return error(HKSTR_ERROR); }

    if(filter_in) { hkfree(&filter_in); }
    if(filter_out) { hkfree(&filter_out); }
    filter_in = NULL;
    filter_out = NULL;
    numOfFilter_in = 0;
    numOfFilter_out = 0;

    setlocale(LC_ALL, "japanese");
    isInit=true;
    return 0;
}

static int hkAddFilter(wchar_t*** filter, size_t* numOfFilter, const wchar_t* const str){
    int lengthOfStr = 0;
    int i=0;

    if(*filter == NULL) {
        if(hkmalloc(&(*filter), sizeof(wchar_t*)*MAXSIZE) != 0) {
            return error(HKMEM_ERROR);
        }
    }
    else { hkfree(&(*filter)); }

    while(str[i] != '\0') {
        lengthOfStr++;
        i++;
    }

    hkmalloc_and_cpyW(&(*filter)[*numOfFilter], str, lengthOfStr);
    *numOfFilter = *numOfFilter + 1;
    hkmalloc_and_cpyW(&(*filter)[*numOfFilter], L"", 1);

    return 0;
}

int hkAddFilter_in(const wchar_t* const str) {
    return hkAddFilter(&filter_in, &numOfFilter_in, str);
}

int hkAddFilter_out(const wchar_t* const str) {
    return hkAddFilter(&filter_out, &numOfFilter_out, str);
}


int hkmakedir(const char* _dirpath){
    char folderapth[100] = {0};
    int i=0;

    while(_dirpath[i] != '\0'){
        folderapth[i] = _dirpath[i];
        i++;
        if(i >= sizeof(folderapth)) {
            return error(OUT_OF_ARR);
        }
    }
    folderapth[i] = '\0';

    i=0;
    while(_mkdir(folderapth)!=0){
        i++;
        int j = strlen(folderapth);
        if(i>1){
            while(folderapth[j] != '_' && j >= 0) {
                folderapth[j] = '\0';
                j--;
            }
            snprintf(folderapth, sizeof(folderapth), "%s%d", folderapth, i);
        }
        else {
            snprintf(folderapth, sizeof(folderapth), "%s_%d", folderapth, i);
        }
        
    }
}


int hkshowdir(FILE* _stream, const wchar_t* _dirpath) {
    HANDLE hFind;
    WIN32_FIND_DATAW fd;
    wchar_t dirpath[MAXSIZE];
    wchar_t filename[MAXSIZE];

    wsprintfW(dirpath, L"%ls\\*.*", _dirpath);

    hFind = FindFirstFileW(dirpath, &fd);

    if(hFind == INVALID_HANDLE_VALUE) {
        FindClose(hFind);
        return error(INVALID_HANDLE_VAL);
    }

    do {
        fwprintf(_stream, L"%ls\n", fd.cFileName);
    } while(FindNextFileW(hFind, &fd));

    FindClose(hFind);
    
    return HKFILE_NO_ERROR;
}


static int _hkgetfilenamesWithFilter(wchar_t*** _filenames, const wchar_t* _dirpath, wchar_t** _filter_in, wchar_t** _filter_out) {
    HANDLE hFind;
    WIN32_FIND_DATAW fd;
    wchar_t dirpath[MAXSIZE] = {0};
    wchar_t** filenames = NULL;
    size_t sizeofFilenames = 0;
    int numOfFiles = 0;
    int i=0, j=0;


    wsprintfW(dirpath, L"%ls\\*.*", _dirpath);


    sizeofFilenames = sizeof(wchar_t*)*MAXSIZE;
    if(hkmalloc(&filenames, sizeofFilenames) != 0) {
        return error(HKMEM_ERROR);
    }

    hFind = FindFirstFileW(dirpath, &fd);
    
    if(hFind == INVALID_HANDLE_VALUE) {
        FindClose(hFind);
        return error(INVALID_HANDLE_VAL);
    }

    i=0;
    do {
        hkmalloc_and_cpyW(&filenames[i], fd.cFileName, (wcslen(fd.cFileName)));

        if(_filter_in == NULL && _filter_out == NULL) { numOfFiles++; }
        else {
            if(_filter_in != NULL && _filter_out != NULL) {
                if(hkstrmatchesW(filenames[i], _filter_in, numOfFilter_in) 
                && !hkstrmatchesW(filenames[i], _filter_out, numOfFilter_out)
                && isHKstrNotError()
                ) {
                    numOfFiles++;
                }
                else if(!isHKstrNotError()) { return error(HKSTR_ERROR); }
            }

            else if(_filter_in != NULL) {                
                if(hkstrmatchesW(filenames[i], _filter_in, numOfFilter_in) && isHKstrNotError()){ numOfFiles++; }
                else if(!isHKstrNotError()) { return error(HKSTR_ERROR); }
            }
            else if(_filter_out != NULL) {
                if(!hkstrmatchesW(filenames[i], _filter_out, numOfFilter_out) && isHKstrNotError()) { numOfFiles++; }
                else if(!isHKstrNotError()) { return error(HKSTR_ERROR); }
            }
        }
        i++;
    } while(FindNextFileW(hFind, &fd) && i < MAXSIZE);
    FindClose(hFind);

    if(hkmalloc(&(*_filenames), sizeof(wchar_t*) * (numOfFiles+1)) != 0) {
        return error(HKMEM_ERROR);
    }

    j=0;
    i=0;
    while(i<sizeofFilenames && j<numOfFiles) {
        if(_filter_in == NULL && _filter_out == NULL) {
            hkmalloc_and_cpyW(&(*_filenames)[j], filenames[i], wcslen(filenames[i]));
            j++;
        }
        else {
            if(_filter_in != NULL && _filter_out != NULL) {
                if(hkstrmatchesW(filenames[i], _filter_in, numOfFilter_in) 
                && !hkstrmatchesW(filenames[i], _filter_out, numOfFilter_out)
                && isHKstrNotError()
                ) {
                    hkmalloc_and_cpyW(&(*_filenames)[j], filenames[i], wcslen(filenames[i]));
                    j++;
                }
                else if(!isHKstrNotError()) { return error(HKSTR_ERROR); }
            }

            else if(_filter_in != NULL) {                
                if(hkstrmatchesW(filenames[i], _filter_in, numOfFilter_in) && isHKstrNotError()){
                    hkmalloc_and_cpyW(&(*_filenames)[j], filenames[i], wcslen(filenames[i]));
                    j++;
                }
                else if(!isHKstrNotError()) { return error(HKSTR_ERROR); }
            }
            else if(_filter_out != NULL) {
                if(!hkstrmatchesW(filenames[i], _filter_out, numOfFilter_out) && isHKstrNotError()) {
                    hkmalloc_and_cpyW(&(*_filenames)[j], filenames[i], wcslen(filenames[i]));
                    j++;
                }
                else if(!isHKstrNotError()) { return error(HKSTR_ERROR); }
            }
        }

        hkfree(&filenames[i]);
        i++;
    }

    hkmalloc_and_cpyW(&(*_filenames)[j], L"", 1);

    return HKFILE_NO_ERROR;
}

int hkgetfilenames(wchar_t*** _filenames, const wchar_t* _dirpath) {
    return _hkgetfilenamesWithFilter(_filenames, _dirpath, NULL, NULL);
}

int hkgetfilenamesWithFilter(wchar_t*** _filenames, const wchar_t* _dirpath) {
    return _hkgetfilenamesWithFilter(_filenames, _dirpath, filter_in, filter_out);
}