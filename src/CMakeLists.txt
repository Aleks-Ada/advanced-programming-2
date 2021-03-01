set(BINARY ${CMAKE_PROJECT_NAME})

file(COPY adaship_config.ini DESTINATION ${CMAKE_BINARY_DIR})

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cc)
set(SOURCES ${SOURCES})
set(SOURCES ${SOURCES} board/boat.cc)

add_executable(${BINARY}_exec ${SOURCES})
add_library(${BINARY}_lib STATIC ${SOURCES})