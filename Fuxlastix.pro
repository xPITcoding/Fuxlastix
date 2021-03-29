QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    alglibinternal.cpp \
    alglibmisc.cpp \
    ap.cpp \
    definepathdlg.cpp \
    fasttransforms.cpp \
    foumel.cpp \
    fuxlastix.cpp \
    gridcalc.cpp \
    linalg.cpp \
    main.cpp \
    previewdialog.cpp \
    tool_functions.cpp

HEADERS += \
    alglibinternal.h \
    alglibmisc.h \
    ap.h \
    definepathdlg.h \
    previewdialog.h \
    stdafx.h \
    fasttransforms.h \
    foumel.h \
    fuxlastix.h \
    gridcalc.h \
    linalg.h \
    tool_functions.h


FORMS += \
    definepathdlg.ui \
    foumel.ui \
    fuxlastix.ui \
    gridcalc.ui \
    previewdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Fuxlastix.qrc \
    Fuxlastix.qrc


DISTFILES += \
    Fuxlastix.pro.user \
    Fuxlastix.pro.user.0915135 \
    Fuxlastix.pro.user.ed0e666 \
    fuxlastix_logo.png \
    libANNlib-5.0.so \
    libANNlib-5.0.so.1 \
    logo.png



