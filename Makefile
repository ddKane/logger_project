# ---------------------------
# Универсальный Makefile
# ---------------------------

CC = gcc
CXX = g++

# Общие флаги для C++ (без -fPIC, т.к. под Windows это не нужно).
# Флаг -fPIC добавляем только для Linux.
CXXFLAGS_COMMON = -Wall -Wextra -std=c++17

# Файлы для библиотеки (Часть 1)
LIB_SOURCES = logger.cpp
LIB_OBJECTS = $(LIB_SOURCES:.cpp=.o)

# Файлы приложений
APP1_SOURCE = test.cpp          # Однопоточное (Часть 1)
APP2_SOURCE = threaded.cpp  # Многопоточное (Часть 2)

# Имена исполняемых файлов под Linux
APP1_TARGET_LIN = test_app
APP2_TARGET_LIN = threaded_app

# Имена исполняемых файлов под Windows
APP1_TARGET_WIN = test_app.exe
APP2_TARGET_WIN = threaded_app.exe

# Имена итоговых библиотек
LIB_TARGET_LIN = liblogger.so    # Под Linux
LIB_TARGET_WIN = logger.dll      # Под Windows
LIB_IMPORT_WIN = logger.lib      # Import library (Windows)

# ---------------------------
# Цель по умолчанию
# ---------------------------
all: help

# ---------------------------
# Сборка под Linux
# ---------------------------
linux: clean linux_build

linux_build:
	$(CXX) $(CXXFLAGS_COMMON) -fPIC -c logger.cpp -o logger.o
	$(CXX) $(CXXFLAGS_COMMON) -shared -fPIC -o $(LIB_TARGET_LIN) logger.o
	$(CXX) $(CXXFLAGS_COMMON) $(APP1_SOURCE) -o $(APP1_TARGET_LIN) -L. -llogger
	$(CXX) $(CXXFLAGS_COMMON) $(APP2_SOURCE) -o $(APP2_TARGET_LIN) -L. -llogger -lpthread

# ---------------------------
# Сборка под Windows (MinGW)
# ---------------------------
windows: clean windows_build

windows_build:
	$(CXX) $(CXXFLAGS_COMMON) -DLOGGER_EXPORTS -c logger.cpp -o logger.o
	$(CXX) $(CXXFLAGS_COMMON) -shared logger.o -o $(LIB_TARGET_WIN) -Wl,--out-implib,$(LIB_IMPORT_WIN) -Wl,--export-all-symbols
	$(CXX) $(CXXFLAGS_COMMON) $(APP1_SOURCE) -o $(APP1_TARGET_WIN) -L. -l:$(LIB_IMPORT_WIN)
	$(CXX) $(CXXFLAGS_COMMON) $(APP2_SOURCE) -o $(APP2_TARGET_WIN) -L. -l:$(LIB_IMPORT_WIN)

# ---------------------------
# Очистка
# ---------------------------
clean:
	rm -f *.o $(LIB_TARGET_LIN) $(LIB_TARGET_WIN) $(LIB_IMPORT_WIN) $(APP1_TARGET_LIN) $(APP2_TARGET_LIN) $(APP1_TARGET_WIN) $(APP2_TARGET_WIN)

# ---------------------------
# Справка
# ---------------------------
help:
	@echo "Доступные цели:"
	@echo "  linux    - сборка под Linux (создаёт liblogger.so, test_app, threaded_app)"
	@echo "  windows  - сборка под Windows (MinGW) (создаёт logger.dll, logger.lib, test_app.exe, threaded_app.exe)"
	@echo "  clean    - удаление временных и результатирующих файлов"
	@echo "  all      - (по умолчанию) показывает это сообщение"
