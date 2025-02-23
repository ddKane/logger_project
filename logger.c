#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

static FILE* logFile = NULL;
static enum LogLevel currentLogLevel = LOG_LEVEL_INFO;
static pthread_mutex_t logMutex = PTHREAD_MUTEX_INITIALIZER;

void initLog(const char* filename, enum LogLevel level) {
    pthread_mutex_lock(&logMutex);
    if (logFile) {
        fclose(logFile);
    }
    logFile = fopen(filename, "a");
    if (!logFile) {
        perror("Ошибка открытия файла");
        exit(EXIT_FAILURE);
    }
    currentLogLevel = level;
    pthread_mutex_unlock(&logMutex);
}

void setLogLevel(enum LogLevel level) {
    pthread_mutex_lock(&logMutex);
    currentLogLevel = level;
    pthread_mutex_unlock(&logMutex);
}

void logMessage(enum LogLevel level, const char* message) {
    pthread_mutex_lock(&logMutex);
    if (level > currentLogLevel || !logFile) {
        pthread_mutex_unlock(&logMutex);
        return;
    }

    time_t now = time(NULL);
    struct tm* localTime = localtime(&now);
    fprintf(logFile, "%04d-%02d-%02d %02d:%02d:%02d [%d] %s\n",
            localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
            localTime->tm_hour, localTime->tm_min, localTime->tm_sec,
            level, message);
    fflush(logFile);
    pthread_mutex_unlock(&logMutex);
}

void logShutdown() {
    pthread_mutex_lock(&logMutex);
    if (logFile) {
        fclose(logFile);
        logFile = NULL;
    }
    pthread_mutex_unlock(&logMutex);
}
