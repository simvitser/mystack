#ifndef _LOG_H
#define _LOG_H

#define logError(message, ...) log("[ERROR] " message, ##__VA_ARGS__);

void logInit(const char *filename);

void logPuts(const char *message);

void log(const char *message, ...);

void logDestroy();

#endif
