#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>

#include "log.h"

FILE* GLOBAL_LOG_FILE = NULL;
const size_t MAX_TIME_SIZE = 100;










































void logInit(const char *filename) {
    assert(GLOBAL_LOG_FILE == NULL);

    if (GLOBAL_LOG_FILE != NULL) {
        logError("Already open log\n");
        return;
    }

    assert(filename);

    GLOBAL_LOG_FILE = fopen(filename, "w");
    
    assert(GLOBAL_LOG_FILE);
    
    setbuf(GLOBAL_LOG_FILE, NULL);
}

void logPuts(const char *message) {
    assert(GLOBAL_LOG_FILE);
    assert(message);

    log("%s\n", message);
}

void log(const char *message, ...) {
    assert(message);
    assert(GLOBAL_LOG_FILE);

    va_list ap = {};
    va_start(ap, message);

    time_t now = time(NULL);
    struct tm *tp = localtime(&now);
    
    char time_string[MAX_TIME_SIZE] = {};
    strftime(time_string, MAX_TIME_SIZE, "%d.%m.%Y %H:%M:%S", tp);
    
    fprintf(GLOBAL_LOG_FILE, "[%s] ", time_string);
    vfprintf(GLOBAL_LOG_FILE, message, ap);
    va_end(ap);
}

void logDestroy() {
    assert(GLOBAL_LOG_FILE);

    fclose(GLOBAL_LOG_FILE);
    GLOBAL_LOG_FILE = NULL;
}





// [OK]      stack init
// [INFO]    got it
// [ERROR]   FUCK
