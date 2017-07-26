QT += core gui  printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkwidget

TARGET = deepin-draw
TEMPLATE = app

isEmpty(PREFIX){
    PREFIX = /usr
}

HEADERS += \
    mainwindow.h \
    mainwidget.h

SOURCES += main.cpp \
    mainwindow.cpp \
    mainwidget.cpp

include(widgets/widgets.pri)
include(utils/utils.pri)
include(controller/controller.pri)
include(drawshape/drawshape.pri)

RESOURCES +=


BINDIR = $$PREFIX/bin
APPSHAREDIR = $$PREFIX/share/deepin-draw
MANDIR = $$PREFIX/share/dman/deepin-draw
MANICONDIR = $$PREFIX/share/icons/hicolor/scalable/apps
APPICONDIR = $$PREFIX/share/icons/deepin/apps/scalable

DEFINES += APPSHAREDIR=\\\"$$APPSHAREDIR\\\"

target.path = $$BINDIR

desktop.path = $${PREFIX}/share/applications/
desktop.files =  deepin-draw.desktop

manual.path = $$MANDIR
#manual.files = doc/*
manual_icon.path = $$MANICONDIR
manual_icon.files = logo/deepin-draw.svg

app_icon.path = $$APPICONDIR
app_icon.files = logo/deepin-draw.svg

# Automating generation .qm files from .ts files
#CONFIG(release, debug|release) {
#    !system($$PWD/generate_translations.sh): error("Failed to generate translation")
#}

#TRANSLATIONS += \
#   translations/deepin-draw.ts\
#    translations/deepin-draw_zh_CN.ts

#translations.path = $$APPSHAREDIR/translations
#translations.files = translations/*.qm
exists(app_icon.files) {
    message("app_icon exists")
}

INSTALLS = target desktop  app_icon manual_icon #manual   translations
