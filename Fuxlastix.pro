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
    main.cpp \
    fuxlastix.cpp

HEADERS += \
    fuxlastix.h

FORMS += \
    fuxlastix.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../Software/elastix_new/lib/release/ -lANNlib-5.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../Software/elastix_new/lib/debug/ -lANNlib-5.0
else:unix: LIBS += -L$$PWD/../../Software/elastix_new/lib/ -lANNlib-5.0

INCLUDEPATH += $$PWD/../../Software/elastix_new
DEPENDPATH += $$PWD/../../Software/elastix_new

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/tiff-4.1.0/libtiff/.libs/release/ -ltiff
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/tiff-4.1.0/libtiff/.libs/debug/ -ltiff
else:unix: LIBS += -L$$PWD/tiff-4.1.0/libtiff/.libs/ -ltiff

INCLUDEPATH += $$PWD/tiff-4.1.0/libtiff
DEPENDPATH += $$PWD/tiff-4.1.0/libtiff

RESOURCES += \
    Fuxlastix.qrc
