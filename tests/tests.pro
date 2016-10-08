TEMPLATE = app

QT += testlib
QT -= gui

CONFIG += testcase c++11

SOURCES += \
    testuefi.cpp \
    main.cpp

HEADERS += \
    testuefi.h

win32 {
    VLD_PATH = "D:/Develop/Visual Leak Detector"
    INCLUDEPATH += $$VLD_PATH/include
    LIBS += -L$$VLD_PATH/lib/Win64 -lvld
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libxefi/release/ -llibxefi
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libxefi/debug/ -llibxefi
else:unix: LIBS += -L$$OUT_PWD/../libxefi/ -llibxefi

INCLUDEPATH += $$PWD/../libxefi/include
DEPENDPATH += $$PWD/../libxefi/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libxefi/release/liblibxefi.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libxefi/debug/liblibxefi.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libxefi/release/libxefi.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libxefi/debug/libxefi.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libxefi/liblibxefi.a
