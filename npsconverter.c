#include "npsconverter.h"
#include "HKC/hkc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <errno.h>

#define debug
// #define cmptest

#define NAME 0
#define TOTAL_TIME 1
#define TOTAL_TIME_CPU 2
#define HITS 3

#define COL_CSV 4 /* Name, Total Time, Total Time (CPU), Hits */

#define MAX_SIZE 1024


/* string filter */
#define NTS 0x1
#define SECTION 0x2
#define FIlTERD_OUT 0x3


typedef struct lineData {
    char* name;
    char* totaltime;
    char* totalTimePer;
    char* totaltimecpu;
    char* totalCpuPer;
    char* hits;
    int stage;  /* 出力するときのインデントの数 */
    int space; /* 入力のcsvでの空白の数 */
    int end;
    struct lineData* next;
    struct lineData* prev;
} linedata_t;

typedef struct linedataptr {
    linedata_t* ld;
    struct linedataptr* next;
    struct linedataptr* prev;
} ldp_t;

FILE* logfp = NULL;

void error(void) {
    if(logfp!=NULL) { fclose(logfp); }
}

int getCells(const char* source, size_t soureSize, char (*buffer)[MAX_SIZE], size_t bufSize[MAX_SIZE], const int numOfCells);
int extractInsideDQ(char* str);
double timetof(const char* time, size_t sizeOfTime);
double timepertof(const char* timeper, size_t sizeOfTimeper);
double extRound(const double num, const int index);
int contains_(const char* str, const char (*filterArr)[MAX_SIZE], const int numOfStr);
linedata_t* findWriteLine(linedata_t* ld);
int exclutionUnecFo(ldp_t* ldptr, linedata_t* lastnts);
bool isSameName(linedata_t* linedata1, linedata_t* linedata2);
bool isSameNameStr(linedata_t* linedata, const char* str);
bool isSameStage(linedata_t* ld1, linedata_t* ld2);
bool isSameTimeBefore(linedata_t* linedata);
bool isEnd(linedata_t* linedata);
int stagecmp(linedata_t* ld1, linedata_t* ld2);
linedata_t* threadSearch(const linedata_t* linedata, const char (*filter_in)[MAX_SIZE], const char (*));
int dcompforaddld(const double num1, const double num2, const int methodcnt);
int maketime(char* time, size_t sizeOfTime, double dtime);
int makeper(char* per, size_t sizeOfPer, double dper);
int writelinedata(FILE* fpout, const linedata_t* linedata, const int maxstage);
void free_lineData(linedata_t* linedata);
void free_linedataptr(ldp_t* ldptr);

unsigned int npsconvert(const wchar_t* const sourceFile, const wchar_t* const outputFile) {
    FILE* fpin = NULL;
    FILE* fpout = NULL;
    char linebuf[MAX_SIZE] = {0};
    char tokbuf[MAX_SIZE] = {0};
    char buf[COL_CSV][MAX_SIZE] = {0};
    size_t bufsizes[COL_CSV] = {0};
    linedata_t* linedata = NULL;
    linedata_t* ldstart = NULL;
    linedata_t* ldprev = NULL;
    ldp_t* ldptr = NULL;
    ldp_t* ldptrstart = NULL;
    ldp_t* ldptrprev = NULL;
    linedata_t* lastnts = NULL;
    double totaltime = 0;
    double timeprev = 0;
    double timecpu = 0;
    double maxtime = 0;
    double maxtimecpu = 0;
    char totalPer[MAX_SIZE] = {0};
    char totalCpuPer[MAX_SIZE] = {0};
    char filter_in[5][MAX_SIZE] = {0};
    char filter_out[5][MAX_SIZE] = {0};
    int stage = 0;
    int maxstage = 0;
    int space = 0;
    char tmp[MAX_SIZE] = {0};
    char RepositoryImpl[] = "RepositoryImpl";
    char filtered_out[] = "Filtered out";
    char self_time[] = "Self time";
    char zeroms[] = "0.0 ms";

    unsigned int writtenBytes = 0;
    int flag = 0;
    int roop = 0;
    int i=0, j=0;


    /* ファイルオープン */
    if(hkOpenFileW(&fpin, sourceFile, L"r") != 0) {
        printf("openFile() error");
        error();
        return 1;
    }

    hklogW(INFO, L"opened file : %ls", sourceFile);

    /* 一行目をカンマ区切りで取得 */
	fgets(tokbuf, sizeof(tokbuf), fpin);
    strncpy(buf[NAME], strtok(tokbuf, ","), sizeof(buf[NAME]));
    strncpy(buf[TOTAL_TIME], strtok(NULL, ","), sizeof(buf[TOTAL_TIME]));
    strncpy(buf[TOTAL_TIME_CPU], strtok(NULL, ","), sizeof(buf[TOTAL_TIME_CPU]));
    strncpy(buf[HITS], strtok(NULL, ","), sizeof(buf[HITS]));

    /* 一行目の確認 */
    if (strcmp(buf[NAME], "\"Name\"") != 0
     || strcmp(buf[TOTAL_TIME], "\"Total Time\"") != 0
     || strcmp(buf[TOTAL_TIME_CPU], "\"Total Time (CPU)\"") != 0
     || strcmp(buf[HITS], "\"Hits\"\n") != 0) {
        hklog(ERR, "The input is incorrect. Check the version of VisualVM (2.4?) and input file.\n");
        error();
        return 1;
    }
    hklog(INFO, "check low 1 : OK");


    /* 出力ファイルオープン */
    if(hkOpenFileW(&fpout, outputFile, L"w") != 0) {
        hklog(ERR, "openFile() error");
        error();
        return 1;
    }

    hklogW(INFO, L"opened file : %ls", outputFile);

    /* タスク名の行を消費 */
    fgets(linebuf, sizeof(linebuf), fpin);

    /* メインループ */
    while(fgets(linebuf, sizeof(linebuf), fpin)) {
        roop++;

        /* 文字列をそれぞれ取得 */
        i=0;
        while(i<COL_CSV){
            bufsizes[i] = sizeof(buf[i]);
            i++;
        }
        getCells(linebuf, sizeof(linebuf), buf, bufsizes, COL_CSV);
        

        /* それぞれのDQ(ダブルクォーテーション),空白を取り除く */
        space = extractInsideDQ(buf[NAME]); /* extractInsideDQの返り値は取り除いた空白の数 */
        extractInsideDQ(buf[TOTAL_TIME]);
        extractInsideDQ(buf[TOTAL_TIME_CPU]);
        extractInsideDQ(buf[HITS]);

        /* TOTALTIME２つの割合の記載を除外 どうせ(-0%)しかないので */
        i=0;
        while(buf[TOTAL_TIME][i] != '(') { i++; }
        if(buf[TOTAL_TIME][i-1] == ' ') { buf[TOTAL_TIME][i-1] = '\0'; }
        else { buf[TOTAL_TIME][i] = '\0'; }

        i=0;
        while(buf[TOTAL_TIME_CPU][i] != '(') { i++; }
        if(buf[TOTAL_TIME_CPU][i-1] == ' ') { buf[TOTAL_TIME_CPU][i-1] = '\0'; }
        else { buf[TOTAL_TIME_CPU][i] = '\0'; }

        timeprev = totaltime;
        /* 時間をdoubleに変換 */
        totaltime = timetof(buf[TOTAL_TIME], sizeof(buf[TOTAL_TIME]));
        timecpu = timetof(buf[TOTAL_TIME_CPU], sizeof(buf[TOTAL_TIME_CPU]));
        /* 100%時間を記録 */
        if(maxtime < totaltime) { maxtime = totaltime; }
        if(maxtimecpu < timecpu) { maxtimecpu = timecpu; }
        /* 時間の割合[%]を計算 */
        snprintf(totalPer, sizeof(totalPer) - 1, "%.2f%%", extRound((totaltime/maxtime) * 100, -2));
        snprintf(totalCpuPer, sizeof(totalCpuPer) - 1, "%.2f%%", extRound((timecpu/maxtimecpu) * 100, -2));


        /* 構造体のメモリ確保 */
        ldprev = linedata;
        linedata = (linedata_t*)malloc(sizeof(linedata_t));
        linedata->next = NULL;
        linedata->prev = ldprev;
        if (ldprev != NULL) { ldprev->next = linedata; }
        else { ldstart = linedata; } /* linedataのスタート地点を記録 */
        /* 文字列のメモリ確保 */ 
        linedata->name         = (char*)malloc(sizeof(char) * strlen(buf[NAME])+1);
        linedata->totaltime    = (char*)malloc(sizeof(char) * strlen(buf[TOTAL_TIME])+1);
        linedata->totalTimePer = (char*)malloc(sizeof(char) * strlen(totalPer)+1);
        linedata->totaltimecpu = (char*)malloc(sizeof(char) * strlen(buf[TOTAL_TIME_CPU])+1);
        linedata->totalCpuPer  = (char*)malloc(sizeof(char) * strlen(totalCpuPer)+1);
        linedata->hits         = (char*)malloc(sizeof(char) * strlen(buf[HITS])+1);
        /* 文字列のコピー */
        strncpy(linedata->name, buf[NAME], strlen(buf[NAME])+1);
        strncpy(linedata->totaltime, buf[TOTAL_TIME], strlen(buf[TOTAL_TIME])+1);
        strncpy(linedata->totalTimePer, totalPer, strlen(totalPer)+1);
        strncpy(linedata->totaltimecpu, buf[TOTAL_TIME_CPU], strlen(buf[TOTAL_TIME_CPU])+1);
        strncpy(linedata->totalCpuPer, totalCpuPer, strlen(totalCpuPer)+1);
        strncpy(linedata->hits, buf[HITS], strlen(buf[HITS])+1);
        linedata->space = space;
        linedata->end = 0;

        /* ldprev==NULL = スタート地点 */
        if(ldprev == NULL) {
            linedata->stage = stage;
        }
        /* 時間が同じ -> 同じstageとして扱う */
        else if(totaltime == timeprev) {
            linedata->stage = ldprev->stage;
        }
        /* 時間に変化アリ */
        /* 空白数増 -> stageを1増加 */
        else if(linedata->space > ldprev->space) {
            stage++;
            linedata->stage = stage;
        }
        /* 空白数同じ -> stageは前の行と同じになる　末尾のSelf timeがこのパターン */
        else if(linedata->space == ldprev->space) {
            linedata->stage = ldprev->stage;
        }
        /* 空白数減 ->　遡ってspaceが同じところを探す */
        /* stageはその行のstageと同じになる */
        else {
            linedata_t* tmp = linedata->prev;
            while(tmp->space != space && tmp->prev != NULL) {
                tmp = tmp->prev;
            }
            stage = tmp->stage;
            linedata->stage = tmp->stage;
        }

        if(stage > maxstage) { maxstage = stage; }
    }

    /* 出力する行の抽出 */
    linedata = ldstart;
    stage = 0;
    while(linedata != NULL) {
        if(linedata->prev != NULL){
            if((linedata->stage != stage || !isSameTimeBefore(linedata))) {
                /* 出力する行を特定 */
                linedata_t* write = findWriteLine(linedata);
                /* ファイル出力する行へのポインタを保存 */
                if(strcmp(write->totaltime, zeroms) != 0) {
                    /* ldptrのメモリ確保 */
                    ldptrprev = ldptr;
                    ldptr = (ldp_t*)malloc(sizeof(ldp_t));
                    /* ldptrに各ポインタを代入 */
                    ldptr->ld = write;
                    ldptr->next = NULL;
                    ldptr->prev = ldptrprev;
                    if(ldptr->prev != NULL) { ldptr->prev->next = ldptr; }
                    else { ldptrstart = ldptr; }
                    /* 不要なFiltered outの排除 */
                    if(ldptr->prev!=NULL){
                        if (ldptr->ld->stage <= ldptr->prev->ld->stage) { /* ld->prevの末端判定 */
                            ldp_t* tmp = ldptr->prev;
                            exclutionUnecFo(ldptr, lastnts);
                            /* RepositoryImplの関係で出力行の特定を再度実行する */
                            ldptr->prev->ld->next->end = 1;
                            ldptr->prev->ld = findWriteLine(ldptr->prev->ld->next); /* 元々保持していた行の次の行から遡行させる */
                        }
                    }
                    /* lastntsの更新 */
                    if(contains(ldptr->ld->name, "nts.") || (isSameNameStr(ldptr->ld, filtered_out) && stagecmp(ldptr->prev->ld, ldptr->ld) < 0)) {
                        lastnts = ldptr->ld;
                    }
                }
            }
        }
        stage = linedata->stage;
        linedata = linedata->next;
    }

    /* 一番最後がFiltered outだった時、不要であれば排除する */
    if(isSameNameStr(ldptr->ld, filtered_out)) {
        ldptrprev = ldptr;
        ldptr = (ldp_t*)malloc(sizeof(ldp_t));
        ldptr->next=NULL;
        ldptr->prev=ldptrprev;
        ldptr->prev->next=ldptr;
        ldptr->ld = (linedata_t*)malloc(sizeof(linedata_t));
        ldptr->ld->stage = 0;
        exclutionUnecFo(ldptr, lastnts);

        free(ldptr->ld);
        ldptr->prev->next=NULL;
        free(ldptr);
    }


    /* 分岐の前と分岐後の合計で実行時間が違わないかの確認 
       必要に応じてFiltered outを挿入*/
    ldptr = ldptrstart;
    while(ldptr->next != NULL) {
        /* 次の行のほうがステージが大きかった時 */
        if(ldptr->next->ld->stage > ldptr->ld->stage) {
            double ldptime = timetof(ldptr->ld->totaltime, strlen(ldptr->ld->totaltime) + 1);
            double ldptimeper = timetof(ldptr->ld->totalTimePer, strlen(ldptr->ld->totalTimePer) + 1);
            double ldptimecpu = timetof(ldptr->ld->totaltimecpu, strlen(ldptr->ld->totaltimecpu) + 1);
            double ldptimecpuper = timetof(ldptr->ld->totalCpuPer, strlen(ldptr->ld->totalCpuPer) + 1);
            ldp_t* tmp = ldptr->next;
            totaltime = 0;
            timecpu = 0;
            double totalTimePer = 0;
            double totalTimeCpuPer = 0;
            int methodcnt=0;
            /* 今の行+1のステージを持つメソッドの時間を合計する 
            　 stageが今の行以下になるまで続ける */
            //printf("tmp->stage:%d, ldptr->stage:%d\n", tmp->ld->stage, ldptr->ld-stage);
            while(tmp->ld->stage > ldptr->ld->stage) {
                if(tmp->ld->stage == ldptr->ld->stage + 1) {
                    totaltime += timetof(tmp->ld->totaltime, strlen(tmp->ld->totaltime)+1);
                    timecpu += timetof(tmp->ld->totaltimecpu, strlen(tmp->ld->totaltimecpu)+1);
                    methodcnt++;
                }
                tmp = tmp->next;
                if(tmp==NULL) { break; }
            }
            /* 計算が合わない場合はFiltered outを挿入して整合する */
            if(dcompforaddld(ldptime, totaltime, methodcnt) != 0
            //|| dcompforaddld(ldptimecpu, timecpu, methodcnt) != 0
            ) {                
                strncpy(buf[NAME], filtered_out, sizeof(buf[NAME]));
                double timediff = ldptime - totaltime;
                double timeperdiff = (timediff/maxtime) * 100;
                double cpudiff = ldptimecpu - timecpu;
                double cpuperdiff = (cpudiff/maxtimecpu) * 100;
                maketime(buf[TOTAL_TIME], sizeof(buf[TOTAL_TIME]), timediff);
                makeper(totalPer, sizeof(totalPer), timeperdiff);
                maketime(buf[TOTAL_TIME_CPU], sizeof(buf[TOTAL_TIME_CPU]), cpudiff);
                makeper(totalCpuPer, sizeof(totalCpuPer), cpuperdiff);
                snprintf(buf[HITS], sizeof(buf[HITS]), "0");

                linedata_t* addld = (linedata_t*)malloc(sizeof(linedata_t));
                /* 文字列のメモリ確保 */ 
                addld->name         = (char*)malloc(sizeof(char) * strlen(buf[NAME])+1);
                addld->totaltime    = (char*)malloc(sizeof(char) * strlen(buf[TOTAL_TIME])+1);
                addld->totalTimePer = (char*)malloc(sizeof(char) * strlen(totalPer)+1);
                addld->totaltimecpu = (char*)malloc(sizeof(char) * strlen(buf[TOTAL_TIME_CPU])+1);
                addld->totalCpuPer  = (char*)malloc(sizeof(char) * strlen(totalCpuPer)+1);
                addld->hits         = (char*)malloc(sizeof(char) * strlen(buf[HITS])+1);
                /* 文字列のコピー */
                strncpy(addld->name, buf[NAME], strlen(buf[NAME])+1);
                strncpy(addld->totaltime, buf[TOTAL_TIME], strlen(buf[TOTAL_TIME])+1);
                strncpy(addld->totalTimePer, totalPer, strlen(totalPer)+1);
                strncpy(addld->totaltimecpu, buf[TOTAL_TIME_CPU], strlen(buf[TOTAL_TIME_CPU])+1);
                strncpy(addld->totalCpuPer, totalCpuPer, strlen(totalCpuPer)+1);
                strncpy(addld->hits, buf[HITS], strlen(buf[HITS])+1);
                addld->stage = ldptr->next->ld->stage;
                addld->space = ldptr->next->ld->space;
                addld->end = 0;

                /* Filtered out を挿入するべき箇所を見つける */
                ldp_t* tmp = ldptr;
                while(tmp->next != NULL && tmp->next->ld->stage > ldptr->ld->stage) {
                    tmp = tmp->next;
                }
                /* Filtered outを挿入するのはtmpの次 */
                /* ldptrのメモリ確保 */
                ldptrprev = ldptr;
                ldptr = (ldp_t*)malloc(sizeof(ldp_t));
                ldptr->ld = addld;
                ldptr->next = tmp->next;
                if(ldptr->next != NULL) { ldptr->next->prev = ldptr; }
                ldptr->prev = tmp;
                ldptr->prev->next = ldptr;
                ldptr = ldptrprev;
            }
        }
        ldptr = ldptr->next;
    }

    /* ファイル出力 */
    ldptr = ldptrstart;
    while(ldptr != NULL) {
        writtenBytes += writelinedata(fpout, ldptr->ld, maxstage);
        ldptr = ldptr->next;
    }


    /* linedataのメモリ開放 */
    free_lineData(ldstart);
    free_linedataptr(ldptrstart);

    fclose(fpin);
    fclose(fpout);

    hklogW(INFO, L"Output was successfully written on %ls (%u byte).", fpout, writtenBytes);

	return writtenBytes;
}


/* 返り値：sourceからbufferに書き込まれた文字列の個数 */
int getCells(const char* source, size_t soureSize, char (*buffer)[MAX_SIZE], size_t bufSize[MAX_SIZE], const int numOfCells) {
    if(!isNullEnd(source, soureSize)) { return 1; }
    
    int i=0, j=0;
    int ret=0;

    /* ダブルクォーテーションで囲われた文字列をカンマ区切りで取得 */
    i=0;
    while(ret < numOfCells) {
        while(source[i] != '\"' && source[i] != '\0') { i++; }
        if(source[i] == '\0') { return ret; }
        
        buffer[ret][0] = '\"';
        i++;
        j=1;
        while(source[i] != '\"' && source[i] != '\0' && j+2 < bufSize[ret]) {
            buffer[ret][j] = source[i];
            j++;
            i++;
        }
        buffer[ret][j] = '\"';
        buffer[ret][j+1] = '\0';
        i++;
        ret++;
    }

    return ret;
}


int extractInsideDQ(char* str) {
    int i=1, j=0;
    int space = 0;
    while(str[i] == ' ') { i++; }
    space = i-1;
    while(str[i] != '"') {
        str[j] = str[i];
        j++;
        i++;
    }
    str[j] = '\0';
    return space;
}


double timetof(const char* time, size_t sizeOfTime) {
    if(!isNullEnd(time, sizeOfTime)) { return (double)0; }

    /* totaltime : <totaltime> ms */
    char* tmp = (char*)malloc(sizeof(char) * sizeOfTime);
    int i=0, j=0;
    while(time[i] != '\0' && time[i] != ' ' && i<sizeOfTime) {
        if(time[i] != ',') { tmp[j] = time[i]; j++; }
        i++;
    }
    tmp[j] = '\0';
    
    double ret = atof(tmp);
    free(tmp);
    
    return ret;
}

double timepertof(const char* timeper, size_t sizeOfTimeper) {
    if(!isNullEnd(timeper, sizeOfTimeper)) { return (double)0; }

    char* tmp = (char*)malloc(sizeof(char) * sizeOfTimeper);
    int i=0;
    while(timeper[i] != '%' && timeper[i] != '\n') {
        tmp[i] = timeper[i];
        i++;
    }
    tmp[i] = '\0';
    double ret = atof(tmp);
    free(tmp);

    return ret;
}

double extRound(const double num, const int index) {
    if(num == 0) { return num; }
    double n = pow((double)10, (double)(-index));
    double m = num * n + 0.5;
    int o = (int)m;
    double p = (double)o / n;
    return p;
}

int contains_(const char* str, const char (*filterArr)[MAX_SIZE], const int numOfStr) {
    int numOfFilterd = 0;
    int i=0;
    while(i<numOfStr){
        if(strstr(str, filterArr[i]) != NULL){ numOfFilterd++; }
        i++;
    }
    return numOfFilterd;
}

linedata_t* findWriteLine(linedata_t* ld){
    linedata_t* write = NULL;
    char filter_in[5][MAX_SIZE] = {0};
    char filter_out[5][MAX_SIZE] = {0};
    char** _filter_out = NULL;
    char RepositoryImpl[] = "RepositoryImpl";
    char Repository[] = "Repository";
    char filtered_out[] = "Filtered out";
    char TypedQueryWrapper[] = "nts.hc.shr.infra.data.repository.query.TypedQueryWrapper";
    char ListMultiExecutor[] = "nts.hc.shr.infra.async.ListMultiExecutor";
    char SubjectContext[] = "nts.hc.ctx.cm.dom.careplan.common.SubjectContext";
    char self_time[] = "Self time";
    char zeroms[] = "0.0 ms";
    int flag=0;


    /* RepositoryImplを優先 */
    write = ld;
    strncpy(filter_in[0], RepositoryImpl, sizeof(filter_in[0]));
    strncpy(filter_out[0], TypedQueryWrapper, sizeof(filter_out[0]));
    strncpy(filter_out[1], ListMultiExecutor, sizeof(filter_out[1]));
    strncpy(filter_out[2], SubjectContext, sizeof(filter_out[2]));
    strncpy(filter_out[3], "$", sizeof(filter_out[3]));
    make2dchar(&_filter_out, TypedQueryWrapper, ListMultiExecutor, SubjectContext, "$");
    do {
        write = write->prev;
        if(contains_(write->name, filter_in, 1) && !contains_(write->name, filter_out, 4)){
            flag=1;
            break;
        }
    } while(isSameTimeBefore(write));

    /* $がついててもRepositoryImplがあればこちらを優先 */
    if(flag==0){
        write = ld;
        do {
            write = write->prev;
            if(contains_(write->name, filter_in, 1) && !contains_(write->name, filter_out, 3)){
                flag=1;
                break;
            }
        } while(isSameTimeBefore(write));
    }
    
    /* Repository */
    if(flag==0){
        write = ld;
        strncpy(filter_in[0], Repository, sizeof(filter_in));
        do {
            write = write->prev;
            if(contains_(write->name, filter_in, 1) && !contains_(write->name, filter_out, 4)){
                flag=1;
                break;
            }
        } while(isSameTimeBefore(write));
    }

    /* Repository(セクション記号有り) */
    if(flag==0){
        write = ld;
        do {
            write = write->prev;
            if(contains_(write->name, filter_in, 1) && !contains_(write->name, filter_out, 3)){
                flag=1;
                break;
            }
        } while(isSameTimeBefore(write));
    }


    /* $の無いメソッド・Filtered out以外 */
    if(flag==0){
        write = ld;
        strncpy(filter_out[0], filtered_out, sizeof(filter_out[0]));
        strncpy(filter_out[1], self_time, sizeof(filter_out[1]));
        strncpy(filter_out[2], "$", sizeof(filter_out[2]));
        do {
            write = write->prev;                        
            if(!contains_(write->name, filter_out, 3)){
                flag=1;
                break;
            }
        } while(isSameTimeBefore(write));
    }

    /* $付きメソッド・Filtered out以外 */
    if(flag==0){
        write = ld;
        strncpy(filter_out[0], filtered_out, sizeof(filter_out[0]));
        strncpy(filter_out[1], self_time, sizeof(filter_out[1]));
        do {
            write = write->prev;
            if(!contains_(write->name, filter_out, 2)){
                flag=1;
                break;
            }
        } while(isSameTimeBefore(write));
    }

    /* Filtered outまたは時間付きSelf Time */
    if(flag==0){
        write=ld->prev;
    }

    return write;
}


int exclutionUnecFo(ldp_t* ldptr, linedata_t* lastnts){
    char filter_in[5][MAX_SIZE] = {0};
    char filter_out[5][MAX_SIZE] = {0};
    char filtered_out[] = "Filtered out";
    ldp_t* tmp = ldptr->prev;
    int flag=0;
    if(tmp==NULL){ return 0; }
    /* ラストntsまで遡ってFiltered outがあるか調査 */
    // printf("ldptr=%s %s, lastnts=%s %s\n", ldptr->ld->name, ldptr->ld->totaltime, lastnts->name, lastnts->totaltime);
    while(tmp->ld != lastnts){
        if(isSameNameStr(tmp->ld, filtered_out)){
            flag = 1;
            break;
        }
        tmp=tmp->prev;
    }
    /* Filtered outがあったとき */
    if(flag == 1) {
        tmp = ldptr->prev;
        while(tmp->ld != lastnts){
            /* ラストntsを上回るstageを持つ&&末端次を上回るstageを持つFiltered outを削除 */
            if(isSameNameStr(tmp->ld, filtered_out) /* メソッド名はFiltered outか？ */
            && tmp->ld->stage > lastnts->stage      /* stageはラストntsを上回るか？ */
            && tmp->ld->stage > ldptr->ld->stage) { /* stageは末端次を上回るか？ */
                tmp->prev->next = ldptr;
                ldptr->prev = tmp->prev;
            }
            tmp=tmp->prev;
        }
    }
    return 0;
}

bool isSameName(linedata_t* linedata1, linedata_t* linedata2) {
    if(linedata1==NULL && linedata2!=NULL) { return false;}
    else if(linedata1!=NULL && linedata2==NULL) { return false;}
    return strcmp(linedata1->name, linedata2->name) == 0;
}

bool isSameNameStr(linedata_t* linedata, const char* str) {
    if(linedata==NULL && str!=NULL) { return false;}
    else if(linedata!=NULL && str==NULL) { return false;}
    return strcmp(str, linedata->name) == 0;
}

bool isSameTimeBefore(linedata_t* linedata){
    if(linedata==NULL) { return false; }
    else if(linedata->prev==NULL) { return false; }
    return strcmp(linedata->totaltime, linedata->prev->totaltime) == 0;
}

bool isSameStage(linedata_t* ld1, linedata_t* ld2) {
    if(ld1==NULL && ld2!=NULL) { return false;}
    else if(ld1!=NULL && ld2==NULL) { return false;}
    return ld1->stage == ld2->stage;
}

bool isEnd(linedata_t* linedata) {
    return (linedata->space == linedata->prev->space || linedata->end == 1);
}

/* return ld2 - ld1 */
int stagecmp(linedata_t* ld1, linedata_t* ld2) {
    if(ld1==NULL && ld2==NULL) { return 0;}
    else if(ld1==NULL) { return ld2->stage; }
    else if(ld2==NULL) { return -(ld1->stage); }
    return ld2->stage - ld1->stage;
}

/* num1がnum2+-errorかを判定 num1が範囲内だったら0を返す */
int dcompforaddld(const double num1, const double num2, const int methodcnt) {
    double error = 0;
    if(fmod(num1, 1) == 0 || fmod(num2, 1) == 0) {
        error = 1 * methodcnt;
    }
    else {
        error = 0.1 * methodcnt;
    }
    if(num2 - error <= num1 && num1 <= num2 + error) { return 0; }
    else { return 1; }
}

int maketime(char* time, size_t sizeOfTime, double dtime) {
    snprintf(time, sizeOfTime, "%g", dtime);
    if(strlen(time)==1) { snprintf(time, sizeOfTime, "%s.00 ms", time); }
    else if(strlen(time)==2) { snprintf(time, sizeOfTime, "%s.0 ms", time); }
    else if(strlen(time)==3) { snprintf(time, sizeOfTime, "%s ms", time); }

    else if(!strchr(time, '.') && strlen(time) >= 4) {
        /* 桁数 */
        const int digits = strlen(time);
        /* カンマの数を求める */
        const int comma = (digits-1 / 3); /* (桁数-1)/3 余りは切り捨て */
        char tmp[digits+comma+3+1]; /* digits + comma + " ms" + '\0 */

        tmp[sizeof(tmp)-1] = '\0';
        int i=digits;
        int j=0; /* カンマの数記録 */
        while(i>=0) {
            tmp[i-j] = time[i];
            if((i-j)%4 == 1) {
                j++;
                tmp[i-j] = ',';
            }
            i--;
        }

        snprintf(time, sizeOfTime, "%s ms", tmp);
    }
    return strlen(time);
}

int isNumberc(char c){
    return ('0' <= c && c <= '9');
}

int makeper(char* per, size_t sizeOfPer, double dper) {
    snprintf(per, sizeOfPer, "%g%%", dper);
    return strlen(per);
}

/* 書き込む行：ひとつ前のstage内で最後に記録されたメソッド */
int writelinedata(FILE* fpout, const linedata_t* linedata, const int maxstage){
    int i=0;
    char indent[MAX_SIZE] = {0};
    char nameToTime[MAX_SIZE] = {0};
    int writtenChars=0;

    while(i<linedata->stage) {
        strncat(indent, ",", 2);
        i++;
    }

    i=linedata->stage;
    while(i<maxstage){
        strncat(nameToTime, ",", 2);
        i++;
    }

#ifndef cmptest
    writtenChars = fprintf(fpout, "%s%s,%s\"%s\",%s\n",
        indent, linedata->name, nameToTime, linedata->totaltime, linedata->totalTimePer);
#endif
#ifdef cmptest
    writtenChars = fprintf(fpout, "%s%s\n",indent, linedata->name);
#endif

    return writtenChars * sizeof(char);
}

void free_lineData(linedata_t* linedata) {
    while (linedata != NULL) {
        if(linedata->prev !=NULL) { free(linedata->prev); }
        if(linedata->name != NULL) { free(linedata->name); }
        if(linedata->totaltime != NULL) { free(linedata->totaltime); }
        if(linedata->totalTimePer != NULL) { free(linedata->totalTimePer); }
        if(linedata->totaltimecpu != NULL) { free(linedata->totaltimecpu); }
        if(linedata->totalCpuPer != NULL) { free(linedata->totalCpuPer); }
        if(linedata->hits != NULL) { free(linedata->hits); }
        linedata = linedata->next;
    }
}

void free_linedataptr(ldp_t* ldptr){
    while(ldptr!=NULL) {
        if(ldptr->prev != NULL) { free(ldptr->prev); }
        ldptr = ldptr->next;
    }
}
