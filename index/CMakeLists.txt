cmake_minimum_required(VERSION 2.8.8)
project(goldendict_index LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Xml Gui Widgets Network Concurrent REQUIRED)

include_directories(../basic)

add_compile_definitions(${goldendict_COMPILE_DEFINITIONS})
set(ISYSTEM_DIRS
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Xml_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Network_INCLUDE_DIRS}
    ${Qt5Concurrent_INCLUDE_DIRS}
)
list(REMOVE_DUPLICATES  ISYSTEM_DIRS)
include_directories(ISYSTEM  ${ISYSTEM_DIRS})

qt5_wrap_cpp(moc_SRCS dictionary.hh)

set(goldendict_index_SRCS
    btreeidx.cc
    chunkedstorage.cc
    dictionary.cc
    ftshelpers.cc
    indexedzip.cc
    instances.cc
    wildcard.cc
)
add_library(goldendict-index SHARED ${goldendict_index_SRCS} ${moc_SRCS})
set_target_properties(goldendict-index PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(goldendict-index
        goldendict-basic
        Qt5::Core
        Qt5::Gui
        Qt5::Xml
        goldendict-basic
        ${LIBLZMA_LIBRARIES}
        ${ZLIB_LIBRARIES}
        ${BZIP2_LIBRARIES}
        -pthread
)
add_dependencies(goldendict-index goldendict-basic)
