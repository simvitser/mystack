#ifndef _LOG_H
#define _LOG_H

#define logError(message, ...) _log("ERROR", __FILE__, __func__, __LINE__, message, ##__VA_ARGS__);
#define    logOk(message, ...) _log("OK",    __FILE__, __func__, __LINE__, message, ##__VA_ARGS__);
#define      log(message, ...) _log("INFO",  __FILE__, __func__, __LINE__, message, ##__VA_ARGS__);

void logInit(const char *filename);

void logPuts(const char *message);

void _log(const char *prefix, const char *file, const char *func, const int line, const char *message, ...);


void logDestroy();

#endif
