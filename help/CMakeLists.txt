cmake_minimum_required(VERSION 2.8.8)
project(goldendict-help LANGUAGES C CXX)


set( HELP_FILES gdhelp_en.qch gdhelp_ru.qch)
install(FILES ${HELP_FILES} DESTINATION share/goldendict/help)
