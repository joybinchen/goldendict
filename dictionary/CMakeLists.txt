cmake_minimum_required(VERSION 2.8.8)
project(goldendict_dictionary LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Xml Gui Widgets Network Concurrent REQUIRED)

include_directories( ../content )

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

set(goldendict_dictionary_SRCS
    aard.cc
    belarusiantranslit.cc
    bgl.cc
    chinese.cc
    dictdfiles.cc
    dictserver.cc
    dsl.cc
    epwing.cc
    gls.cc
    lsa.cc
    mdx.cc
    romaji.cc
    sdict.cc
    slob.cc
    sounddir.cc
    stardict.cc
    transliteration.cc
    voiceengines.cc
    xdxf.cc
    zim.cc
    zipsounds.cc
)
add_library(goldendict-dictionary SHARED ${goldendict_dictionary_SRCS})
set_target_properties(goldendict-dictionary PROPERTIES LINKER_LANGUAGE CXX)
add_dependencies(goldendict-dictionary goldendict-content)
target_link_libraries(goldendict-dictionary
    goldendict-basic goldendict-index goldendict-content
)
