cmake_minimum_required(VERSION 2.8.8)
project(goldendict_translate LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Gui REQUIRED)

include_directories( ../dictionary )

add_compile_definitions(${goldendict_COMPILE_DEFINITIONS})
set(ISYSTEM_DIRS
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
)
list(REMOVE_DUPLICATES  ISYSTEM_DIRS)
include_directories(ISYSTEM  ${ISYSTEM_DIRS})

set(goldendict_translate_SRCS
    german.cc
    greektranslit.cc
    russiantranslit.cc
)
add_library(goldendict-translate SHARED ${goldendict_translate_SRCS})
set_target_properties(goldendict-translate PROPERTIES LINKER_LANGUAGE CXX)
add_dependencies(goldendict-translate goldendict-dictionary)
target_link_libraries(goldendict-translate
    goldendict-basic goldendict-index goldendict-dictionary)
