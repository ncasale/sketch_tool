#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H


#include <QOpenGLWidget>
#include <QCoreApplication>
#include <OpenGLFunctions.h>
#include <QOpenGLShaderProgram>
#include <QTime>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QtCore>
#include <QtGui>
#include "View.h"

/*
 * This is the main OpenGL-based window in our application
 * The QOpenGLWindow class exposes some convenient functionality for any OpenGL program to override
 *
*/

class MyGLWidget : public QOpenGLWidget
{
    Q_OBJECT
    public:
        explicit MyGLWidget(QWidget *parent=0);
        virtual ~MyGLWidget();
        void setAnimating(bool enabled);
        void saveFile();

    protected:
        //OpenGL Functions
        void initializeGL();
        void paintGL();
        //void paintOverGL();
        void resizeGL(int w,int h);

        //Mouse/Keyboard Events
        void mouseMoveEvent(QMouseEvent *);
        void mousePressEvent(QMouseEvent *);
        void mouseReleaseEvent(QMouseEvent *);
        void keyPressEvent(QKeyEvent *);




        /*
         * this class encapsulates all our program-specific rendering.
         * This provides a clean divide from Qt-specific window setup and
         * events and our OpenGL stuff. Thus we would need to mostly change
         * the View class from one application to another. That is, to create a
         * new application, retain this class as-is and change the View class
         */
        View view;


        /*
         * normally OpenGL is for the C language (i.e. no objects). Thus all
         * Opengl functions can be called directly provided we include
         * the header files.

         * With newer modern versions of opengl, this gets messy. This is
         * because (a) we must have the correct version of the header file that
         * has all the functions in a particular OpenGL version. (b) this correct
         * file must be included at every place where we are using OpenGL
         * functions. For this simple application, that ends up being here,
         * View, ObjectInstance classes. To change the OpenGL version for any
         * reasons, all these files must be edited.
         *
         * Therefore we wrap all the OpenGL functionality in an object, which
         * we pass around. Qt has several wrapper classes, one for each version
         * of OpenGL (and even OpenGL ES). In order to make our code even
         * more independent of Qt, we create a new class OpenGLFunctions. For
         * now we make it extend the QOpenGLFunctions_3_3_Core, which
         * encapsulates all the OpenGL 3.3 core functionality.
         *
         * To change the version or even to use with a different library,
         * simply change the OpenGLFunctions class to have the correct parent
         */

        util::OpenGLFunctions *gl;

private:
        bool isDragged;
        int frames;
        QTime timer;
        float framerate;
};



#endif // OPENGLWINDOW_H
