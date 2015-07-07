#-------------------------------------------------
#
# Project created by QtCreator 2015-02-19T10:32:48
#
#-------------------------------------------------

QT       += xml widgets concurrent

QT       -= gui

TARGET = MoRPHED_LIB
TEMPLATE = lib

DEFINES += MORPHED_LIB_LIBRARY

SOURCES += morphed_lib.cpp \
    xmlreadwrite.cpp \
    morph_delft3dio.cpp \
    morph_base.cpp \
    morph_raster.cpp \
    morph_pathlengthdist.cpp \
    renderer.cpp \
    renderer_bytedata.cpp \
    renderer_classified.cpp \
    renderer_gcderror.cpp \
    renderer_gcdptdens.cpp \
    renderer_gcdslopedeg.cpp \
    renderer_gcdslopeper.cpp \
    renderer_stretchminmax.cpp \
    renderer_stretchstddev.cpp \
    morph_filemanager.cpp \
    morph_sedimenttransport.cpp \
    morph_exception.cpp \
    morph_model.cpp

HEADERS += morphed_lib.h\
        morphed_lib_global.h \
    xmlreadwrite.h \
    morph_delft3dio.h \
    morph_base.h \
    morph_raster.h \
    morph_pathlengthdist.h \
    renderer.h \
    renderer_bytedata.h \
    renderer_classified.h \
    renderer_gcderror.h \
    renderer_gcdptdens.h \
    renderer_gcdslopedeg.h \
    renderer_gcdslopeper.h \
    renderer_stretchminmax.h \
    renderer_stretchstddev.h \
    morph_filemanager.h \
    morph_sedimenttransport.h \
    morph_exception.h \
    morph_model.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}



win32: LIBS += -L$$PWD/../../../../../../MinGW/msys/1.0/local/lib/ -llibgdal

INCLUDEPATH += $$PWD/../../../../../../MinGW/msys/1.0/local/include
DEPENDPATH += $$PWD/../../../../../../MinGW/msys/1.0/local/include
