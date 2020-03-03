cmake_minimum_required(VERSION 2.8.8)
project(goldendict_ui LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
include_directories(../widget)

qt5_wrap_ui(goldendict_ui_SRCS
    about.ui
    articleview.ui
    authentication.ui
    chineseconversion.ui
    dictgroupwidget.ui
    dictheadwords.ui
    dictinfo.ui
    editdictionaries.ui
    fulltextsearch.ui
    groups.ui
    initializing.ui
    mainwindow.ui
    orderandprops.ui
    preferences.ui
    scanflag.ui
    scanpopup.ui
    sources.ui
)
add_library(goldendict-ui SHARED ${goldendict_ui_SRCS})
set_target_properties(goldendict-ui PROPERTIES LINKER_LANGUAGE CXX)
