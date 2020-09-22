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
    alglib/src/alglibinternal.cpp \
    alglib/src/alglibmisc.cpp \
    alglib/src/ap.cpp \
    alglib/src/dataanalysis.cpp \
    alglib/src/diffequations.cpp \
    alglib/src/fasttransforms.cpp \
    alglib/src/integration.cpp \
    alglib/src/interpolation.cpp \
    alglib/src/linalg.cpp \
    alglib/src/optimization.cpp \
    alglib/src/solvers.cpp \
    alglib/src/specialfunctions.cpp \
    alglib/src/statistics.cpp \
    colorcheck.cpp \
    foumel.cpp \
    gridcalc.cpp \
    main.cpp \
    fuxlastix.cpp

HEADERS += \
    alglib/src/alglibinternal.h \
    alglib/src/alglibmisc.h \
    alglib/src/ap.h \
    alglib/src/dataanalysis.h \
    alglib/src/diffequations.h \
    alglib/src/fasttransforms.h \
    alglib/src/integration.h \
    alglib/src/interpolation.h \
    alglib/src/linalg.h \
    alglib/src/optimization.h \
    alglib/src/solvers.h \
    alglib/src/specialfunctions.h \
    alglib/src/statistics.h \
    alglib/src/stdafx.h \
    colorcheck.h \
    foumel.h \
    fuxlastix.h \
    gridcalc.h

FORMS += \
    colorcheck.ui \
    foumel.ui \
    fuxlastix.ui \
    gridcalc.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Fuxlastix.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/release/ -ltiff
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../usr/local/lib/debug/ -ltiff
else:unix: LIBS += -L$$PWD/../../../../usr/local/lib/ -ltiff

INCLUDEPATH += $$PWD/../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../usr/local/include
