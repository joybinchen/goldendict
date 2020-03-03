cmake_minimum_required(VERSION 2.8.8)
project(goldendict_view LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Widgets WebKitWidgets Xml Help X11Extras REQUIRED)
#find_package(Qt5 COMPONENTS Concurrent Core Gui Help Multimedia Network PrintSupport Sql Svg WebKit WebKitWidgets Widgets X11Extras Xml REQUIRED)
add_compile_definitions(${goldendict_COMPILE_DEFINITIONS})

include_directories(
        .
        ../model
        ../webdict
        ../widget
        ../audio
        ../application
        ../qtsingleapplication/src
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
qt5_wrap_cpp(goldendict_view_moc_SRCS
    about.hh
    articleinspector.hh
    articleview.hh
    articlewebview.hh
    chineseconversion.hh
    dictionarybar.hh
    dictinfo.hh
    event.hh
    fulltextsearch.hh
    gdappstyle.hh
    groups.hh
    initializing.hh
    mainstatusbar.hh
    mruqmenu.hh
    orderandprops.hh
    scanflag.hh
    scanpopup.hh
    sources.hh
    translatebox.hh
    wordlist.hh
)

set(goldendict_view_SRCS
    about.cc
    articleinspector.cc
    articleview.cc
    articlewebview.cc
    broken_xrecord.cc
    chineseconversion.cc
    dictinfo.cc
    dictionarybar.cc
    event.cc
    fulltextsearch.cc
    gdappstyle.cc
    gestures.cc
    groups.cc
    hunspell.cc
    initializing.cc
    mainstatusbar.cc
    mruqmenu.cc
    orderandprops.cc
    scanflag.cc
    scanpopup.cc
    sources.cc
    translatebox.cc
    wordlist.cc
)
add_library(goldendict-view SHARED ${goldendict_view_SRCS} ${goldendict_view_moc_SRCS})
set_target_properties(goldendict-view PROPERTIES LINKER_LANGUAGE CXX)
add_dependencies(goldendict-view
    goldendict-ui
    goldendict-model
    goldendict-widget
    goldendict-webdict
    goldendict-audio
)
