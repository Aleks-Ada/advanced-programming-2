set(BINARY ${CMAKE_PROJECT_NAME}_test)

set(TEST_SOURCES main.cc board-renderer-test.cc configuration-parser-test.cc)
set(SOURCES ${TEST_SOURCES})

add_executable(${BINARY} ${TEST_SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})

target_link_libraries(${BINARY} PUBLIC ${CMAKE_PROJECT_NAME}_lib gtest gmock)