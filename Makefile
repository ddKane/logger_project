# Компиляторы
CC = gcc
CXX = g++

# Общие флаги для C++ (и для линукса, и для windows), без -fPIC
CXXFLAGS_COMMON = -Wall -Wextra -std=c++17

# Файлы исходников
SRCS = logger.cpp
OBJS = logger.o

# Тестовое приложение (C++)
TEST_SRC = test.cpp
TEST_EXE_WIN = test_app.exe
TEST_EXE_LIN = test_app

# --------------------------
#       Цели сборки
# --------------------------

all: help

# --------------------------
# Сборка под Windows (MinGW)
# --------------------------
windows: clean $(OBJS)
	# Создаём DLL
	# -DLOGGER_EXPORTS => экспорт символов (dllexport) в logger.h
	$(CXX) $(CXXFLAGS_COMMON) -DLOGGER_EXPORTS -shared -o logger.dll $(OBJS) \
		-Wl,--out-implib,logger.lib \
		-Wl,--export-all-symbols

	# Сборка тестового приложения, линкуем с logger.lib
	$(CXX) $(CXXFLAGS_COMMON) $(TEST_SRC) -o $(TEST_EXE_WIN) \
		-L. -l:logger.lib

# --------------------------
# Сборка под Linux (Ubuntu)
# --------------------------
linux: clean $(OBJS)
	# Перекомпилируем с флагом -fPIC (если хотите делать PIC)
	$(CXX) $(CXXFLAGS_COMMON) -fPIC -c logger.cpp -o logger.o
	# Создаём so
	$(CXX) $(CXXFLAGS_COMMON) -shared -fPIC -o liblogger.so logger.o

	# Собираем тестовое приложение, линкуем с liblogger.so (через -L. -llogger)
	$(CXX) $(CXXFLAGS_COMMON) $(TEST_SRC) -o $(TEST_EXE_LIN) \
		-L. -llogger

# --------------------------
# Правило компиляции (без PIC). 
# Реальный PIC ставится в linux-правиле. 
# --------------------------
$(OBJS): $(SRCS) logger.h
	$(CXX) $(CXXFLAGS_COMMON) -c $(SRCS) -o $(OBJS)

clean:
	rm -f *.o *.dll *.lib *.so *.exe $(TEST_EXE_LIN)

help:
	@echo "Make targets:"
	@echo "  windows - сборка Windows (MinGW): logger.dll + test_app.exe"
	@echo "  linux   - сборка Linux (Ubuntu): liblogger.so + test_app"
	@echo "  clean   - очистка"
