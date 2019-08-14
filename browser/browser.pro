#-------------------------------------------------
#
# Project created by QtCreator 2019-08-08T22:26:44
#
#-------------------------------------------------

QT       += core gui webenginecore webenginewidgets thelib network

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
        core/thewebschemehandler.cpp \
        core/urlinterceptor.cpp \
        main.cpp \
        mainwindow.cpp \
        managers/profilemanager.cpp \
        managers/settingsmanager.cpp \
        popovers/jsalert.cpp \
        popovers/jsconfirm.cpp \
        tab/certificateerrorpane.cpp \
        tab/permissionpopup.cpp \
        tab/webpage.cpp \
        tab/webtab.cpp \
        widgets/bar.cpp \
        widgets/securitychunk.cpp \
        widgets/tabbutton.cpp

HEADERS += \
        core/thewebschemehandler.h \
        core/urlinterceptor.h \
        mainwindow.h \
        managers/profilemanager.h \
        managers/settingsmanager.h \
        popovers/jsalert.h \
        popovers/jsconfirm.h \
        tab/certificateerrorpane.h \
        tab/permissionpopup.h \
        tab/webpage.h \
        tab/webtab.h \
        widgets/bar.h \
        widgets/securitychunk.h \
        widgets/tabbutton.h

FORMS += \
        mainwindow.ui \
        popovers/jsalert.ui \
        popovers/jsconfirm.ui \
        tab/certificateerrorpane.ui \
        tab/permissionpopup.ui \
        tab/webtab.ui \
        widgets/securitychunk.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../internal-pages/release/ -linternal-pages
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../internal-pages/debug/ -linternal-pages
else:unix: LIBS += -L$$OUT_PWD/../internal-pages/ -linternal-pages

INCLUDEPATH += $$PWD/../internal-pages
DEPENDPATH += $$PWD/../internal-pages

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../internal-pages/release/libinternal-pages.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../internal-pages/debug/libinternal-pages.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../internal-pages/release/internal-pages.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../internal-pages/debug/internal-pages.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../internal-pages/libinternal-pages.a
