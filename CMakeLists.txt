cmake_minimum_required(VERSION 2.8.8)
project(gdb-Release LANGUAGES C CXX)

find_package(Threads)
add_compile_options(  -rdynamic -pipe -Wall -W)
add_link_options( 
	-Wl,-rpath-link,/usr/lib/x86_64-linux-gnu
)
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
include_directories( AFTER
	.
	Release
	qtsingleapplication/src
	Release/build
	/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-clang
)
include_directories(SYSTEM AFTER
	/usr/include/hunspell
	/usr/local/include
	/usr/include/x86_64-linux-gnu/qt5
	/usr/include/x86_64-linux-gnu/qt5/QtSvg
	/usr/include/x86_64-linux-gnu/qt5/QtWebKitWidgets
	/usr/include/x86_64-linux-gnu/qt5/QtPrintSupport
	/usr/include/x86_64-linux-gnu/qt5/QtHelp
	/usr/include/x86_64-linux-gnu/qt5/QtWidgets
	/usr/include/x86_64-linux-gnu/qt5/QtWebKit
	/usr/include/x86_64-linux-gnu/qt5/QtMultimedia
	/usr/include/x86_64-linux-gnu/qt5/QtX11Extras
	/usr/include/x86_64-linux-gnu/qt5/QtGui
	/usr/include/x86_64-linux-gnu/qt5/QtXml
	/usr/include/x86_64-linux-gnu/qt5/QtNetwork
	/usr/include/x86_64-linux-gnu/qt5/QtSql
	/usr/include/x86_64-linux-gnu/qt5/QtConcurrent
	/usr/include/x86_64-linux-gnu/qt5/QtCore
	/usr/include/libdrm
)

foreach(X
	about
	article_maker
	article_netmgr
	articleinspector
	articleview
	articlewebview
	audioplayerinterface
	config
	dictheadwords
	dictinfo
	dictionary
	dictionarybar
	dictspanewidget
	editdictionaries
	externalaudioplayer
	externalviewer
	extlineedit
	favoritespanewidget
	ffmpegaudio
	ffmpegaudioplayer
	forvo
	fulltextsearch
	gdappstyle
	groupcombobox
	groups
	groups_widgets
	helpwindow
	history
	historypanewidget
	hotkeyedit
	hotkeywrapper
	initializing
	loaddictionaries
	mainstatusbar
	maintabwidget
	mainwindow
	mediawiki
	mouseover
	mruqmenu
	multimediaaudioplayer
	orderandprops
	preferences
	programs
	scanflag
	scanpopup
	sources
	stylescombobox
	translatebox
	webmultimediadownload
	website
	wordfinder
	wordlist
)
	add_custom_command(OUTPUT Release/build/moc_${X}.cpp
		COMMAND moc --include ${CMAKE_CURRENT_SOURCE_DIR}/Release/build/moc_predefs.h -I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-clang -I${CMAKE_CURRENT_SOURCE_DIR}/. -I${CMAKE_CURRENT_SOURCE_DIR}/. -I${CMAKE_CURRENT_SOURCE_DIR}/qtsingleapplication/src -I/usr/include/hunspell -I/usr/local/include -I/usr/include/x86_64-linux-gnu/qt5 -I/usr/include/x86_64-linux-gnu/qt5/QtSvg -I/usr/include/x86_64-linux-gnu/qt5/QtWebKitWidgets -I/usr/include/x86_64-linux-gnu/qt5/QtPrintSupport -I/usr/include/x86_64-linux-gnu/qt5/QtHelp -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets -I/usr/include/x86_64-linux-gnu/qt5/QtWebKit -I/usr/include/x86_64-linux-gnu/qt5/QtMultimedia -I/usr/include/x86_64-linux-gnu/qt5/QtX11Extras -I/usr/include/x86_64-linux-gnu/qt5/QtGui -I/usr/include/x86_64-linux-gnu/qt5/QtXml -I/usr/include/x86_64-linux-gnu/qt5/QtNetwork -I/usr/include/x86_64-linux-gnu/qt5/QtSql -I/usr/include/x86_64-linux-gnu/qt5/QtConcurrent -I/usr/include/x86_64-linux-gnu/qt5/QtCore -I${CMAKE_CURRENT_SOURCE_DIR}/Release -I/usr/include/c++/7.4.0 -I/usr/include/x86_64-linux-gnu/c++/7.4.0 -I/usr/include/c++/7.4.0/backward -I/usr/include/clang/6.0.0/include -I/usr/local/include -I/usr/include/x86_64-linux-gnu -I/usr/include -DMAKE_QTMULTIMEDIA_PLAYER -DMAKE_FFMPEG_PLAYER -DHAVE_X11 -DPROGRAM_DATA_DIR="/opt/goldendict/share/goldendict/" -DPROGRAM_VERSION="1.5.0-RC2+git" -DMAKE_ZIM_SUPPORT -DMAKE_EXTRA_TIFF_HANDLER -DQT_NO_DEBUG -DQT_SVG_LIB -DQT_WEBKITWIDGETS_LIB -DQT_PRINTSUPPORT_LIB -DQT_HELP_LIB -DQT_WIDGETS_LIB -DQT_WEBKIT_LIB -DQT_MULTIMEDIA_LIB -DQT_X11EXTRAS_LIB -DQT_GUI_LIB -DQT_XML_LIB -DQT_NETWORK_LIB -DQT_SQL_LIB -DQT_CONCURRENT_LIB -DQT_CORE_LIB
		${X}.hh
		-o ${CMAKE_CURRENT_BINARY_DIR}/Release/build/moc_${X}.cpp
	)

endforeach(X)

foreach(X qtlocalpeer qtsingleapplication)
	add_custom_command(OUTPUT Release/build/moc_${X}.cpp
		COMMAND moc --include ${CMAKE_CURRENT_SOURCE_DIR}/Release/build/moc_predefs.h -I/usr/lib/x86_64-linux-gnu/qt5/mkspecs/linux-clang -I${CMAKE_CURRENT_SOURCE_DIR}/. -I${CMAKE_CURRENT_SOURCE_DIR}/. -I${CMAKE_CURRENT_SOURCE_DIR}/qtsingleapplication/src -I/usr/include/hunspell -I/usr/local/include -I/usr/include/x86_64-linux-gnu/qt5 -I/usr/include/x86_64-linux-gnu/qt5/QtSvg -I/usr/include/x86_64-linux-gnu/qt5/QtWebKitWidgets -I/usr/include/x86_64-linux-gnu/qt5/QtPrintSupport -I/usr/include/x86_64-linux-gnu/qt5/QtHelp -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets -I/usr/include/x86_64-linux-gnu/qt5/QtWebKit -I/usr/include/x86_64-linux-gnu/qt5/QtMultimedia -I/usr/include/x86_64-linux-gnu/qt5/QtX11Extras -I/usr/include/x86_64-linux-gnu/qt5/QtGui -I/usr/include/x86_64-linux-gnu/qt5/QtXml -I/usr/include/x86_64-linux-gnu/qt5/QtNetwork -I/usr/include/x86_64-linux-gnu/qt5/QtSql -I/usr/include/x86_64-linux-gnu/qt5/QtConcurrent -I/usr/include/x86_64-linux-gnu/qt5/QtCore -I${CMAKE_CURRENT_SOURCE_DIR}/Release -I/usr/include/c++/7.4.0 -I/usr/include/x86_64-linux-gnu/c++/7.4.0 -I/usr/include/c++/7.4.0/backward -I/usr/include/clang/6.0.0/include -I/usr/local/include -I/usr/include/x86_64-linux-gnu -I/usr/include -DMAKE_QTMULTIMEDIA_PLAYER -DMAKE_FFMPEG_PLAYER -DHAVE_X11 -DPROGRAM_DATA_DIR="/opt/goldendict/share/goldendict/" -DPROGRAM_VERSION="1.5.0-RC2+git" -DMAKE_ZIM_SUPPORT -DMAKE_EXTRA_TIFF_HANDLER -DQT_NO_DEBUG -DQT_SVG_LIB -DQT_WEBKITWIDGETS_LIB -DQT_PRINTSUPPORT_LIB -DQT_HELP_LIB -DQT_WIDGETS_LIB -DQT_WEBKIT_LIB -DQT_MULTIMEDIA_LIB -DQT_X11EXTRAS_LIB -DQT_GUI_LIB -DQT_XML_LIB -DQT_NETWORK_LIB -DQT_SQL_LIB -DQT_CONCURRENT_LIB -DQT_CORE_LIB
		qtsingleapplication/src/${X}.h
		-o ${CMAKE_CURRENT_BINARY_DIR}/Release/build/moc_${X}.cpp
	)

endforeach(X)

set(MOC_PREDEFS_SRCS
	/usr/lib/x86_64-linux-gnu/qt5/mkspecs/features/data/dummy.cpp
)
add_executable(moc_predefs ${MOC_PREDEFS_SRCS})
target_compile_options(moc_predefs PRIVATE -dM -E)

foreach(X
	about
	articleview
	authentication
	dictgroupwidget
	dictheadwords
	dictinfo
	editdictionaries
	fulltextsearch
	groups
	initializing
	mainwindow
	orderandprops
	preferences
	scanflag
	scanpopup
	sources
)
	add_custom_command(OUTPUT Release/build/ui_${X}.h COMMAND uic  ${X}.ui  ${CMAKE_CURRENT_BINARY_DIR}/Release/build/ui_${X}.h)

endforeach(X)

set(GOLDENDICT_SRCS
	Release/build/moc_about.cpp
	Release/build/moc_article_maker.cpp
	Release/build/moc_article_netmgr.cpp
	Release/build/moc_articleinspector.cpp
	Release/build/moc_articleview.cpp
	Release/build/moc_articlewebview.cpp
	Release/build/moc_audioplayerinterface.cpp
	Release/build/moc_config.cpp
	Release/build/moc_dictheadwords.cpp
	Release/build/moc_dictinfo.cpp
	Release/build/moc_dictionary.cpp
	Release/build/moc_dictionarybar.cpp
	Release/build/moc_dictspanewidget.cpp
	Release/build/moc_editdictionaries.cpp
	Release/build/moc_externalaudioplayer.cpp
	Release/build/moc_externalviewer.cpp
	Release/build/moc_extlineedit.cpp
	Release/build/moc_favoritespanewidget.cpp
	Release/build/moc_ffmpegaudio.cpp
	Release/build/moc_ffmpegaudioplayer.cpp
	Release/build/moc_forvo.cpp
	Release/build/moc_fulltextsearch.cpp
	Release/build/moc_gdappstyle.cpp
	Release/build/moc_groupcombobox.cpp
	Release/build/moc_groups.cpp
	Release/build/moc_groups_widgets.cpp
	Release/build/moc_helpwindow.cpp
	Release/build/moc_history.cpp
	Release/build/moc_historypanewidget.cpp
	Release/build/moc_hotkeyedit.cpp
	Release/build/moc_hotkeywrapper.cpp
	Release/build/moc_initializing.cpp
	Release/build/moc_loaddictionaries.cpp
	Release/build/moc_mainstatusbar.cpp
	Release/build/moc_maintabwidget.cpp
	Release/build/moc_mainwindow.cpp
	Release/build/moc_mediawiki.cpp
	Release/build/moc_mouseover.cpp
	Release/build/moc_mruqmenu.cpp
	Release/build/moc_multimediaaudioplayer.cpp
	Release/build/moc_orderandprops.cpp
	Release/build/moc_preferences.cpp
	Release/build/moc_programs.cpp
	Release/build/moc_qtlocalpeer.cpp
	Release/build/moc_qtsingleapplication.cpp
	Release/build/moc_scanflag.cpp
	Release/build/moc_scanpopup.cpp
	Release/build/moc_sources.cpp
	Release/build/moc_stylescombobox.cpp
	Release/build/moc_translatebox.cpp
	Release/build/moc_webmultimediadownload.cpp
	Release/build/moc_website.cpp
	Release/build/moc_wordfinder.cpp
	Release/build/moc_wordlist.cpp
	Release/build/qrc_flags.cpp
	Release/build/qrc_resources.cpp
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
add_executable(goldendict ${GOLDENDICT_SRCS})
target_compile_options(goldendict PRIVATE -fPIC)
target_include_directories(goldendict PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
install(TARGETS goldendict DESTINATION bin/goldendict)

foreach(X
	ar_SA	ay_WI	be_BY	be_BY@latin	bg_BG	cs_CZ	de_DE	el_GR	eo_EO
	es_BO	es_ES	fa_IR	fi_FI	fr_FR	it_IT	ja_JP	ko_KR	lt_LT
	nl_NL	pl_PL	pt_BR	qu_WI	ru_RU	sk_SK	sq_AL	sr_SR	sv_SE
	tk_TM	tr_TR	uk_UA	vi_VN	zh_CN	zh_TW
)
	add_custom_command(OUTPUT Release/locale/${X}.qm COMMAND lrelease  locale/${X}.ts  ${CMAKE_CURRENT_BINARY_DIR}/Release/locale/${X}.qm)

endforeach(X)

install(FILES redist/goldendict.desktop DESTINATION share/applications/goldendict.desktop)

install(FILES redist/icons/goldendict.png DESTINATION share/pixmaps/goldendict.png)

install(FILES redist/goldendict.appdata.xml DESTINATION share/metainfo/goldendict.appdata.xml)

set( LOCALE_FILES
	locale/ar_SA.qm
	locale/ay_WI.qm
	locale/be_BY.qm
	locale/be_BY@latin.qm
	locale/bg_BG.qm
	locale/cs_CZ.qm
	locale/de_DE.qm
	locale/el_GR.qm
	locale/eo_EO.qm
	locale/es_AR.qm
	locale/es_BO.qm
	locale/es_ES.qm
	locale/fa_IR.qm
	locale/fi_FI.qm
	locale/fr_FR.qm
	locale/it_IT.qm
	locale/ja_JP.qm
	locale/ko_KR.qm
	locale/lt_LT.qm
	locale/mk_MK.qm
	locale/nl_NL.qm
	locale/pl_PL.qm
	locale/pt_BR.qm
	locale/qu_WI.qm
	locale/ru_RU.qm
	locale/sk_SK.qm
	locale/sq_AL.qm
	locale/sr_SR.qm
	locale/sv_SE.qm
	locale/tg_TJ.qm
	locale/tk_TM.qm
	locale/tr_TR.qm
	locale/uk_UA.qm
	locale/vi_VN.qm
	locale/zh_CN.qm
	locale/zh_TW.qm
)
install(FILES ${LOCALE_FILES} DESTINATION share/goldendict/locale)

install(FILES help/gdhelp_en.qch help/gdhelp_ru.qch DESTINATION share/goldendict/help)
