cmake_minimum_required(VERSION 2.8.8)
project(goldendict_content LANGUAGES CXX)
cmake_policy(SET CMP0038 NEW)

find_package(Qt5 COMPONENTS Core Gui Xml REQUIRED)
find_package(BZip2  REQUIRED)
find_package(PkgConfig  REQUIRED)
pkg_check_modules(LIBLZMA REQUIRED  liblzma)
pkg_check_modules(ZLIB REQUIRED  zlib)

include_directories(../basic ../index
)

add_compile_definitions(${goldendict_COMPILE_DEFINITIONS})
set(ISYSTEM_DIRS
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Xml_INCLUDE_DIRS}
)
list(REMOVE_DUPLICATES  ISYSTEM_DIRS)
include_directories(ISYSTEM  ${ISYSTEM_DIRS})

set(goldendict_content_SRCS
    audiolink.cc
    bgl_babylon.cc
    dictzip.c
    dsl_details.cc
    epwing_book.cc
    epwing_charmap.cc
    filetype.cc
    mdictparser.cc
    ripemd.cc
    tiff.cc
    xdxf2html.cc
)
add_library(goldendict-content SHARED ${goldendict_content_SRCS})
set_target_properties(goldendict-content PROPERTIES LINKER_LANGUAGE CXX)
add_dependencies(goldendict-content goldendict-index)
target_link_options(goldendict-content PRIVATE -Wl,--no-undefined)
target_link_libraries(goldendict-content
        Qt5::Core
        Qt5::Gui
        Qt5::Xml
        goldendict-basic
        goldendict-index
        -L/usr/lib/x86_64-linux-gnu
        ${LIBLZMA_LIBRARIES}
        ${ZLIB_LIBRARIES}
        ${BZIP2_LIBRARIES}
        -pthread
)
