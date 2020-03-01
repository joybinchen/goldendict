cmake_minimum_required(VERSION 2.8.8)
project(goldendict LANGUAGES C CXX)

find_package(Threads)
add_subdirectory(locale)
add_subdirectory(redist)
add_subdirectory(help)

find_package(Qt5 COMPONENTS Concurrent Core Gui Help Multimedia Network PrintSupport Sql Svg WebKit WebKitWidgets Widgets X11Extras Xml REQUIRED)
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
pkg_check_modules(VDPAU REQUIRED  vdpau)
pkg_check_modules(VORBIS REQUIRED  vorbis)
pkg_check_modules(VORBISFILE REQUIRED  vorbisfile)
pkg_check_modules(X11 REQUIRED  x11)
pkg_check_modules(XEXT REQUIRED  xext)
pkg_check_modules(XTST REQUIRED  xtst)
pkg_check_modules(XV REQUIRED  xv)
pkg_check_modules(ZLIB REQUIRED  zlib)
add_compile_options(  -pipe -Wall -W)

add_link_options(  -rdynamic)

add_compile_definitions( 
	_REENTRANT
	MAKE_QTMULTIMEDIA_PLAYER
	MAKE_FFMPEG_PLAYER
	HAVE_X11
	PROGRAM_DATA_DIR="/opt/goldendict/share/goldendict/"
	PROGRAM_VERSION="1.5.0-RC2+git"
	MAKE_ZIM_SUPPORT
	MAKE_EXTRA_TIFF_HANDLER
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

set(INCLUDE_DIRS 
	.
	Release
	qtsingleapplication/src
	Release/build
	/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-clang
)
list(REMOVE_DUPLICATES  INCLUDE_DIRS)
include_directories(  ${INCLUDE_DIRS})

set(ISYSTEM_DIRS 
	${HUNSPELL_INCLUDE_DIR}
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

qt5_wrap_cpp(goldendict_moc_SRCS
	about.hh
	article_maker.hh
	article_netmgr.hh
	articleinspector.hh
	articleview.hh
	articlewebview.hh
	audioplayerinterface.hh
	config.hh
	dictheadwords.hh
	dictinfo.hh
	dictionary.hh
	dictionarybar.hh
	dictspanewidget.hh
	editdictionaries.hh
	externalaudioplayer.hh
	externalviewer.hh
	extlineedit.hh
	favoritespanewidget.hh
	ffmpegaudio.hh
	ffmpegaudioplayer.hh
	forvo.hh
	fulltextsearch.hh
	gdappstyle.hh
	groupcombobox.hh
	groups.hh
	groups_widgets.hh
	helpwindow.hh
	history.hh
	historypanewidget.hh
	hotkeyedit.hh
	hotkeywrapper.hh
	initializing.hh
	loaddictionaries.hh
	mainstatusbar.hh
	maintabwidget.hh
	mainwindow.hh
	mediawiki.hh
	mouseover.hh
	mruqmenu.hh
	multimediaaudioplayer.hh
	orderandprops.hh
	preferences.hh
	programs.hh
	qtsingleapplication/src/qtlocalpeer.h
	qtsingleapplication/src/qtsingleapplication.h
	scanflag.hh
	scanpopup.hh
	sources.hh
	stylescombobox.hh
	translatebox.hh
	webmultimediadownload.hh
	website.hh
	wordfinder.hh
	wordlist.hh
)
qt5_add_resources(goldendict_rc_SRCS flags.qrc resources.qrc)
qt5_wrap_ui(goldendict_ui_SRCS
	about.ui
	articleview.ui
	authentication.ui
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
set(MOC_PREDEFS_SRCS
	/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/data/dummy.cpp
)
add_library(moc_predefs OBJECT ${MOC_PREDEFS_SRCS})
set_property(TARGET moc_predefs PROPERTY LIBRARY_OUTPUT_NAME moc_predefs.h)
target_compile_options(moc_predefs PRIVATE -dM -E)

set(GOLDENDICT_SRCS
	aard.cc
	about.cc
	article_maker.cc
	article_netmgr.cc
	articleinspector.cc
	articleview.cc
	articlewebview.cc
	atomic_rename.cc
	audiolink.cc
	audioplayerfactory.cc
	belarusiantranslit.cc
	bgl.cc
	bgl_babylon.cc
	broken_xrecord.cc
	btreeidx.cc
	chunkedstorage.cc
	config.cc
	country.cc
	decompress.cc
	delegate.cc
	dictdfiles.cc
	dictheadwords.cc
	dictinfo.cc
	dictionary.cc
	dictionarybar.cc
	dictserver.cc
	dictzip.c
	dsl.cc
	dsl_details.cc
	editdictionaries.cc
	epwing.cc
	epwing_book.cc
	epwing_charmap.cc
	externalaudioplayer.cc
	externalviewer.cc
	extlineedit.cc
	favoritespanewidget.cc
	ffmpegaudio.cc
	file.cc
	filetype.cc
	folding.cc
	forvo.cc
	fsencoding.cc
	ftshelpers.cc
	fulltextsearch.cc
	gdappstyle.cc
	gddebug.cc
	german.cc
	gestures.cc
	gls.cc
	greektranslit.cc
	groupcombobox.cc
	groups.cc
	groups_widgets.cc
	helpwindow.cc
	history.cc
	historypanewidget.cc
	hotkeyedit.cc
	hotkeywrapper.cc
	htmlescape.cc
	hunspell.cc
	iconv.cc
	indexedzip.cc
	initializing.cc
	instances.cc
	keyboardstate.cc
	langcoder.cc
	language.cc
	loaddictionaries.cc
	lsa.cc
	main.cc
	mainstatusbar.cc
	maintabwidget.cc
	mainwindow.cc
	mdictparser.cc
	mdx.cc
	mediawiki.cc
	mouseover.cc
	mruqmenu.cc
	multimediaaudioplayer.cc
	mutex.cc
	orderandprops.cc
	parsecmdline.cc
	preferences.cc
	processwrapper.cc
	programs.cc
	qtsingleapplication/src/qtlocalpeer.cpp
	qtsingleapplication/src/qtsingleapplication.cpp
	ripemd.cc
	romaji.cc
	russiantranslit.cc
	scanflag.cc
	scanpopup.cc
	sdict.cc
	slob.cc
	sounddir.cc
	sources.cc
	splitfile.cc
	stardict.cc
	stylescombobox.cc
	termination.cc
	tiff.cc
	translatebox.cc
	transliteration.cc
	treeview.cc
	ufile.cc
	utf8.cc
	voiceengines.cc
	webmultimediadownload.cc
	website.cc
	wildcard.cc
	wordfinder.cc
	wordlist.cc
	wstring.cc
	wstring_qt.cc
	xdxf.cc
	xdxf2html.cc
	zim.cc
	zipfile.cc
	zipsounds.cc
)
add_executable(goldendict
	${GOLDENDICT_SRCS}
	${goldendict_moc_SRCS}
	${goldendict_rc_SRCS}
	${goldendict_ui_SRCS}
)
target_compile_options(goldendict PRIVATE -fPIC)
target_include_directories(goldendict PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
set(goldendict_LINKED_LIBRARIES 
	lzo2
	${XTST_LIBRARIES}
	${LIBTIFF4_LIBRARIES}
	eb
	${VORBISFILE_LIBRARIES}
	${VORBIS_LIBRARIES}
	${OGG_LIBRARIES}
	${HUNSPELL_LIBRARIES}
	${AO_LIBRARIES}
	-L/usr/local/lib
	${LIBAVFORMAT_LIBRARIES}
	${LIBAVCODEC_LIBRARIES}
	${LIBLZMA_LIBRARIES}
	${ZLIB_LIBRARIES}
	${AOM_LIBRARIES}
	${LIBSWRESAMPLE_LIBRARIES}
	${LIBAVUTIL_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
	${VDPAU_LIBRARIES}
	${XV_LIBRARIES}
	${X11_LIBRARIES}
	${XEXT_LIBRARIES}
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
	${GL_LIBRARIES}
	pthread
)
list(REMOVE_DUPLICATES  goldendict_LINKED_LIBRARIES)
target_link_libraries(goldendict PRIVATE  ${goldendict_LINKED_LIBRARIES})

target_link_options(goldendict PRIVATE
	-Wl,-rpath-link,/usr/lib/x86_64-linux-gnu
)
add_dependencies(goldendict moc_predefs)
install(TARGETS goldendict DESTINATION bin)
