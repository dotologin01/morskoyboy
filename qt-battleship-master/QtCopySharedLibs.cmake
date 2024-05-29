ADD_CUSTOM_COMMAND(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${QT5_DIR}bin\\Qt5Core$<$<CONFIG:Debug>:d>.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E copy ${QT5_DIR}bin\\Qt5Widgets$<$<CONFIG:Debug>:d>.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E copy ${QT5_DIR}bin\\Qt5Gui$<$<CONFIG:Debug>:d>.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E copy ${QT5_DIR}bin\\Qt5Network$<$<CONFIG:Debug>:d>.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMENT "Copying Qt binaries" VERBATIM)
# Если это GNU (MinGW) нужны еще runtime-библиотеки
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    ADD_CUSTOM_COMMAND(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${QT5_DIR}bin\\libgcc_s_dw2-1.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            #COMMAND ${CMAKE_COMMAND} -E copy ${QT5_DIR}bin\\libstdc++-6.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy ${QT5_DIR}bin\\libwinpthread-1.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMENT "Copying MinGW runtime binaries for Qt" VERBATIM)
endif()