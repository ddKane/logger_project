#ifndef LOGGER_H
#define LOGGER_H

#ifdef _WIN32
   #ifdef LOGGER_EXPORTS
       #define LOGGER_API __declspec(dllexport)
   #else
       #define LOGGER_API __declspec(dllimport)
   #endif
#else
   #define LOGGER_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Уровни важности в стиле C (числовые константы)
#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_ERROR 2

// Непрозрачный тип "дескриптор" для C
typedef void* CLoggerHandle;

// Функции, доступные из C-кода
LOGGER_API CLoggerHandle logger_create(const char* logFileName, int defaultLevel);
LOGGER_API void logger_destroy(CLoggerHandle handle);
LOGGER_API void logger_set_level(CLoggerHandle handle, int newLevel);
LOGGER_API void logger_write_message(CLoggerHandle handle, int level, const char* message);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <ctime>

enum class LogLevel {
    DEBUG = 0,
    INFO  = 1,
    ERROR = 2
};

// Экспортируемый класс логгера
class LOGGER_API Logger {
public:
    Logger(const std::string& logFileName, LogLevel defaultLevel);
    ~Logger();

    void setLogLevel(LogLevel newLevel);
    void logMessage(LogLevel level, const std::string& message);

private:
    std::ofstream logFile_;
    LogLevel currentLevel_;
    std::mutex writeMutex_;

    std::string getCurrentTime();
};

#endif // __cplusplus

#endif // LOGGER_H
