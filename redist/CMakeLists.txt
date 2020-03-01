cmake_minimum_required(VERSION 2.8.8)
project(goldendict-redist LANGUAGES C CXX)


install(FILES goldendict.desktop DESTINATION share/applications)

install(FILES goldendict.appdata.xml DESTINATION share/metainfo)

install(FILES icons/goldendict.png DESTINATION share/pixmaps)
