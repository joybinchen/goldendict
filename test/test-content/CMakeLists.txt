set(SRCS main.cpp test_utf8.cpp)

find_package(Qt5 COMPONENTS Core Gui Xml REQUIRED)
find_package(BZip2  REQUIRED)
find_package(PkgConfig  REQUIRED)
pkg_check_modules(LIBLZMA REQUIRED  liblzma)
pkg_check_modules(ZLIB REQUIRED  zlib)

include_directories(../../basic ../../index ../../content)

add_executable(test-content ${SRCS})
add_dependencies(test-content goldendict-content)

target_link_libraries(test-content
    libgtest
    libgmock
    Qt5::Core
    Qt5::Gui
    Qt5::Xml
    goldendict-basic
    goldendict-index
    goldendict-content
    -L/usr/lib/x86_64-linux-gnu
    ${LIBLZMA_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${BZIP2_LIBRARIES}
    -pthread
)

add_test(NAME test-content COMMAND test-content)