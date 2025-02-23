#include <iostream>
#include "logger.h"

int main() {
    try {
        // Создаём логгер, используя C++‑класс
        Logger myLogger("test_output.log", LogLevel::INFO);

        // Сообщение DEBUG по умолчанию не пишется
        myLogger.logMessage(LogLevel::DEBUG, "");
        myLogger.logMessage(LogLevel::INFO,  "");
        myLogger.logMessage(LogLevel::ERROR, "");

        // Меняем уровень на DEBUG
        myLogger.setLogLevel(LogLevel::DEBUG);
        myLogger.logMessage(LogLevel::DEBUG, "");

        std::cout << "Лог записан в test_output.log" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
