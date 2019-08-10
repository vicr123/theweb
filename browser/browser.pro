#-------------------------------------------------
#
# Project created by QtCreator 2019-08-08T22:26:44
#
#-------------------------------------------------

QT       += core gui webenginecore webenginewidgets thelib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = theweb
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        bar.cpp \
        core/urlinterceptor.cpp \
        main.cpp \
        mainwindow.cpp \
        managers/profilemanager.cpp \
        popovers/jsalert.cpp \
        tab/certificateerrorpane.cpp \
        tab/permissionpopup.cpp \
        tab/webpage.cpp \
        tab/webtab.cpp

HEADERS += \
        bar.h \
        core/urlinterceptor.h \
        mainwindow.h \
        managers/profilemanager.h \
        popovers/jsalert.h \
        tab/certificateerrorpane.h \
        tab/permissionpopup.h \
        tab/webpage.h \
        tab/webtab.h

FORMS += \
        mainwindow.ui \
        popovers/jsalert.ui \
        tab/certificateerrorpane.ui \
        tab/permissionpopup.ui \
        tab/webtab.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
