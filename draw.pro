QT += core gui  printsupport svg dbus concurrent dtkwidget dtkgui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

  qtHaveModule(opengl): QT += opengl

CONFIG += c++11 link_pkgconfig
PKGCONFIG += libexif
LIBS += -lfreeimage

TARGET = deepin-draw
TEMPLATE = app

isEmpty(PREFIX){
    PREFIX = /usr
}

include(./qtsingleapplication/qtsingleapplication.pri)

HEADERS += \
    application.h

SOURCES += main.cpp \
    application.cpp

include(utils/utils.pri)
include(widgets/widgets.pri)
include(drawshape/drawshape.pri)
include(frame/frame.pri)
include(service/service.pri)


RESOURCES += \
    icon/icons.qrc

BINDIR = $$PREFIX/bin
APPSHAREDIR = $$PREFIX/share/deepin-draw
MANDIR = $$PREFIX/share/dman/deepin-draw
MANICONDIR = $$PREFIX/share/icons/hicolor/scalable/apps
APPICONDIR = $$PREFIX/share/icons/hicolor/scalable/apps

DEFINES += APPSHAREDIR=\\\"$$APPSHAREDIR\\\"

target.path = $$BINDIR

desktop.path = $${PREFIX}/share/applications/
desktop.files =  deepin-draw.desktop

formatFile.path = $$PREFIX/share/mime/packages/
formatFile.files = service/x-ddf.xml

manual.path = $$MANDIR
#manual.files = doc/*
manual_icon.path = $$MANICONDIR
manual_icon.files = logo/deepin-draw.svg

app_icon.path = $$APPICONDIR
app_icon.files = logo/deepin-draw.svg


# Automating generation .qm files from .ts files
CONFIG(release, debug|release) {
    system($$PWD/generate_translations.sh)
}

TRANSLATIONS += $$PWD/translations/$${TARGET}.ts \
                $$PWD/translations/$${TARGET}_zh_CN.ts \
                $$PWD/translations/$${TARGET}_en_US.ts

translations.path = $$APPSHAREDIR/translations
translations.files = translations/*.qm

service.path = $${PREFIX}/share/dbus-1/services
service.files = service/com.deepin.Draw.service

mime_file.path = /usr/share/mime/packages
mime_file.files = $$PWD/mimetype/*.xml

exists(app_icon.files) {
    message("app_icon exists")
}

INSTALLS = target desktop  app_icon manual_icon formatFile translations service mime_file

DISTFILES += \
    logo/deepin-draw-16.svg \
    logo/deepin-draw-32.svg \
    logo/deepin-draw-64.svg \
    logo/deepin-draw-96.svg \
    logo/deepin-draw-128.svg \
    logo/deepin-draw-256.svg

# dbus 外部调用程序
dbus_service.files = $$PWD/com.deepin.Draw.service
dbus_service.path = $$PREFIX/share/dbus-1/services
