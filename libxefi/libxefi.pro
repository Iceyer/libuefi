#-------------------------------------------------
#
# Project created by QtCreator 2014-09-30T10:21:17
#
#-------------------------------------------------

QT -= gui core

TARGET = libxefi
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    src/Firmware.cpp \
    src/Utils.cpp \
    src/BootOrder.cpp \
    src/GUID.cpp \
    src/BootOption.cpp

HEADERS += \
    src/Firmware.h \
    src/BootOrder.h \
    src/Type.h \
    src/GUID.h \
    include/Firmware \
    src/BootOption.h \
    src/Protocol.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
