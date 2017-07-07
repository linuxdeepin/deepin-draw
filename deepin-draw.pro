QT += core gui   printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkwidget dtkutil dtkbase

TARGET = deepin-draw
TEMPLATE = app


HEADERS += \
    mainwindow.h

SOURCES += main.cpp \
    mainwindow.cpp
