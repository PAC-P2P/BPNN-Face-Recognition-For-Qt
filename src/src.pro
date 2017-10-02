#-------------------------------------------------
#
# Project created by QtCreator 2017-05-19T11:18:43
#
#-------------------------------------------------

QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BPNN-Face-Recognition-For-Qt
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    camera.cpp \
    imagesettings.cpp \
    imagenet.cpp \
    pgmimage.cpp \
    backprop.cpp \
    facetrain.cpp \
    trainingsetting.cpp \
    dir.cpp

HEADERS  += \
    camera.h \
    imagesettings.h \
    backprop.h \
    pgmimage.h \
    imagenet.h \
    facetrain.h \
    trainingsetting.h \
    dir.h

FORMS    += \
    camera.ui \
    imagesettings.ui \
    trainingsetting.ui

#指定生成路径
DESTDIR = $$PWD/../Release

win32 {

}

macx {

}

unix:!macx{
    QMAKE_CC    =   gcc
    QMAKE_CXX   =   g++
}
