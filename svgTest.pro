#-------------------------------------------------
#
# Project created by QtCreator 2018-11-21T14:28:18
#
#-------------------------------------------------

QT       += core gui svg concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = svgTest
TEMPLATE = app

QMAKE_CFLAGS_RELEASE += -Zi
QMAKE_CXXFLAGS_RELEASE += -Zi
QMAKE_LFLAGS_RELEASE += /DEBUG /OPT:REF

LIBS         += User32.lib
LIBS         += DbgHelp.lib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        widget.cpp \
    svgButton.cpp \
    HS_ImageViewUi.cpp \
    HS_FadeOutWidget.cpp

HEADERS  += widget.h \
    svgButton.h \
    exception_handler.h \
    HS_ImageViewUi.h \
    HS_FadeOutWidget.h

FORMS    += widget.ui

RESOURCES += \
    res.qrc
