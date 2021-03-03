set(BINARY ${CMAKE_PROJECT_NAME})

configure_file(adaship_config.ini ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
set(SOURCES main.cc
        board-renderer/board-renderer.cc board-renderer/board-renderer.h
        board/board.cc board/board.h
        board/boat.cc board/boat.h
        configuration/configuration-parser.cc configuration/configuration-parser-test.h
        configuration/configuration.h)

add_executable(${BINARY}_exec ${SOURCES})
add_library(${BINARY}_lib STATIC ${SOURCES})