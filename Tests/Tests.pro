TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

include ($$PWD/../Logger/Logger.pri)

SOURCES += \
        $$PWD/ArgsToStringTest.cpp \
        $$PWD/AsyncAppenderQueueOverflowTest.cpp \
        $$PWD/AsyncAppenderTest.cpp \
        $$PWD/ConsoleAppendersTest.cpp \
        $$PWD/FileAppenderTest.cpp \
        $$PWD/LevelsTest.cpp \
        $$PWD/MultipleThreadsTest.cpp \
        $$PWD/TestsRunner.cpp \
        $$PWD/main.cpp

HEADERS += \
    $$PWD/TestsRunner.h
