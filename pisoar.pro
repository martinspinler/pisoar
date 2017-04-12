#-------------------------------------------------
#
# Project created by QtCreator 2016-01-09T13:25:52
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = pisoar
TEMPLATE = app


SOURCES += main.cpp\
        pisoar.cpp \
    image.cpp \
    database.cpp \
    kasuar.cpp \
    layout.cpp \
    mainwindow.cpp \
    settings.cpp \
    jaguar.cpp \
    layoutview.cpp \
    factory.cpp \
    database/objectitem.cpp \
    database/imagefile.cpp \
    database/layoutitem.cpp \
    database/layoutpage.cpp \
    database/objectview.cpp \
    database/objectimage.cpp \
    database/events.cpp \
    jaguarview.cpp \
    layoutviewfactory.cpp

HEADERS  += pisoar.h \
    image.h \
    database.h \
    kasuar.h \
    layout.h \
    mainwindow.h \
    settings.h \
    jaguar.h \
    layoutview.h \
    factory.h \
    database/objectitem.h \
    database/imagefile.h \
    database/layoutitem.h \
    database/layoutpage.h \
    database/objectview.h \
    database/objectimage.h \
    database/events.h \
    database/common.h \
    jaguarview.h \
    layoutviewfactory.h

RESOURCES += \
    resources.qrc
