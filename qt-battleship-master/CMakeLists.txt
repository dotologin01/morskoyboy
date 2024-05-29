# Версия CMake
cmake_minimum_required(VERSION 3.5)

# Название проекта (решение в Visual Studio)
project(QtBattleship)

# Стандарт С/С++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Устанавливаем каталоги для выходных файлов
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)

# Установка линковки для QT библиотеки
# Линковку необходимо установить глобально, в противном случае у CMake будет какая-то хуйбалистика с путями (CMAKE_PREFIX_PATH)
set(QT_STATIC_LINK ON)

# Wrapper для работы с сетью
add_subdirectory("Sources/NetworkApi")

# Сервер
add_subdirectory("Sources/Server")

# Клиент (консольная версия)
add_subdirectory("Sources/ClientShell")

# Клиент (GUI)
add_subdirectory("Sources/Client")



