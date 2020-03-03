set(SRCS main.cpp test_utf8.cpp)

find_package(Qt5 COMPONENTS Core Gui Xml REQUIRED)
find_package(BZip2  REQUIRED)
find_package(PkgConfig  REQUIRED)
pkg_check_modules(LIBLZMA REQUIRED  liblzma)
pkg_check_modules(ZLIB REQUIRED  zlib)

add_executable(test-index ${SRCS})
add_dependencies(test-index goldendict-index)

target_link_libraries(test-index goldendict-index)

add_test(NAME test-index COMMAND test-index)