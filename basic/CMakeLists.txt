cmake_minimum_required(VERSION 2.8.8)

set(ISYSTEM_DIRS
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Xml_INCLUDE_DIRS}
)
list(REMOVE_DUPLICATES  ISYSTEM_DIRS)
include_directories(ISYSTEM  ${ISYSTEM_DIRS})

set(goldendict_basic_SRCS
    atomic_rename.cc
    config.cc
    decompress.cc
    file.cc
    folding.cc
    fsencoding.cc
    gddebug.cc
    htmlescape.cc
    iconv.cc
    langcoder.cc
    language.cc
    mutex.cc
    parsecmdline.cc
    splitfile.cc
    ufile.cc
    utf8.cc
    wstring.cc
    wstring_qt.cc
    zipfile.cc
)
add_library(goldendict-basic SHARED ${goldendict_basic_SRCS})
target_link_libraries(goldendict-basic
        Qt5::Core
        Qt5::Gui
        Qt5::Xml
        ${LIBLZMA_LIBRARIES}
        ${ZLIB_LIBRARIES}
        ${BZIP2_LIBRARIES}
)
set_target_properties(goldendict-basic PROPERTIES LINKER_LANGUAGE CXX)
