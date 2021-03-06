#-------------------------------------------------
#
# Project created by QtCreator 2019-08-08T22:26:44
#
#-------------------------------------------------

QT       += core gui webenginecore webenginewidgets network sql
SHARE_APP_NAME = theweb

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

# Safe Browsing
SAFE_BROWSING_KEY = $$(THEWEB_SAFE_BROWSING_API_KEY)
if (isEmpty(SAFE_BROWSING_KEY)) {
    message("Safe Browsing API key not found in environment variables.");
    message("Set the environment variable THEWEB_SAFE_BROWSING_API_KEY to the Google Safe Browsing API Key to enable it.");
} else {
    DEFINES += "SAFE_BROWSING_API_KEY=\\\"$${SAFE_BROWSING_KEY}\\\""
    message("Found Safe Browsing API Key");
}

unix:!macx {
    # Include the-libs build tools
    include(/usr/share/the-libs/pri/buildmaster.pri)

    QT += thelib
    TARGET = theweb

    target.path = /usr/bin

    desktop.path = /usr/share/applications
    desktop.files = com.vicr123.theweb.desktop

    icon.path = /usr/share/icons/hicolor/scalable/apps/
    icon.files = icons/theweb.svg

    INSTALLS += target desktop icon
}

win32 {
    # Include the-libs build tools
    include(C:/Program Files/thelibs/pri/buildmaster.pri)

    QT += thelib
    INCLUDEPATH += "C:/Program Files/thelibs/include"
    LIBS += -L"C:/Program Files/thelibs/lib" -lthe-libs
#    RC_FILE = icon.rc
    TARGET = theWeb
}

macx {
    # Include the-libs build tools
    include(/usr/local/share/the-libs/pri/buildmaster.pri)

    QT += macextras
    LIBS += -framework CoreFoundation -framework AppKit

    blueprint {
        TARGET = "theWeb Blueprint"
        ICON = icon-bp.icns
    } else {
        TARGET = "theWeb"
        ICON = icon.icns
    }

    INCLUDEPATH += "/usr/local/include/the-libs"
    LIBS += -L/usr/local/lib -lthe-libs

    QMAKE_POST_LINK += $$quote(cp $${PWD}/dmgicon.icns $${PWD}/app-dmg-background.png $${PWD}/node-appdmg-config*.json $${OUT_PWD})

    SOURCES += mainwindow-objc.mm
}

SOURCES += \
        core/safebrowsing.cpp \
        core/thewebschemehandler.cpp \
        core/urlinterceptor.cpp \
        main.cpp \
        mainwindow.cpp \
        managers/downloadmanager.cpp \
        managers/featuremanager.cpp \
        managers/historymanager.cpp \
        managers/iconmanager.cpp \
        managers/profilemanager.cpp \
        managers/settingsmanager.cpp \
        managers/singleinstancemanager.cpp \
        popovers/httpauthentication.cpp \
        popovers/jsalert.cpp \
        popovers/jsconfirm.cpp \
        popovers/jsprompt.cpp \
        tab/certificateerrorpane.cpp \
        tab/findinpage.cpp \
        tab/permissionpopup.cpp \
        tab/webpage.cpp \
        tab/webtab.cpp \
        widgets/bar.cpp \
        widgets/devtoolsheader.cpp \
        widgets/downloads/downloadspopover.cpp \
        widgets/downloads/downloadspopoveritem.cpp \
        widgets/securitychunk.cpp \
        widgets/tabbutton.cpp \
        widgets/toolbar.cpp \
        widgets/toolbar/barautocomplete.cpp \
        widgets/toolbar/securityinfo.cpp \
        widgets/toolbar/securityinfopermissionswitch.cpp \
        widgets/toolbarbutton.cpp

HEADERS += \
        core/safebrowsing.h \
        core/thewebschemehandler.h \
        core/urlinterceptor.h \
        mainwindow.h \
        managers/downloadmanager.h \
        managers/featuremanager.h \
        managers/historymanager.h \
        managers/iconmanager.h \
        managers/profilemanager.h \
        managers/settingsmanager.h \
        managers/singleinstancemanager.h \
        popovers/httpauthentication.h \
        popovers/jsalert.h \
        popovers/jsconfirm.h \
        popovers/jsprompt.h \
        tab/certificateerrorpane.h \
        tab/findinpage.h \
        tab/permissionpopup.h \
        tab/webpage.h \
        tab/webtab.h \
        widgets/bar.h \
        widgets/devtoolsheader.h \
        widgets/downloads/downloadspopover.h \
        widgets/downloads/downloadspopoveritem.h \
        widgets/securitychunk.h \
        widgets/tabbutton.h \
        widgets/toolbar.h \
        widgets/toolbar/barautocomplete.h \
        widgets/toolbar/securityinfo.h \
        widgets/toolbar/securityinfopermissionswitch.h \
        widgets/toolbarbutton.h

FORMS += \
        mainwindow.ui \
        popovers/httpauthentication.ui \
        popovers/jsalert.ui \
        popovers/jsconfirm.ui \
        popovers/jsprompt.ui \
        tab/certificateerrorpane.ui \
        tab/findinpage.ui \
        tab/permissionpopup.ui \
        tab/webtab.ui \
        widgets/devtoolsheader.ui \
        widgets/downloads/downloadspopover.ui \
        widgets/downloads/downloadspopoveritem.ui \
        widgets/securitychunk.ui \
        widgets/toolbar.ui \
        widgets/toolbar/securityinfo.ui \
        widgets/toolbar/securityinfopermissionswitch.ui

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

DISTFILES += \
    com.vicr123.theweb.desktop
