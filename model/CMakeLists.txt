cmake_minimum_required(VERSION 2.8.8)
project(goldendict_model LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Xml Gui Widgets WebKit Network Concurrent REQUIRED)

include_directories( ../translate )

add_compile_definitions(${goldendict_COMPILE_DEFINITIONS})
set(ISYSTEM_DIRS
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Xml_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5WebKit_INCLUDE_DIRS}
    ${Qt5Network_INCLUDE_DIRS}
    ${Qt5Concurrent_INCLUDE_DIRS}
)
list(REMOVE_DUPLICATES  ISYSTEM_DIRS)
include_directories(ISYSTEM  ${ISYSTEM_DIRS})
qt5_wrap_cpp(goldendict_model_moc_SRCS
    article_maker.hh
    article_netmgr.hh
    webmultimediadownload.hh
    wordfinder.hh
)

set(goldendict_model_SRCS
    article_maker.cc
    article_netmgr.cc
    webmultimediadownload.cc
    wordfinder.cc
)
add_library(goldendict-model SHARED ${goldendict_model_SRCS} ${goldendict_model_moc_SRCS})
set_target_properties(goldendict-model PROPERTIES LINKER_LANGUAGE CXX)
add_dependencies(goldendict-model goldendict-content)
add_dependencies(goldendict-model goldendict-translate)
target_link_libraries(goldendict-model Qt5::WebKit goldendict-translate)
