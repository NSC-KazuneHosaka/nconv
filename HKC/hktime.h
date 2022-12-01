#ifndef HK_TIMEH
#define HK_TIMEH

#include <stdio.h>

#define HK_MAXSIZE 1024

typedef struct hktime {
    double time;
    double time_cpu;
} hktime_t;

int hktimeinit(void);

/* 開始時刻を取得する。 */
void hktime_start(void);

/* ラップタイムを取得する。 */
void hktime_lap(void);

/* 開始時刻からラップタイムまでの時間を秒単位で返す。 */
hktime_t getsec(void);

/* 開始時刻からラップタイムまでの時間をミリ秒単位で返す。 */
hktime_t getmillisec(void);

/* 開始時刻からラップタイムまでの時間をマイクロ秒単位で返す。 */
hktime_t getmicrosec(void);

#endif