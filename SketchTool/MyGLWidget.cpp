#include "MyGLWidget.h"
#include <QScreen>
#include <OpenGLFunctions.h>
#include <QMessageBox> //requires QT += widgets in .pro file
#include <QPainter>
#include <QDebug>
#include <QStaticText>
#include <iostream>


MyGLWidget::MyGLWidget(QWidget *parent)
    :QOpenGLWidget(parent)
{

    //make sure we have OpenGL 3.3 (major.minor), with 16-bit buffers
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setProfile(QSurfaceFormat::CoreProfile);

    //Added to allow for Keyboard Inputs
    this->setFocusPolicy(Qt::StrongFocus);

    this->setFormat(format);

    isDragged = false;
    frames = 0;
    setAnimating(true);


}


MyGLWidget::~MyGLWidget()
{
    //When this window is called, we must release all opengl resources
    view.dispose(*gl);
}

void MyGLWidget::initializeGL()
{
    //create the opengl function wrapper class
    gl = new util::OpenGLFunctions();
    try
    {
        view.init(*gl);
    }
    catch (exception& e)
    {
        //if something goes wrong, show an error message in a popup.
        //look at the comment in the #include above for this to work correctly
        QMessageBox msgBox;
        msgBox.setText(e.what());
        msgBox.exec();
        //assuming we cannot recover from this error, shut down the application
        exit(1);
    }

    view.initScenegraph(*gl,string("scenegraphs/sketch.xml"));
}

void MyGLWidget::paintGL()
{
    //simply delegate to the view's draw
    view.draw(*gl);

    //measure frame rate

    if (this->frames==0)
    {
        this->timer.start();
    }
    this->frames++;
    if (this->frames>100)
    {
        framerate = this->frames/((float)this->timer.restart()/1000.0f);
        this->frames = 0;
    }

    //display frame rate as text

    QPainter painter(this);
   // painter.fillRect(0, 0, width(), height(), Qt::white);
    painter.setPen(QColor(255, 0, 0));
    painter.setFont(QFont("Sans", 12));
    QStaticText text(QString("Frame rate: %1 fps").arg(framerate));
    painter.drawStaticText(5, 20, text);

}

void MyGLWidget::mousePressEvent(QMouseEvent *e)
{
    isDragged = true;
    view.mousePressed(e->x(),e->y());
}


void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
    //for now, simply re-render if it is a drag
    if (isDragged)
    {
        view.mouseDragged(e->x(),e->y());
        this->update();
    }

}

void MyGLWidget::mouseReleaseEvent(QMouseEvent *e)
{
    isDragged = false;
    view.mouseReleased(e->x(),e->y());
}


void MyGLWidget::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_1:
        //Call Cube creation
        std::cout << "Pressed 1..." << endl;
        view.createShape("box");
        view.recreateScenegraph(*gl, string("scenegraphs/sketch.xml"));
        break;
    case Qt::Key_2:
        //Call Sphere creation
        view.createShape("sphere");
        view.recreateScenegraph(*gl, string("scenegraphs/sketch.xml"));
        break;
    case Qt::Key_3:
        //Call Cylinder creation
        view.createShape("cylinder");
        view.recreateScenegraph(*gl, string("scenegraphs/sketch.xml"));
        break;
    case Qt::Key_4:
        //Call cone creation
        view.createShape("cone");
        view.recreateScenegraph(*gl, string("scenegraphs/sketch.xml"));
        break;
    case Qt::Key_C:
        //Clear the scenegraph
        view.clearScenegraph();
        view.recreateScenegraph(*gl, string("scenegraphs/sketch.xml"));
        break;
    case Qt::Key_T:
        //Format XML file
        view.insertTabs(view.getSgraphFileLocation());
        break;
    case Qt::Key_S:
        //Save the XML Fi"le
        view.saveXMLFile("scenegraphs/test_save.xml");
        view.insertTabs("scenegraphs/test_save.xml");
        break;
    }
}



void MyGLWidget::resizeGL(int w,int h)
{
    //simply delegate to the view's reshape
    view.reshape(*gl,w,h);
}



/*
 * This function helps us to automatically start animating
 * When we call this function with "true", it sets up the window so
 * that it calls update() again and again automatically
 */

void MyGLWidget::setAnimating(bool enabled)
{
    if (enabled) {
        // Animate continuously, throttled by the blocking swapBuffers() call the
        // QOpenGLWindow internally executes after each paint. Once that is done
        // (frameSwapped signal is emitted), we schedule a new update. This
        // obviously assumes that the swap interval (see
        // QSurfaceFormat::setSwapInterval()) is non-zero.
        connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
        update();
    } else {
        disconnect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
    }
}




