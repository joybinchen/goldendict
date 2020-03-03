cmake_minimum_required(VERSION 2.8.8)
project(goldendict LANGUAGES C CXX)

find_package(Threads)

set(goldendict_COMPILE_DEFINITIONS
	_REENTRANT
	MAKE_QTMULTIMEDIA_PLAYER
	MAKE_FFMPEG_PLAYER
	HAVE_X11
	PROGRAM_DATA_DIR="/opt/goldendict/share/goldendict/"
	PROGRAM_VERSION="1.5.0-RC2+git"
	MAKE_ZIM_SUPPORT
	MAKE_EXTRA_TIFF_HANDLER
	MAKE_CHINESE_CONVERSION_SUPPORT
	QT_NO_DEBUG
	QT_SVG_LIB
	QT_WEBKITWIDGETS_LIB
	QT_PRINTSUPPORT_LIB
	QT_HELP_LIB
	QT_WIDGETS_LIB
	QT_WEBKIT_LIB
	QT_MULTIMEDIA_LIB
	QT_X11EXTRAS_LIB
	QT_GUI_LIB
	QT_XML_LIB
	QT_NETWORK_LIB
	QT_SQL_LIB
	QT_CONCURRENT_LIB
	QT_CORE_LIB
)

find_package(Qt5 COMPONENTS Concurrent Core Gui Help Multimedia Network PrintSupport Sql Svg WebKit WebKitWidgets Widgets X11Extras Xml REQUIRED)
find_package(BZip2  REQUIRED)
find_package(PkgConfig  REQUIRED)
pkg_check_modules(AO REQUIRED  ao)
pkg_check_modules(AOM REQUIRED  aom)
pkg_check_modules(GL REQUIRED  gl)
pkg_check_modules(HUNSPELL REQUIRED  hunspell)
pkg_check_modules(LIBAVCODEC REQUIRED  libavcodec)
pkg_check_modules(LIBAVFORMAT REQUIRED  libavformat)
pkg_check_modules(LIBAVUTIL REQUIRED  libavutil)
pkg_check_modules(LIBLZMA REQUIRED  liblzma)
pkg_check_modules(LIBSWRESAMPLE REQUIRED  libswresample)
pkg_check_modules(LIBTIFF4 REQUIRED  libtiff-4)
pkg_check_modules(OGG REQUIRED  ogg)
pkg_check_modules(OPENCC REQUIRED  opencc)
pkg_check_modules(VDPAU REQUIRED  vdpau)
pkg_check_modules(VORBIS REQUIRED  vorbis)
pkg_check_modules(VORBISFILE REQUIRED  vorbisfile)
pkg_check_modules(X11 REQUIRED  x11)
pkg_check_modules(XEXT REQUIRED  xext)
pkg_check_modules(XTST REQUIRED  xtst)
pkg_check_modules(XV REQUIRED  xv)
pkg_check_modules(ZLIB REQUIRED  zlib)
add_compile_options(  -pipe -Wall -W -fPIC)

add_link_options(-Wl,--no-undefined)
add_compile_definitions(${goldendict_COMPILE_DEFINITIONS})

add_subdirectory(locale)
add_subdirectory(redist)
add_subdirectory(help)
add_subdirectory(basic)
include_directories(basic)

link_libraries(goldendict-basic)
add_subdirectory(index)
include_directories(index)

link_libraries(goldendict-index eb lzo2 ${LIBTIFF4_LIBRARIES})
add_subdirectory(content)
include_directories(content)

link_libraries(Qt5::Network)
add_subdirectory(webdict)

link_libraries(Qt5::Svg ${OPENCC_LIBRARIES} ${VORBISFILE_LIBRARIES})
add_subdirectory(dictionary)
add_subdirectory(translate)
add_subdirectory(model)
add_subdirectory(ui)

include_directories(${goldendict_ui_BINARY_DIR})
link_libraries(Qt5::Help Qt5::X11Extras ${AOM_LIBRARIES} ${X11_LIBRARIES})
add_subdirectory(widget)

link_libraries(Qt5::Multimedia
		${AO_LIBRARIES} ${LIBAVCODEC_LIBRARIES} ${LIBAVFORMAT_LIBRARIES})
add_subdirectory(audio)

link_libraries(goldendict-model goldendict-webdict
		goldendict-ui goldendict-widget goldendict-audio
		Qt5::WebKitWidgets
		${HUNSPELL_LIBRARIES}
)
add_subdirectory(view)

link_libraries(
	goldendict-translate
	goldendict-content
	goldendict-dictionary
	goldendict-view
	${XTST_LIBRARIES}
)
add_subdirectory(application)
add_subdirectory(test)

set(INCLUDE_DIRS
	basic
	dictionary
	webdict
	${PROJECT_SOURCE_DIR}
	ui
	index
	model
	webdict
	widget
	view
	audio
	application
	${goldendict_ui_BINARY_DIR}
)
list(REMOVE_DUPLICATES  INCLUDE_DIRS)
include_directories(  ${INCLUDE_DIRS})

set(ISYSTEM_DIRS 
	${AOM_INCLUDE_DIR}
	${Qt5Concurrent_INCLUDE_DIRS}
	${Qt5Svg_INCLUDE_DIRS}
	${Qt5WebKitWidgets_INCLUDE_DIRS}
	${Qt5PrintSupport_INCLUDE_DIRS}
	${Qt5Help_INCLUDE_DIRS}
	${Qt5Multimedia_INCLUDE_DIRS}
	${Qt5X11Extras_INCLUDE_DIRS}
	${Qt5Gui_INCLUDE_DIRS}
	${Qt5Xml_INCLUDE_DIRS}
	${GL_INCLUDE_DIR}
)
list(REMOVE_DUPLICATES  ISYSTEM_DIRS)
include_directories(ISYSTEM  ${ISYSTEM_DIRS})

qt5_add_resources(goldendict_rc_SRCS flags.qrc resources.qrc)
set(MOC_PREDEFS_SRCS
	/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/data/dummy.cpp
)
add_library(moc_predefs OBJECT ${MOC_PREDEFS_SRCS})
set_property(TARGET moc_predefs PROPERTY LIBRARY_OUTPUT_NAME moc_predefs.h)
target_compile_options(moc_predefs PRIVATE -dM -E)

set(GOLDENDICT_SRCS
	main.cc
	processwrapper.cc
	termination.cc
)
add_executable(goldendict
	${GOLDENDICT_SRCS}
	${goldendict_rc_SRCS}
	${goldendict_ui_SRCS}
)
target_include_directories(goldendict PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}
        application/qtsingleapplication/src
)
set(goldendict_LINKED_LIBRARIES
	Qt5::Svg
	Qt5::WebKitWidgets
	Qt5::PrintSupport
	Qt5::Help
	Qt5::Widgets
	Qt5::WebKit
	Qt5::Multimedia
	Qt5::X11Extras
	Qt5::Gui
	Qt5::Xml
	Qt5::Network
	Qt5::Sql
	Qt5::Concurrent
	Qt5::Core
	${XTST_LIBRARIES}
	${OPENCC_LIBRARIES}
	${VORBIS_LIBRARIES}
	${OGG_LIBRARIES}
	-L/usr/local/lib
	${LIBAVFORMAT_LIBRARIES}
	${LIBAVCODEC_LIBRARIES}
	${LIBLZMA_LIBRARIES}
	${ZLIB_LIBRARIES}
	${LIBSWRESAMPLE_LIBRARIES}
	${LIBAVUTIL_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
	${VDPAU_LIBRARIES}
	${XV_LIBRARIES}
	${XEXT_LIBRARIES}
	${GL_LIBRARIES}
	pthread
)
list(REMOVE_DUPLICATES  goldendict_LINKED_LIBRARIES)
target_link_libraries(goldendict ${goldendict_LINKED_LIBRARIES})
target_link_libraries(goldendict goldendict-application)

target_link_options(goldendict PRIVATE
	-Wl,-rpath-link,/usr/lib/x86_64-linux-gnu
)
add_dependencies(goldendict moc_predefs)
add_dependencies(goldendict goldendict-application)
install(TARGETS goldendict DESTINATION bin)
