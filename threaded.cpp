#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <sstream>
#include "logger.h"  // Подключаем нашу библиотеку (из Части 1)

using namespace std;

// Структура, описывающая задание на логирование
struct LogTask {
    LogLevel level;
    std::string message;
};

// Общая очередь задач и объекты синхронизации
std::queue<LogTask> taskQueue;
std::mutex queueMutex;
std::condition_variable cv;
std::atomic<bool> stopFlag(false); // Флаг, сигнализирующий о завершении

// Рабочий поток, который забирает задания из очереди и пишет их в лог
void loggingWorker(Logger* loggerPtr) {
    while (true) {
        std::unique_lock<std::mutex> lock(queueMutex);
        // Ждём, пока в очереди появится задание ИЛИ придёт сигнал завершения
        cv.wait(lock, [] {
            return !taskQueue.empty() || stopFlag.load();
        });

        // Если пришёл сигнал завершения и очередь пуста – завершаем поток
        if (stopFlag.load() && taskQueue.empty()) {
            break;
        }

        // Иначе берём очередную задачу из очереди
        LogTask task = taskQueue.front();
        taskQueue.pop();
        lock.unlock();

        // Пишем сообщение в лог (через библиотеку)
        loggerPtr->logMessage(task.level, task.message);
    }
}

int main(int argc, char* argv[]) {
    // Ожидаем 2 параметра:
    // 1) имя файла журнала
    // 2) уровень важности по умолчанию (DEBUG, INFO или ERROR)
    if (argc < 3) {
        cerr << "Использование: " << argv[0] << " <файл_лога> <уровень_по_умолчанию>\n";
        cerr << "Возможные уровни: DEBUG, INFO, ERROR\n";
        return 1;
    }

    std::string logFileName = argv[1];
    std::string defaultLevelStr = argv[2];

    // Преобразуем строковый уровень в enum
    LogLevel defaultLevel;
    if (defaultLevelStr == "DEBUG") {
        defaultLevel = LogLevel::DEBUG;
    } else if (defaultLevelStr == "INFO") {
        defaultLevel = LogLevel::INFO;
    } else if (defaultLevelStr == "ERROR") {
        defaultLevel = LogLevel::ERROR;
    } else {
        cerr << "Неизвестный уровень: " << defaultLevelStr << endl;
        return 1;
    }

    try {
        // Создаём логгер (из динамической библиотеки, Часть 1)
        Logger logger(logFileName, defaultLevel);

        // Запускаем рабочий поток
        std::thread workerThread(loggingWorker, &logger);

        // Основной цикл: считываем ввод пользователя
        cout << "Программа для записи сообщений в лог.\n";
        cout << "Вводите строки, при желании указывая уровень (DEBUG/INFO/ERROR) в начале.\n";
        cout << "Пример: 'INFO Это информационное сообщение'\n";
        cout << "Если уровень не указан, будет использован уровень по умолчанию: " 
             << defaultLevelStr << "\n";
        cout << "Для выхода введите 'exit'\n\n";

        while (true) {
            // Считываем строку
            std::string input;
            if (!std::getline(std::cin, input)) {
                // EOF или ошибка ввода
                break;
            }
            if (input.empty()) {
                continue; // Игнорируем пустые строки
            }

            // Проверяем, не "exit" ли это
            if (input == "exit") {
                // Ставим флаг завершения
                stopFlag.store(true);
                // Сигнализируем рабочему потоку
                cv.notify_all();
                break;
            }

            // Попытаемся разобрать уровень
            // Если строка начинается с "DEBUG ", "INFO " или "ERROR ",
            // используем этот уровень, иначе используем defaultLevel
            LogLevel levelToUse = defaultLevel;
            std::string actualMessage = input;

            // Разделяем строку на слова, смотрим первое
            {
                std::istringstream iss(input);
                std::string firstWord;
                iss >> firstWord;
                if (firstWord == "DEBUG" || firstWord == "INFO" || firstWord == "ERROR") {
                    if (firstWord == "DEBUG") levelToUse = LogLevel::DEBUG;
                    if (firstWord == "INFO")  levelToUse = LogLevel::INFO;
                    if (firstWord == "ERROR") levelToUse = LogLevel::ERROR;
                    // Берём остальную часть строки
                    std::string rest;
                    std::getline(iss, rest);
                    // Удаляем ведущие пробелы
                    if (!rest.empty() && rest[0] == ' ') {
                        rest.erase(0, 1);
                    }
                    actualMessage = rest;
                }
            }

            // Добавляем задачу в очередь
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                taskQueue.push(LogTask{levelToUse, actualMessage});
            }
            cv.notify_one();
        }

        // Ожидаем, пока рабочий поток завершится
        workerThread.join();
    }
    catch (const std::exception& e) {
        cerr << "Произошла ошибка: " << e.what() << endl;
        return 1;
    }

    cout << "Программа завершена.\n";
    return 0;
}
