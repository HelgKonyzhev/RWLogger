CONFIG -= qt

TEMPLATE = lib
DEFINES += LOGGER_LIBRARY

CONFIG += c++11

SOURCES += \
    src/Logger.cpp \
    src/Appender.cpp \
    src/ConsoleAppender.cpp \
    src/FileAppender.cpp \
    src/AsyncAppender.cpp \

HEADERS += \
    include/Logger.h \
    include/Appender.h \
    include/Commons.h \
    include/ConsoleAppender.h \
    include/FileAppender.h \
    include/AsyncAppender.h \

INCLUDEPATH += \
    include

Release:DESTDIR = $$PWD/bin/release
Debug:DESTDIR = $$PWD/bin/debug
