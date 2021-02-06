#-------------------------------------------------
#
# Project created by QtCreator 2020-03-05T23:15:43
#
#-------------------------------------------------

QT       += widgets core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = planeWar
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


SOURCES += \
        main.cpp \
        mainscene.cpp \
    map.cpp \
    heroplane.cpp \
    bullet.cpp \
    enemyplane.cpp \
    bomb.cpp

HEADERS += \
        mainscene.h \
    config.h \
    map.h \
    heroplane.h \
    bullet.h \
    enemyplane.h \
    bomb.h

#RESOURCES += \
#    res.qrc

INCLUDEPATH+= D:\Program_Files\Qt\Qt5.14.2\repo\air_kinect\kinect\inc
LIBS +=  -LD:\Program_Files\Qt\Qt5.14.2\repo\air_kinect\kinect\Lib\x86 -lKinect20 -lKinect20.Fusion -lKinect20.Face -lKinect20.VisualGestureBuilder# -lKinect20.lib -lKinect20.Face.lib -lKinect20.Fusion.lib  -lKinect20.VisualGestureBuilder.lib
