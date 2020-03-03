cmake_minimum_required(VERSION 2.8.8)
project(goldendict_application LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Widgets WebKitWidgets Xml X11Extras PrintSupport REQUIRED)
#find_package(Qt5 COMPONENTS Concurrent Core Gui Help Multimedia Network PrintSupport Sql Svg WebKit WebKitWidgets Widgets X11Extras Xml REQUIRED)
add_compile_definitions(${goldendict_COMPILE_DEFINITIONS})
add_compile_options(  -pipe -Wall -W -fPIC)

message({goldendict_ui_BINARY_DIR} ${goldendict_ui_BINARY_DIR})
include_directories(
        .
        ../model
        ../widget
        ../audio
        ../translate
        ../content
        ../dictionary
        ../webdict
        ../view
        qtsingleapplication/src
        ${goldendict_ui_BINARY_DIR}
)

set(ISYSTEM_DIRS
    ${Qt5Concurrent_INCLUDE_DIRS}
    ${Qt5Svg_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5WebKit_INCLUDE_DIRS}
    ${Qt5WebKitWidgets_INCLUDE_DIRS}
    ${Qt5PrintSupport_INCLUDE_DIRS}
    ${Qt5Help_INCLUDE_DIRS}
    ${Qt5Multimedia_INCLUDE_DIRS}
    ${Qt5X11Extras_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Xml_INCLUDE_DIRS}
)
list(REMOVE_DUPLICATES  ISYSTEM_DIRS)
include_directories(ISYSTEM  ${ISYSTEM_DIRS})
qt5_wrap_cpp(moc_SRCS
    dictheadwords.hh
    editdictionaries.hh
    hotkeywrapper.hh
    loaddictionaries.hh
    mainwindow.hh
    preferences.hh
    qtsingleapplication/src/qtlocalpeer.h
    qtsingleapplication/src/qtsingleapplication.h
)

set(goldendict_application_SRCS
    dictheadwords.cc
    editdictionaries.cc
    hotkeywrapper.cc
    loaddictionaries.cc
    mainwindow.cc
    preferences.cc
    qtsingleapplication/src/qtlocalpeer.cpp
    qtsingleapplication/src/qtsingleapplication.cpp
)
add_library(goldendict-application SHARED ${goldendict_application_SRCS} ${moc_SRCS})
set_target_properties(goldendict-application PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(goldendict-application
        Qt5::PrintSupport
        goldendict-view goldendict-model goldendict-dictionary)
add_dependencies(goldendict-application goldendict-dictionary)
add_dependencies(goldendict-application goldendict-model)
add_dependencies(goldendict-application goldendict-view)
