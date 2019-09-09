QT += testlib sql network gui widgets
CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app

HEADERS += ../browser/core/safebrowsing.h
SOURCES +=  tst_safebrowsingtests.cpp

INCLUDEPATH += ../browser


unix:!macx {
    QT += thelib
}

win32 {
    QT += thelib
    INCLUDEPATH += "C:/Program Files/thelibs/include"
    LIBS += -L"C:/Program Files/thelibs/lib" -lthe-libs
}

macx {
    INCLUDEPATH += "/usr/local/include/the-libs"
    LIBS += -L/usr/local/lib -lthe-libs
}
