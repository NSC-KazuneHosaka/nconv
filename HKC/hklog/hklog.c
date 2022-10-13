#include "hklog.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

#define MAX_SIZE HKLOG_MAXSIZE

#define NOT_ERROR INFO + STOP

typedef struct hklogFuncCall {
	unsigned int hkloginit;
	unsigned int hklog_outputMode;
	unsigned int hklog;
} hklogFuncCall_t;

/* enum of error */
enum hklogerr {
	NO_ERROR = 0,
	NOT_INIT = 301,
};

bool isInit = false;
bool andstd = false;
int hklogerror = NO_ERROR;
static unsigned int logMode_logFile = ALL;
static unsigned int logMode_std = ALL;
static unsigned int logFormat_logFile = ALL;
static unsigned int logFormat_std = ALL;
static int _hkLog(FILE* _fp, const unsigned int _LogLevel, const char* const _FuncName, const char* const _Log, const unsigned int _Line);
static int makeLogSentence(char* const _logSentence, const char* const _logMode, size_t max_size, const struct timeval const _now, const unsigned int _LogLevel, const char* const _FuncName, const unsigned int _Line, const char* const _Log);

int hkloginit(void) {
	if(isInit == false) {
		isInit = true;
	}
	return 0;
}

int hklog_outputMode_file(unsigned int _logMode_logFile) {
	logMode_logFile = _logMode_logFile;
	return logMode_logFile;
}

int hklog_outputMode_std(unsigned int _logMode_std) {
	logMode_std = _logMode_std;
	return logMode_std;
}

void hklog_outputMode(unsigned int _logMode_logFile, unsigned int _logMode_std) {
	hklog_outputMode_file(_logMode_logFile);
	hklog_outputMode_std(_logMode_std);
}

int hklog_customFormat_file(const unsigned int _logFormat_logFile) {
	logFormat_logFile = _logFormat_logFile;
	return logFormat_logFile;
}

int hklog_customFormat_std(const unsigned int _logFormat_std) {
	logFormat_std = _logFormat_std;
	return logFormat_std;
}

int hklog_customFormat(const unsigned int _logFormat_logFile, const unsigned int _logFormat_std) {
	hklog_customFormat_file(_logFormat_logFile);
	hklog_customFormat_std(_logFormat_std);
}

int hkLog(FILE* _fp, const unsigned int _LogLevel, const char* const _FuncName, const char* const _Log, const unsigned int _Line, ...) {
	const unsigned int logDefs[HKLOG_LEVELS - 1] = { INFO, WARNING, ERR, FATAL, STOP};
	char logSentence[MAX_SIZE] = {0};
	int level = UNKNOWN;
	va_list args;
	int i = 0;

	while (i < sizeof(logDefs)) {
		if (logDefs[i] == _LogLevel) {
			level = logDefs[i];
			break;
		}
		i++;
	}
	if (i >= sizeof(logDefs)/sizeof(logDefs[0])) { level = UNKNOWN; }
	if (_fp != NULL && (logMode_std & level != 0 || logMode_logFile & level != 0)) {
		va_start(args, _Line);
		vsnprintf(logSentence, sizeof(logSentence), _Log, args);
		va_end(args);

		if (errno != 0 && level != INFO) {
			snprintf(logSentence, sizeof(logSentence) - 1, "%s, strerror : %s", logSentence, strerror(errno));
		}

		_hkLog(_fp, level, _FuncName, logSentence, _Line);
		fflush(_fp);
	}

	return NO_ERROR;
}


static int _hkLog(FILE* _fp, const unsigned int _LogLevel, const char* const _FuncName, const char* const _Log, const unsigned int _Line) {
	char output[MAX_SIZE] = { 0 };
	struct timeval now;

	gettimeofday(&now, NULL);
	
	if ((logMode_logFile & _LogLevel) != 0 && _fp != NULL) {
		makeLogSentence(output, "f", sizeof(output), now, _LogLevel, _FuncName, _Line, _Log);
		fprintf(_fp, output);
	}

	if ((logMode_std & _LogLevel) != 0) {
		FILE* stream = NULL;
		if (_LogLevel & NOT_ERROR != 0) { stream = stdout; }
		else { stream = stderr; }

		makeLogSentence(output, "s", sizeof(output), now, _LogLevel, _FuncName, _Line, _Log);
		fprintf(stream, output);
	}
	
	return 0;
}


/* mode  f:file, s:std*/
static int makeLogSentence(char* const _logSentence, const char* const _logMode, size_t max_size, const struct timeval const _now, const unsigned int _LogLevel, const char* const _FuncName, const unsigned int _Line, const char* const _Log) {
	unsigned int logMode = 0;
	unsigned int customLogFormat = 0;
	
	_logSentence[0] = '\0';

	switch (_logMode[0]) {
		case 'f':
			logMode = logMode_logFile;
			customLogFormat = logFormat_logFile;
			break;

		default:
			logMode = logMode_std;
			customLogFormat = logFormat_std;
	}

	/* TIME */
	if(customLogFormat != NOTIME) {
		char time[13] = {0};
		struct tm* ltm = NULL;

		ltm = localtime(&_now.tv_sec);

		if((customLogFormat & TIME_HMSM) != 0) {
			snprintf(time, sizeof(time), "%02d:%02d:%02d.%03d", ltm->tm_hour, ltm->tm_min, ltm->tm_sec, _now.tv_usec);
		}
		else if(customLogFormat == TIME_HMS) {
			snprintf(time, sizeof(time), "%02d:%02d:%02d", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
		}
		else if(customLogFormat == TIME_HM) {
			snprintf(time, sizeof(time), "%02d:%02d", ltm->tm_hour, ltm->tm_min);
		}
		else if(TIME_H) {
			snprintf(time, sizeof(time), "%02d", ltm->tm_hour);
		}

		snprintf(_logSentence, max_size, "%s ", time);
	}

	/* LOG LEVEL */
	if(customLogFormat != NOLOGLEVEL) {
		const char logLevels[HKLOG_LEVELS][8] = { "INFO", "WARNING", "ERR", "FATAL", "STOP", "UNKNOWN"};
		int logLevel = _LogLevel;
		if(logLevel <= HKLOG_LEVELS) { logLevel = UNKNOWN; }
		
		int N = (int)(log(_LogLevel) / log(2) + 0.5); /* logLevels array index */
		snprintf(_logSentence, max_size, "%s[%s] ", _logSentence, logLevels[N]);
	}

	/* FUNC */
	if(customLogFormat != NOFUNC) {
		char func[MAX_SIZE] = {0};
		snprintf(func, sizeof(func), "%s", _FuncName);

		if(customLogFormat != NOLINE) {
			snprintf(func, sizeof(func), "%s(%d)", func, _Line);
		}

		snprintf(_logSentence, max_size, "%s%s : ", _logSentence, func);
	}

	/* LOG */
	snprintf(_logSentence, max_size, "%s%s", _logSentence, _Log);

	return strlen(_logSentence);
}
