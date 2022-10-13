/* include */
#include "template.h"
#include <stdbool.h>

/* define */
#define MAXSIZE HK_MAXSIZE


/* enum of error */
static enum hkerr {
    HK_NO_ERROR = 0,
};

/* grobal variavle */
static bool isInit = false;
static int hkerror = HK_NO_ERROR;

/* func */
static int error(int errorCode) {
    hkerror = errorCode;
    return hkerror;
}

int hkmeminit(void) {
    isInit = true;
    return 0;
}
