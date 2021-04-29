TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        src/main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../Logger/bin/release/ -lLogger
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../Logger/bin/debug/ -lLogger
else:unix:!macx: LIBS += -L$$PWD/../Logger/bin/ -lLogger

INCLUDEPATH += \
    $$PWD/../Logger/include

Release:DESTDIR = $$PWD/bin/release
Debug:DESTDIR = $$PWD/bin/debug
