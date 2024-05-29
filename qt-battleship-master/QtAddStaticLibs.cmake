if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    SET(LIB_EXT ".lib")
else()
    SET(LIB_EXT ".a")
endif()

# Основные модули QT
SET(ADDITIONAL_LIBS ${ADDITIONAL_LIBS}
        "${QT5_DIR}/lib/qtpcre2$<$<CONFIG:Debug>:d>${LIB_EXT}"
        "${QT5_DIR}/lib/qtlibpng$<$<CONFIG:Debug>:d>${LIB_EXT}"
        "${QT5_DIR}/lib/qtharfbuzz$<$<CONFIG:Debug>:d>${LIB_EXT}"
        "${QT5_DIR}/lib/qtfreetype$<$<CONFIG:Debug>:d>${LIB_EXT}"
        "${QT5_DIR}/lib/Qt5FontDatabaseSupport$<$<CONFIG:Debug>:d>${LIB_EXT}"
        "${QT5_DIR}/lib/Qt5ThemeSupport$<$<CONFIG:Debug>:d>${LIB_EXT}"
        "${QT5_DIR}/lib/Qt5EventDispatcherSupport$<$<CONFIG:Debug>:d>${LIB_EXT}"
        "${QT5_DIR}/lib/Qt5AccessibilitySupport$<$<CONFIG:Debug>:d>${LIB_EXT}")

if(WIN32)
    # Плагины
    SET(ADDITIONAL_LIBS ${ADDITIONAL_LIBS} "${QT5_DIR}/plugins/platforms/qwindows$<$<CONFIG:Debug>:d>${LIB_EXT}")
    # Библиотеки ОС
    SET(ADDITIONAL_LIBS ${ADDITIONAL_LIBS} "imm32.lib" "version.lib" "netapi32.lib" "uxtheme.lib" "dwmapi.lib" "Ws2_32.lib" "Winmm.lib" "Iphlpapi.lib")
endif()