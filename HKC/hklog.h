#ifndef _HKLOG_H
#define _HKLOG_H

#include <stdio.h>
#include <stdarg.h>

#define HKLOG_MAXSIZE 2048

/* Log levels */
#define INFO 0x00000001
#define WARNING 0x00000002
#define ERR 0x00000004
#define FATAL 0x00000008
#define STOP 0x00000010
#define UNKNOWN 0x00000020

#define HKLOG_LEVELS 6

/* Log output mode */
#define ALL 0x0000ffff
#define NORMAL ALL - INFO
#define NOLOGOUT 0

/* custom log format */

/* TIME */
#define NOTIME 0x00000010
#define TIME_HMSM 0b0001
#define TIME_HMS 0b0010
#define TIME_HM 0b0100
#define TIME_H 0b1000
/* LOGLEVEL */
#define NOLOGLEVEL 0x00000020
/* FUNC */
#define NOFUNC 0x00000040
#define NOLINE 0x00000080
/* NEWLINE */
#define NL_AUTO   0x00000100
#define NL_MANUAL 0x00000200
/* DEFAULT */
#define DEFAULT TIME_HMSM + NL_AUTO

/* output : [hour:minute:sec][log level] funcname(line) : log */
#define hklog(logLevel, log, ...) hkLog(logLevel, __FUNCTION__, log, __LINE__, ##__VA_ARGS__)

#define hklogW(logLevel, log, ...) hkLogW(logLevel, __FUNCTION__, log, __LINE__, ##__VA_ARGS__)


int hkloginit(void);

int hkSetLogfp(FILE* logfp);

/* output : [hour:minute:sec][log level] func(line) : log */
int hkLog(const unsigned int _LogLevel, const char* const _FuncName, const char* const _Log, const unsigned int _Line, ...);


/* hklog(), using wchar_t to _Log. */
int hkLogW(const unsigned int _LogLevel, const char* const _FuncName, const wchar_t* const _Log, const unsigned int _Line, ...);


/* ALL(default), INFO, WARNING, ERR, FATAL, STOP, NORMAL(other than NOMAL), NOLOGOUT */
void hklog_outputMode(const unsigned int _logMode_std, const unsigned int _logMode_logFile);

/* ALL(default), INFO, WARNING, ERR, FATAL, STOP, NORMAL(other than NOMAL), NOLOGOUT */
int hklog_outputMode_std(const unsigned int _logMode_std);

/* ALL(default), INFO, WARNING, ERR, FATAL, STOP, NORMAL(other than NOMAL), NOLOGOUT */
int hklog_outputMode_file(const unsigned int _logMode_logFile);

/*
TIME : TIME_HMSM(default), TIME_HMS, TIME_HM, TIME_H
LOGLEVEL : NOLOGLEVEL
FUNC : NOFUNC, NOLINE
NEWLINE : NL_AUTO(default), NL_MANUAL
set DEFAULT to reset
*/
int hklog_customFormat(const unsigned int _logFormat_logFile, const unsigned int _logFormat_std);

/*
TIME : TIME_HMSM(default), TIME_HMS, TIME_HM, TIME_H
LOGLEVEL : NOLOGLEVEL
FUNC : NOFUNC, NOLINE
NEWLINE : NL_AUTO(default), NL_MANUAL
set DEFAULT to reset
*/
int hklog_customFormat_file(const unsigned int _logFormat_logFile);

/*
TIME : TIME_HMSM(default), TIME_HMS, TIME_HM, TIME_H
LOGLEVEL : NOLOGLEVEL
FUNC : NOFUNC, NOLINE
NEWLINE : NL_AUTO(default), NL_MANUAL
set DEFAULT to reset
*/
int hklog_customFormat_std(const unsigned int _logFormat_std);

#endif /* _HKLOG_H */