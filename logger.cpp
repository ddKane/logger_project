#include "logger.h"
#include <iostream>
#include <stdexcept>

/* --------------------- Реализация C++ класса Logger --------------------- */

Logger::Logger(const std::string& logFileName, LogLevel defaultLevel)
    : currentLevel_(defaultLevel)
{
    // Открываем файл в режиме "append" + "out"
    logFile_.open(logFileName, std::ios::app | std::ios::out);
    if (!logFile_.is_open()) {
        throw std::runtime_error("Не удалось открыть файл журнала: " + logFileName);
    }
}

Logger::~Logger()
{
    if (logFile_.is_open()) {
        logFile_.close();
    }
}

void Logger::setLogLevel(LogLevel newLevel)
{
    std::lock_guard<std::mutex> lock(writeMutex_);
    currentLevel_ = newLevel;
}

void Logger::logMessage(LogLevel level, const std::string& message)
{
    if (level < currentLevel_) {
        return; // Не записываем, если уровень ниже текущего
    }
    std::lock_guard<std::mutex> lock(writeMutex_);

    // Формируем строку времени
    std::string timeStr = getCurrentTime();

    // Преобразуем уровень в человекочитаемый вид
    std::string levelStr;
    switch (level) {
        case LogLevel::DEBUG: levelStr = "DEBUG"; break;
        case LogLevel::INFO:  levelStr = "INFO";  break;
        case LogLevel::ERROR: levelStr = "ERROR"; break;
        default:              levelStr = "UNKNOWN";
    }

    // Пишем в файл журнала
    logFile_ << "[" << timeStr << "] "
             << "[" << levelStr << "]: "
             << message << std::endl;
	
}

std::string Logger::getCurrentTime()
{
    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    char buf[20];
    std::tm* tmStruct = std::localtime(&nowTime);
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tmStruct);
    return std::string(buf);
}

/* --------------------- Реализация C-интерфейса --------------------- */
#ifdef __cplusplus
extern "C" {
#endif

CLoggerHandle logger_create(const char* logFileName, int defaultLevel)
{
    if (!logFileName) {
        return nullptr;
    }
    try {
        Logger* logger = new Logger(
            std::string(logFileName),
            static_cast<LogLevel>(defaultLevel)
        );
        return reinterpret_cast<CLoggerHandle>(logger);
    }
    catch (const std::exception& e) {
        std::cerr << "[logger_create] Exception: " << e.what() << std::endl;
        return nullptr;
    }
}

void logger_destroy(CLoggerHandle handle)
{
    if (!handle) {
        return;
    }
    Logger* logger = reinterpret_cast<Logger*>(handle);
    delete logger;
}

void logger_set_level(CLoggerHandle handle, int newLevel)
{
    if (!handle) {
        return;
    }
    Logger* logger = reinterpret_cast<Logger*>(handle);
    logger->setLogLevel(static_cast<LogLevel>(newLevel));
}

void logger_write_message(CLoggerHandle handle, int level, const char* message)
{
    if (!handle || !message) {
        return;
    }
    Logger* logger = reinterpret_cast<Logger*>(handle);
    logger->logMessage(static_cast<LogLevel>(level), std::string(message));
}

#ifdef __cplusplus
}
#endif
