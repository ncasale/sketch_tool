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
    customdialog.cpp \
    console_input.cpp \
    shape.cpp \
    circle.cpp \
    line.cpp \
    MyTreeWidget.cpp \
    MyTreeWidgetItem.cpp \
    cluster.cpp

INCLUDEPATH += ../headers

HEADERS  += mainwindow.h \
    View.h \
    VertexAttrib.h \
    sgraph/GLScenegraphRenderer.h \
    sgraph/IScenegraph.h \
    sgraph/scenegraphinfo.h \
    sgraph/SceneXMLReader.h \
    MyGLWidget.h \
    sgraph/AbstractNode.h \
    sgraph/GroupNode.h \
    sgraph/INode.h \
    sgraph/LeafNode.h \
    sgraph/Scenegraph.h \
    sgraph/TransformNode.h \
    ui_mainwindow.h \
    customdialog.h \
    console_input.h \
    shape.h \
    circle.h \
    line.h \
    MyTreeWidget.h \
    MyTreeWidgetItem.h \
    cluster.h

FORMS    += mainwindow.ui

DISTFILES += \
    shaders/phong-multiple.frag \
    shaders/phong-multiple.vert

RESOURCES += \
    myres.qrc
