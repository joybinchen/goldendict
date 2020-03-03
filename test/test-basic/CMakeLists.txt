set(SRCS main.cpp test_utf8.cpp)

include_directories(../../basic)

add_executable(test-basic ${SRCS})
add_dependencies(test-basic goldendict-basic)

add_test(NAME test-basic
         COMMAND test-basic)