#include "HKC/hklog.h"
#include "HKC/hkmem.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <errno.h>

// #define debug
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
    int end;
    int stage;
    int space;
    struct lineData* next;
    struct lineData* prev;
} linedata_t;

typedef struct linedataptr {
    linedata_t* ld;
    struct linedataptr* next;
    struct linedataptr* prev;
} ldp_t;


int openFile(FILE** fpp, const char* fileName, const char* openMode);
bool isNullEnd(const char* str, size_t maxsize);
int getCells(const char* source, size_t soureSize, char (*buffer)[MAX_SIZE], size_t bufSize[MAX_SIZE], const int numOfCells);
int extractInsideDQ(char* str);
double timetof(const char* time, size_t sizeOfTime);
double extRound(const double num, const int index);
int contains(const char* str, const char (*filterArr)[MAX_SIZE], const int numOfStr);
linedata_t* findWriteLine(linedata_t* ld);
int exclutionUnecFo(ldp_t* ldptr, linedata_t* lastnts);
bool isSameName(linedata_t* linedata1, linedata_t* linedata2);
bool isSameNameStr(linedata_t* linedata, const char* str);
bool isSameStage(linedata_t* ld1, linedata_t* ld2);
bool isSameTimeBefore(linedata_t* linedata);
bool isEnd(linedata_t* linedata);
int stagecmp(linedata_t* ld1, linedata_t* ld2);
linedata_t* threadSearch(const linedata_t* linedata, const char (*filter_in)[MAX_SIZE], const char (*));
int writelinedata(FILE* fpout, const linedata_t* linedata, const int maxstage);
void free_lineData(linedata_t* linedata);
void free_linedataptr(ldp_t* ldptr);


int main(int argc, char* argv[]) {
    char input[MAX_SIZE] = {0};
    char output[MAX_SIZE] = {0};
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

    int writtenBytes = 0;
    int flag = 0;
    int roop = 0;
    int i=0, j=0;

    time_t start_time, end_time;
    clock_t start_clock, end_clock;


    /* 入力ファイル名 */
    if (argc <= 1){
        printf("Input the csv file : ");
        fgets(input, sizeof(input), stdin);

        i=0;
        while(i<sizeof(input)){
            if(input[i] == '\n'){
                input[i] = '\0';
                break;
            }
            i++;
        }
    }
    else{
        strncpy(input, argv[1], sizeof(input));
        if(!isNullEnd(input, sizeof(input))) {
            printf("The input file is incorrect or name is too long.\n");
            return 1;
        }
    }

    start_clock = clock();
    start_time = time(NULL);

    /* ファイルオープン */
    if(openFile(&fpin, input, "r") != 0) {
        printf("openFile() error\n");
        return 1;
    }

    printf("opened file : %s\n", input);

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
        printf("The input is incorrect. Check the version of VisualVM (2.4?) and input file.\n");
        return 1;
    }
    printf("check low 1 : OK\n");


    /* 出力ファイルオープン */
    if(argc >= 3){
        strncpy(output, argv[2], sizeof(output));
        if(!isNullEnd(output, sizeof(output))) {
            printf("The output file is incorrect or name is too long.\n");
            printf("make output.csv\n");
            strncpy(output, "output.csv", sizeof(output));
        }
    }
    else {
        snprintf(output, sizeof(output)-1, "%s%s", "out_", input);
    }
    if(openFile(&fpout, output, "w") != 0) {
        printf("openFile() error\n");
        return 1;
    }

    printf("write to %s\n", output);

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

    /* ファイルへの出力ループ */
    linedata = ldstart;
    stage = 0;
    strncpy(filter_in[0], "nts.", sizeof(filter_in));
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
                    if(contains(ldptr->ld->name, filter_in, 1) || (isSameNameStr(ldptr->ld, filtered_out) && stagecmp(ldptr->prev->ld, ldptr->ld) < 0)) {                     
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

    /* ファイル出力 */
    ldptr = ldptrstart;
    while(ldptr != NULL) {
        writtenBytes += writelinedata(fpout, ldptr->ld, maxstage);
        ldptr = ldptr->next;
    }
    
    printf("convert was successful.\n");

    /* linedataのメモリ開放 */
    free_lineData(ldstart);
    free_linedataptr(ldptrstart);

    fclose(fpin);
    fclose(fpout);

    printf("Output was successfully written on %s (%d byte).\n", output, writtenBytes);
    end_time = time(NULL);
    end_clock = clock();
    printf("Total time: %ld ms\n", (end_time - start_time) * 1000);
    printf("CPU Time : %.1f ms\n", ((double)(end_clock - start_clock) / CLOCKS_PER_SEC) * 1000);
	return 0;
}



int openFile(FILE** fpp, const char* fileName, const char* openMode){
    int i=0;

    *fpp = fopen(fileName, openMode);

    if (fpp == NULL) {
        return 1;
    }

    return 0;
}

bool isNullEnd(const char* str, size_t maxsize) {
    int i=0;
    while (i<maxsize) {
        if(str[i] == '\0'){ return true; }
        i++;
    }

    return false;
    
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

double extRound(const double num, const int index) {
    if(num == 0) { return num; }
    double n = pow((double)10, (double)(-index));
    double m = num * n + 0.5;
    int o = (int)m;
    double p = (double)o / n;
    return p;
}

int contains(const char* str, const char (*filterArr)[MAX_SIZE], const int numOfStr) {
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
    char RepositoryImpl[] = "RepositoryImpl";
    char Repository[] = "Repository";
    char filtered_out[] = "Filtered out";
    char TypedQueryWrapper[] = "nts.hc.shr.infra.data.repository.query.TypedQueryWrapper";
    char ListMultiExecutor[] = "nts.hc.shr.infra.async.ListMultiExecutor";
    char SubjectContext[] = "nts.hc.ctx.cm.dom.careplan.common.SubjectContext";
    char self_time[] = "Self time";
    char zeroms[] = "0.0 ms";
    int flag=0;


    /* 末端 -> RepositoryImplを優先 */
    if(isEnd(ld)) {
        write = ld;
        strncpy(filter_in[0], RepositoryImpl, sizeof(filter_in[0]));
        strncpy(filter_out[0], TypedQueryWrapper, sizeof(filter_out[0]));
        strncpy(filter_out[1], ListMultiExecutor, sizeof(filter_out[1]));
        strncpy(filter_out[2], SubjectContext, sizeof(filter_out[2]));
        strncpy(filter_out[3], "$", sizeof(filter_out[3]));
        do {
            write = write->prev;
            if(contains(write->name, filter_in, 1) && !contains(write->name, filter_out, 4)){                            
                flag=1;
                break;
            }
        } while(isSameTimeBefore(write));

        /* $がついててもRepositoryImplがあればこちらを優先 */
        if(flag==0){
            write = ld;
            do {
                write = write->prev;
                if(contains(write->name, filter_in, 1) && !contains(write->name, filter_out, 3)){
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
                if(contains(write->name, filter_in, 1) && !contains(write->name, filter_out, 4)){
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
                if(contains(write->name, filter_in, 1) && !contains(write->name, filter_out, 3)){
                    flag=1;
                    break;
                }
            } while(isSameTimeBefore(write));
        }
    }

    /* $の無いメソッド・Filtered out以外 */
    if(flag==0){
        write = ld;
        strncpy(filter_out[0], filtered_out, sizeof(filter_out[0]));
        strncpy(filter_out[1], self_time, sizeof(filter_out[1]));
        strncpy(filter_out[2], "$", sizeof(filter_out[2]));
        do {
            write = write->prev;                        
            if(!contains(write->name, filter_out, 3)){
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
            if(!contains(write->name, filter_out, 2)){
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

int stagecmp(linedata_t* ld1, linedata_t* ld2) {
    if(ld1==NULL && ld2==NULL) { return 0;}
    else if(ld1==NULL) { return ld2->stage; }
    else if(ld2==NULL) { return -(ld1->stage); }
    return ld2->stage - ld1->stage;
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