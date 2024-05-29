# Версия CMake
cmake_minimum_required(VERSION 3.5)

# Определить разрядность платформы
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    set(PLATFORM_BIT_SUFFIX "x86")
else()
    set(PLATFORM_BIT_SUFFIX "x64")
endif()

# Для того чтобы была возможность включать файлы созданные генератором QT расположенные в CMAKE промежуточном каталоге
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Название цели сборки
set(TARGET_NAME "BattleShipClient")
set(TARGET_BIN_NAME "BattleShipClient")

# Пути к библиотеке QT для различных компиляторов и платформ
include("../../QtDir.cmake")

# Поиск пакета QT средствами CMAKE
set(CMAKE_PREFIX_PATH "${QT5_DIR}")
find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

# Включение генераторов QT
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(MSVC)
    add_compile_options("/utf-8")
endif()

# Добавляем .exe (проект в Visual Studio)
add_executable(${TARGET_NAME} WIN32
        "Main.cpp"
        "SettingsWindow.h" "SettingsWindow.cpp" "SettingsWindow.ui"
        "GameStartWindow.h" "GameStartWindow.cpp" "GameStartWindow.ui"
        "GameWindow.cpp" "GameWindow.h"
        "GameField.cpp" "GameField.h")

# Меняем название запускаемого файла в зависимости от типа сборки
set_property(TARGET ${TARGET_NAME} PROPERTY OUTPUT_NAME "${TARGET_BIN_NAME}$<$<CONFIG:Debug>:_Debug>_${PLATFORM_BIT_SUFFIX}")

# Если это статическая линковка - объявить символ QT_STATIC_BUILD (может понадобиться в исходном коде)
if(QT_STATIC_LINK)
    target_compile_definitions(${TARGET_NAME} PUBLIC QT_STATIC_BUILD)
endif()


# Дополнительные действия
if(WIN32)
    # Если это не статическая линковка - необходимо скопировать DLL в папку с исполняемым файлом
    if(NOT QT_STATIC_LINK)
        include("../../QtCopySharedLibs.cmake")
    endif()
else()
    # TODO: дополнительные действия для других ОС
endif()

# Дополнительные библиотеки для линковки с приложением
SET(ADDITIONAL_LIBS "")

# Если QT линкуется статически
if(QT_STATIC_LINK)
    include("../../QtAddStaticLibs.cmake")
endif()

# Линковка приложения и дополнительных библиотек
target_link_libraries(${TARGET_NAME} "Qt5::Widgets" "Qt5::Network" ${ADDITIONAL_LIBS})