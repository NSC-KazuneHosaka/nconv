#include "hklog.h"
#include <stdio.h>

int main(void) {
	FILE* fp = fopen("output.txt", "w");

	hkloginit();
	hklog_outputMode(ALL-(INFO+FATAL), ALL);
	hklog(fp, INFO, "%d %s\n", 1, "INFO");
	hklog(fp, WARNING, "%d %s\n", 2, "WARNING");
	hklog(fp, ERR, "%d %s\n", 3, "ERROR");
	hklog(fp, FATAL, "%d %s\n", 4, "FATAL");
	hklog(fp, STOP, "%d %s\n", 5, "STOP");
	hklog(fp, 12345, "%d %s\n", 6, "12345");

	printf("///////////////////////\n");

	hklog_customFormat_std(TIME_HMS);
	hklog(fp, INFO, "%d %s\n", 1, "INFO");
	hklog_customFormat_std(TIME_HM);
	hklog(fp, WARNING, "%d %s\n", 2, "WARNING");
	hklog_customFormat_std(TIME_H);
	hklog(fp, ERR, "%d %s\n", 3, "ERROR");
	hklog_customFormat_std(NOTIME);
	hklog(fp, FATAL, "%d %s\n", 4, "FATAL");
	hklog_customFormat_std(NOLINE);
	hklog(fp, STOP, "%d %s\n", 5, "STOP");
	hklog_customFormat_std(NOFUNC);
	hklog(fp, 12345, "%d %s\n", 6, "12345");

	fclose(fp);
	return 0;
}