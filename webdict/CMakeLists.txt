cmake_minimum_required(VERSION 2.8.8)
project(goldendict_webdict LANGUAGES CXX)

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

qt5_wrap_cpp(goldendict_webdict_moc_SRCS
    forvo.hh
    mediawiki.hh
    programs.hh
    website.hh
)
set(goldendict_webdict_SRCS
    country.cc
    forvo.cc
    mediawiki.cc
    programs.cc
    website.cc
)
add_library(goldendict-webdict SHARED ${goldendict_webdict_SRCS} ${goldendict_webdict_moc_SRCS})
set_target_properties(goldendict-webdict PROPERTIES LINKER_LANGUAGE CXX)
add_dependencies(goldendict-webdict goldendict-content)
target_link_libraries(goldendict-webdict
    goldendict-basic goldendict-index goldendict-content
)
