# Версия CMake
cmake_minimum_required(VERSION 3.5)

# Название цели сборки
set(TARGET_NAME "NetworkApi")

# Добавить в проект header-only библиотеку
add_library(${TARGET_NAME} INTERFACE)

# Указать файлы библиотеки
target_sources(${TARGET_NAME} INTERFACE
        "${CMAKE_SOURCE_DIR}/Sources/NetworkApi/Msg.hpp"
        "${CMAKE_SOURCE_DIR}/Sources/NetworkApi/MsgGameStatus.hpp"
        "${CMAKE_SOURCE_DIR}/Sources/NetworkApi/MsgPlayerQuery.hpp"
        "${CMAKE_SOURCE_DIR}/Sources/NetworkApi/MsgPlayerResponse.hpp"
        "${CMAKE_SOURCE_DIR}/Sources/NetworkApi/MsgShotAvailable.hpp"
        "${CMAKE_SOURCE_DIR}/Sources/NetworkApi/MsgShotDetails.hpp"
        "${CMAKE_SOURCE_DIR}/Sources/NetworkApi/MsgShotResults.hpp"
        "${CMAKE_SOURCE_DIR}/Sources/NetworkApi/BasePeer.hpp"
        "${CMAKE_SOURCE_DIR}/Sources/NetworkApi/PlayerPeer.hpp"
        "${CMAKE_SOURCE_DIR}/Sources/NetworkApi/ServerPeer.hpp"
        "${CMAKE_SOURCE_DIR}/Sources/NetworkApi/GameSession.hpp")