#ifndef HKLOG_H
#define HKLOG_H

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
#define ALL 0x0fffffff
#define NORMAL ALL - INFO
#define NOLOGOUT 0

/* output : [minute:sec:millisec][log level] in <func> log */
#define hklog(fp, logLevel, log, ...) hkLog(fp, logLevel, __FUNCTION__, log, __LINE__, ##__VA_ARGS__)

int hkloginit(void);

/* output : [hour:minute:sec][log level] func(line) : log */
int hkLog(FILE* _fp, const unsigned int _LogLevel, const char* const _FuncName, const char* const _Log, const unsigned int _Line, ...);

int hklog_outputMode(const unsigned int mode_std, const unsigned int mode_file);
int hklog_outputMode_std(const unsigned int mode_std);
int hklog_outputMode_file(const unsigned int mode_file);

#endif /* HKLOG_H */