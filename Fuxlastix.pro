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
    ../../Fuxlastix/alglib/src/alglibinternal.cpp \
    ../../Fuxlastix/alglib/src/alglibmisc.cpp \
    ../../Fuxlastix/alglib/src/ap.cpp \
    ../../Fuxlastix/alglib/src/dataanalysis.cpp \
    ../../Fuxlastix/alglib/src/diffequations.cpp \
    ../../Fuxlastix/alglib/src/fasttransforms.cpp \
    ../../Fuxlastix/alglib/src/integration.cpp \
    ../../Fuxlastix/alglib/src/interpolation.cpp \
    ../../Fuxlastix/alglib/src/linalg.cpp \
    ../../Fuxlastix/alglib/src/optimization.cpp \
    ../../Fuxlastix/alglib/src/solvers.cpp \
    ../../Fuxlastix/alglib/src/specialfunctions.cpp \
    ../../Fuxlastix/alglib/src/statistics.cpp \
    colorcheck.cpp \
    foumel.cpp \
    gridcalc.cpp \
    main.cpp \
    fuxlastix.cpp

HEADERS += \
    ../../Fuxlastix/alglib/src/alglibinternal.h \
    ../../Fuxlastix/alglib/src/alglibmisc.h \
    ../../Fuxlastix/alglib/src/ap.h \
    ../../Fuxlastix/alglib/src/dataanalysis.h \
    ../../Fuxlastix/alglib/src/diffequations.h \
    ../../Fuxlastix/alglib/src/fasttransforms.h \
    ../../Fuxlastix/alglib/src/integration.h \
    ../../Fuxlastix/alglib/src/interpolation.h \
    ../../Fuxlastix/alglib/src/linalg.h \
    ../../Fuxlastix/alglib/src/optimization.h \
    ../../Fuxlastix/alglib/src/solvers.h \
    ../../Fuxlastix/alglib/src/specialfunctions.h \
    ../../Fuxlastix/alglib/src/statistics.h \
    ../../Fuxlastix/alglib/src/stdafx.h \
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
