cmake_minimum_required(VERSION 2.8.8)
project(goldendict_widget LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Widgets WebKitWidgets Xml Help X11Extras REQUIRED)
#find_package(Qt5 COMPONENTS Concurrent Core Gui Help Multimedia Network PrintSupport Sql Svg WebKit WebKitWidgets Widgets X11Extras Xml REQUIRED)
add_compile_definitions(${goldendict_COMPILE_DEFINITIONS})

include_directories(. )

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
qt5_wrap_cpp(goldendict_widget_moc_SRCS
        dictspanewidget.hh
        extlineedit.hh
        favoritespanewidget.hh
        groupcombobox.hh
        groups_widgets.hh
        helpwindow.hh
        history.hh
        historypanewidget.hh
        hotkeyedit.hh
        maintabwidget.hh
        mouseover.hh
        stylescombobox.hh
)

set(goldendict_widget_SRCS
        delegate.cc
        extlineedit.cc
        favoritespanewidget.cc
        groupcombobox.cc
        groups_widgets.cc
        helpwindow.cc
        history.cc
        historypanewidget.cc
        hotkeyedit.cc
        keyboardstate.cc
        maintabwidget.cc
        mouseover.cc
        stylescombobox.cc
        treeview.cc
)
add_library(goldendict-widget SHARED ${goldendict_widget_SRCS} ${goldendict_widget_moc_SRCS})
set_target_properties(goldendict-widget PROPERTIES LINKER_LANGUAGE CXX)
add_dependencies(goldendict-widget goldendict-ui)
add_dependencies(goldendict-widget goldendict-model)
