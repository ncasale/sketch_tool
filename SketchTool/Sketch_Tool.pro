#-------------------------------------------------
#
# Project created by QtCreator 2017-02-07T17:05:04
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Sketch_Tool
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    View.cpp \
    MyGLWidget.cpp \

INCLUDEPATH += ../headers

HEADERS  += mainwindow.h \
    View.h \
    VertexAttrib.h \
    sgraph/AbstractNode.h \
    sgraph/GLScenegraphRenderer.h \
    sgraph/GroupNode.h \
    sgraph/INode.h \
    sgraph/IScenegraph.h \
    sgraph/LeafNode.h \
    sgraph/Scenegraph.h \
    sgraph/scenegraphinfo.h \
    sgraph/SceneXMLReader.h \
    sgraph/TransformNode.h \
    MyGLWidget.h \

FORMS    += mainwindow.ui

DISTFILES += \
    shaders/phong-multiple.frag \
    shaders/phong-multiple.vert
