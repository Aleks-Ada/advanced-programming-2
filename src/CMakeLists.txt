set(BINARY ${CMAKE_PROJECT_NAME})

file(COPY adaship_config.ini DESTINATION ${CMAKE_BINARY_DIR})

set(SOURCES main.cc
        board-renderer.cc board-renderer.h
        board/board.cc board/board.h
        board/boat.cc board/boat.h
        configuration/configuration-parser.cc configuration/configuration-parser-test.h
        configuration/configuration.h)

add_executable(${BINARY}_exec ${SOURCES})
add_library(${BINARY}_lib STATIC ${SOURCES})