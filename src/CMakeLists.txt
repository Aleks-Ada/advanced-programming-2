set(BINARY ${CMAKE_PROJECT_NAME})

configure_file(adaship_config.ini ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
set(SOURCES main.cc
        board/auto-placer.cc board/board.cc board/random-placement-generator.cc
        board-renderer/board-renderer.cc
        configuration/configuration-parser.cc computer-ai.cc computer-ai.h)

add_executable(${BINARY}_exec ${SOURCES})
add_library(${BINARY}_lib STATIC ${SOURCES})