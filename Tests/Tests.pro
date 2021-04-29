TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

include ($$PWD/../Logger/Logger.pri)

SOURCES += \
        ArgsToStringTest.cpp \
        AsyncAppenderQueueOverflowTest.cpp \
        AsyncAppenderTest.cpp \
        ConsoleAppendersTest.cpp \
        FileAppenderTest.cpp \
        LevelsTest.cpp \
        MultipleThreadsTest.cpp \
        TestsRunner.cpp \
        main.cpp

HEADERS += \
    TestsRunner.h
