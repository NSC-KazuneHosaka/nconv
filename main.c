#include "npsconverter.h"
#include "HKC/hkc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <time.h>

int main(void) {
    char logFileName[] = "a.log";
    FILE* logfp = NULL;
    wchar_t input[] = L"input.csv";
    wchar_t output[] = L"output.csv";
    char* outputdir = NULL;
    wchar_t* outputdirW = NULL;
    wchar_t* madedir = NULL;
    wchar_t** csvFiles = NULL;
    unsigned int writtenBytes = 0;
    int size = 0;
    int i=0;
    
    time_t timer;
    struct tm* date;
    time_t start_time, end_time;
    clock_t start_clock, end_clock;

    start_clock = clock();
    start_time = time(NULL);

    if(hkcinit() != 0) { 
        printf("hkcinit() error\n");
        return 1;
    }

    /* ログファイルオープン */
    if(hkOpenFile(&logfp, logFileName, "w") != 0) {
        printf("openFile() error : %s\n", logFileName);
        return 1;
    }
    hkSetLogfp(logfp);

    hklog(INFO, "start logging");


    /* 出力ファイルを格納するフォルダの作成 */
    /* 時刻を取得 */
    timer = time(NULL);
    date = localtime(&timer);
    /* フォルダ名を生成 */
    hkmalloc(&outputdir, (sizeof(char))*15);
    strftime(outputdir, 14, "%Y%m%d%H%M", date);
    /* フォルダ名をワイド文字列に変換 */
    size = sizeof(wchar_t)*(strlen(outputdir)+1000);
    hkmalloc(&outputdirW, size);
    mbstowcs(outputdirW, outputdir, size);
    /* フォルダを作成 */
    makedirW_(outputdirW, &madedir);
    hkfree(&outputdir);
    hkfree(&outputdirW);
    /* csvファイルの抽出 */
    hkAddFilter_in(L"*.csv");
    getfilenamesWithFilter(&csvFiles, L".");
    hklog_customFormat(NL_MANUAL, NL_MANUAL);
    hklogW(INFO, L"csv files: ");
    unsigned int enumFormat = NOTIME + NOLOGLEVEL + NOFUNC + NOLINE;
    hklog_customFormat(enumFormat, enumFormat);
    i=0;
    while(csvFiles[i][0] != L'\0') {
        if(i != 0) { hklog(INFO, "                                         "); } /* 41 spaces */
        hklogW(INFO, L"%ls\n", csvFiles[i]);
        i++;
    }
    if(i==0) { hklog(INFO, "not found\n"); }
    hklog_customFormat(DEFAULT, DEFAULT);

    /* npsconvert */
    i=0;
    while(csvFiles[i][0] != L'\0') {
        wchar_t* outputFile = NULL;
        size = sizeof(wchar_t)*(wcslen(madedir) + wcslen(L"\\") + wcslen(csvFiles[i]) + 1);
        hkmalloc(&outputFile, size);
        snwprintf(outputFile, size, L"%ls\\%ls", madedir, csvFiles[i]);
        writtenBytes += npsconvert(csvFiles[i], outputFile);
        i++;
    }
    
    hklog(INFO, "all convert was successfully end, written %u bytes", writtenBytes);

    hkmemreset();
    
    end_time = time(NULL);
    end_clock = clock();
    hklog(INFO, "Total time: %ld ms", (end_time - start_time) * 1000);
    hklog(INFO, "CPU Time : %.1f ms", ((double)(end_clock - start_clock) / CLOCKS_PER_SEC) * 1000);

    return 0;
}