cmake_minimum_required(VERSION 2.8.8)
project(goldendict_audio LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Widgets WebKitWidgets Multimedia REQUIRED)

add_compile_definitions(${goldendict_COMPILE_DEFINITIONS})


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
qt5_wrap_cpp(goldendict_audio_moc_SRCS
        audioplayerinterface.hh
        externalaudioplayer.hh
        externalviewer.hh
        ffmpegaudio.hh
        ffmpegaudioplayer.hh
        multimediaaudioplayer.hh
)

set(goldendict_audio_SRCS
        audioplayerfactory.cc
        externalaudioplayer.cc
        externalviewer.cc
        ffmpegaudio.cc
        multimediaaudioplayer.cc
)
add_library(goldendict-audio SHARED ${goldendict_audio_SRCS} ${goldendict_audio_moc_SRCS})
set_target_properties(goldendict-audio PROPERTIES LINKER_LANGUAGE CXX)
add_dependencies(goldendict-audio goldendict-ui)
add_dependencies(goldendict-audio goldendict-model)
