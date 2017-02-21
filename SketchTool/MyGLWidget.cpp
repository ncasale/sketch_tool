#include "MyGLWidget.h"
#include <QScreen>
#include <OpenGLFunctions.h>
#include <QMessageBox> //requires QT += widgets in .pro file
#include <QPainter>
#include <QDebug>
#include <QStaticText>
#include <QInputDialog>
#include <iostream>
#include "customdialog.h"

#define Key_Translate Qt::Key_Shift
#define Key_Scale  Qt::Key_U
#define Key_Rotate  Qt::Key_R

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

/**
 * @brief MyGLWidget::initializeGL
 * Used to initialize the OpenGL functionality within the program. Done by
 * creating and instantiating an instangce of util::OpenGLFunctions() which
 * operates as a wrapper for OpenGL functionality
 */
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

/**
 * @brief MyGLWidget::paintGL
 * Called whenever this widget needs to be painted (drawn). For this, we
 * defer to View::draw. In addition, we can optionally use this function
 * to display other material in the widget -- for example, a framerate.
 */
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

/**
 * @brief MyGLWidget::mousePressEvent
 * Called when the mouse is clicked -- we alert the view when this happens
 *
 * @param e
 * Contains information relevant to the mouse click event such as mouse
 * location.
 */
void MyGLWidget::mousePressEvent(QMouseEvent *e)
{
    isDragged = true;
    view.mousePressed(e->x(),e->y());
}

/**
 * @brief MyGLWidget::mouseMoveEvent
 * Called when mouse is moved (even hovered)
 *
 * @param e
 * Contains information relevant to the mouse move event, such as mouse
 * location
 */
void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
    //for now, simply re-render if it is a drag
    if (isDragged)
    {
        view.mouseDragged(e->x(),e->y());
        this->update();
    }

}

/**
 * @brief MyGLWidget::mouseReleaseEvent
 * Called when mouse button is released (following a click)
 *
 * @param e
 * Contains information relevant to the mouse release event, such as mouse
 * location
 */
void MyGLWidget::mouseReleaseEvent(QMouseEvent *e)
{
    isDragged = false;
    view.mouseReleased(e->x(),e->y());
}

/**
 * @brief MyGLWidget::keyPressEvent
 * Called when a key is pressed while our widget is in focus. Functionally a
 * switch/ case statement. If new key actions are to be added, must be added to
 * switch/case
 *
 * @param e
 * Contains information relevant to key press event, such as the key pressed
 */
void MyGLWidget::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_1:
        //Call Cube creation
        view.addToScenegraph("box");
        break;
    case Qt::Key_2:
        //Call Sphere creation
        view.addToScenegraph("sphere");
        break;
    case Qt::Key_3:
        //Call Cylinder creation
        view.addToScenegraph("cylinder");
        break;
    case Qt::Key_4:
        //Call cone creation
        view.addToScenegraph("cone");
        break;
    case Qt::Key_C:
        //Clear the scenegraph -- ask user if they would like to.
        clearScene();
        break;
    case Qt::Key_T:
        //Format XML file
        view.insertTabs(view.getSgraphFileLocation());
        break;
    case Qt::Key_S:
        //Save the XML File
        saveFile();
        break;
    case Qt::Key_P:
        //Used for debug printing
        view.printNodeNames();
        break;
    case Key_Translate:
    {
        //Used for translation
        //Use custom dialog to ask for translation
        string node_name = "";
        string x_trans = "";
        string y_trans = "";
        string z_trans = "";

        CustomDialog d("Add Translation", this);
        d.addLineEdit("Node Name: ", &node_name);
        d.addLineEdit("X-axis Translation: ", &x_trans);
        d.addLineEdit("Y-axis Translation: ", &y_trans);
        d.addLineEdit("Z-axis Translation: ", &z_trans);

        d.exec();

        if(d.wasCancelled())
            break;

        //Grab our scenegraph
        sgraph::Scenegraph* scenegraph = view.getScenegraph();
        //Find node -- first check if passed name is valid
        bool valid = scenegraph->isValidNodeName(node_name);

        if(!valid)
        {
            QString msg(node_name.c_str());
            QMessageBox::information(this, "Invalid Node Name", "Invalid node name: " + msg);
            break;
        }
        else
        {
            //Convert translation values to floats
            vector<float> data;
            data.push_back(atof(x_trans.c_str()));
            data.push_back(atof(y_trans.c_str()));
            data.push_back(atof(z_trans.c_str()));

            view.addTransformNode(node_name, View::TRANSLATION, data);
        }

        break;
    }

    case Key_Scale:
    {
        //Use custom dialog to ask for Scale
        string node_name = "";
        string x_scale = "";
        string y_scale = "";
        string z_scale = "";

        CustomDialog d("Add Scale", this);
        d.addLineEdit("Node Name: ", &node_name);
        d.addLineEdit("X-axis Scale: ", &x_scale);
        d.addLineEdit("Y-axis Scale: ", &y_scale);
        d.addLineEdit("Z-axis Scale: ", &z_scale);

        d.exec();

        if(d.wasCancelled())
            break;

        //Grab our scenegraph
        sgraph::Scenegraph* scenegraph = view.getScenegraph();
        //Find node -- first check if passed name is valid
        bool valid = scenegraph->isValidNodeName(node_name);

        if(!valid)
        {
            QString msg(node_name.c_str());
            QMessageBox::information(this, "Invalid Node Name", "Invalid node name: " + msg);
            break;
        }
        else
        {
            //Convert scale values to floats
            vector<float> data;
            data.push_back(atof(x_scale.c_str()));
            data.push_back(atof(y_scale.c_str()));
            data.push_back(atof(z_scale.c_str()));

            view.addTransformNode(node_name, View::SCALE, data);
        }

        break;
    }
    case Key_Rotate:
    {
        //Use custom dialog to ask for roatation
        string node_name = "";
        bool x_axis = false;
        bool y_axis = false;
        bool z_axis = false;
        string angle = "";

        CustomDialog d("Add Rotation", this);
        d.addLineEdit("Node Name: ", &node_name);
        d.addLineEdit("Rotation Angle: ", &angle, "In Degrees.");
        d.addLabel("Select which axis/axes to rotate about:", true);
        d.addCheckBox("X-Axis: ", &x_axis);
        d.addCheckBox("Y-Axis: ", &y_axis);
        d.addCheckBox("Z-Axis: ", &z_axis);

        d.exec();

        if(d.wasCancelled())
            break;

        //Grab our scenegraph
        sgraph::Scenegraph* scenegraph = view.getScenegraph();
        //Find node -- first check if passed name is valid
        bool valid = scenegraph->isValidNodeName(node_name);

        if(!valid)
        {
            QString msg(node_name.c_str());
            QMessageBox::information(this, "Invalid Node Name", "Invalid node name: " + msg);
            break;
        }
        else
        {
            //Convert rotation values to floats
            vector<float> data;
            data.push_back(atof(angle.c_str()));
            data.push_back((float)x_axis);
            data.push_back((float)y_axis);
            data.push_back((float)z_axis);
            view.addTransformNode(node_name, View::ROTATION, data);
        }
        break;
    }


    }
}


/**
 * @brief MyGLWidget::resizeGL
 * Called when the GLWidget is resized. For our purposes, we delegate this
 * resizing the the View
 *
 * @param w
 * The new width of the GLWidget
 *
 * @param h
 * The new height of the GLWidget
 */
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

/**
 * @brief MyGLWidget::setAnimating
 * This function helps us to automatically start animating. When we call this
 * function with "true", it sets up the window so that it calls update()
 * continuously and automatically.
 *
 * @param enabled
 * When "true", widget continuously and automatically updates. Does not,
 * otherwise
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

/**
 * @brief MyGLWidget::saveFile
 * Called when Save action is executed from File menu or Main Toolbar.
 * Saves file to save location
 */
void MyGLWidget::saveFile()
{
    view.saveXMLFile("scenegraphs/test_save.xml");
    view.insertTabs("scenegraphs/test_save.xml");

    QMessageBox::information(this, "File Saved", "File successfully saved.");
}

/**
 * @brief MyGLWidget::clearScene
 * Clears the current scene -- Asks user permission beforehand.
 */
void MyGLWidget::clearScene()
{
    //Parameters for popup message box
    const QString popup_title = "Clear Scene?";
    const QString popup_msg = "Clear Scene? (This cannot be undone)";

    //Ask user to confirm their clear of scene
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, popup_title, popup_msg,
                                  QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::Yes)
        view.clearScenegraph();
    else
        return;
}





