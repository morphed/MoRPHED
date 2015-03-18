#-------------------------------------------------
#
# Project created by QtCreator 2015-02-19T10:31:27
#
#-------------------------------------------------

QT       += core gui xml concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = MoRPHED_GUI
TEMPLATE = app

SOURCES += main.cpp\
        gui_mainwindow.cpp \
    dialog_delftparams.cpp \
    dialog_morphparams.cpp \
    dialog_inputs.cpp \
    qcustomplot.cpp

HEADERS  += gui_mainwindow.h \
    dialog_delftparams.h \
    dialog_morphparams.h \
    dialog_inputs.h \
    qcustomplot.h

FORMS    += gui_mainwindow.ui \
    dialog_delftparams.ui \
    dialog_morphparams.ui \
    dialog_inputs.ui


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../MoRPHED_LIB/release/ -lMoRPHED_LIB
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../MoRPHED_LIB/debug/ -lMoRPHED_LIB

INCLUDEPATH += $$PWD/../MoRPHED_LIB
DEPENDPATH += $$PWD/../MoRPHED_LIB

win32: LIBS += -L$$PWD/../../../../../../MinGW/msys/1.0/local/lib/ -llibgdal

INCLUDEPATH += $$PWD/../../../../../../MinGW/msys/1.0/local/include
DEPENDPATH += $$PWD/../../../../../../MinGW/msys/1.0/local/include
