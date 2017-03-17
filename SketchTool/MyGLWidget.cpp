#include "MyGLWidget.h"
#include <QScreen>
#include <OpenGLFunctions.h>
#include <QMessageBox> //requires QT += widgets in .pro file
#include <QPainter>
#include <QDebug>
#include <QStaticText>
#include <QInputDialog>
#include <QPlainTextEdit>
#include <iostream>
#include <QFileDialog>
#include "customdialog.h"
#include <QTabletEvent>
#include "glm/gtc/type_ptr.hpp"

//Key Press Event definitions
#define KEY_TRANSLATE                   Qt::Key_Shift
#define KEY_SCALE                       Qt::Key_U
#define KEY_ROTATE                      Qt::Key_R
#define KEY_X_AXIS_SELECTION            Qt::Key_X
#define KEY_Y_AXIS_SELECTION            Qt::Key_Y
#define KEY_Z_AXIS_SELECTION            Qt::Key_Z
#define KEY_ALL_AXES_SELECTION          Qt::Key_A
#define KEY_TABULATE_FILE               Qt::Key_T
#define KEY_SAVE_FILE                   Qt::Key_S
#define KEY_CLEAR                       Qt::Key_C
#define KEY_ADD_CUBE                    Qt::Key_1
#define KEY_ADD_SPHERE                  Qt::Key_2
#define KEY_ADD_CYLINDER                Qt::Key_3
#define KEY_ADD_CONE                    Qt::Key_4
#define KEY_PRINT                       Qt::Key_P
#define KEY_SELECT_OBJECT               Qt::Key_O
#define KEY_POSITIVE_TRANSLATION        Qt::Key_Right
#define KEY_NEGATIVE_TRANSLATION        Qt::Key_Left
#define KEY_POSITIVE_SCALE              Qt::Key_Up
#define KEY_NEGATIVE_SCALE              Qt::Key_Down
#define KEY_POSITIVE_ROTATION           Qt::Key_Period
#define KEY_NEGATIVE_ROTATION           Qt::Key_Comma


//Detect shape vector locations
#define ERROR                           0
#define CENTER_X                        1
#define CENTER_Y                        2
#define RADIUS                          3

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
    string axis_string = "Selected Axis: " + curr_axis_str;
    QStaticText axis_text(QString(axis_string.c_str()));
    string obj_string = "Selected Object: " + selected_node_name;
    QStaticText obj_text(QString(obj_string.c_str()));
    painter.drawStaticText(5, 20, text);
    painter.drawStaticText(5, 50, axis_text);
    painter.drawStaticText(5,80,obj_text);


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
    //Only want to move trackball when ctrl key is pressed. This allows us
    //to use a default drag and click to draw a shape
    if(QApplication::keyboardModifiers() & Qt::ControlModifier)
    {
        isDragged = true;
        view.mousePressed(e->x(),e->y());
    }
    else
    {
        //If control not pressed, want to begin our drawing detection
        return;
    }

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
    case KEY_ADD_CUBE:
        //Call Cube creation
        view.addToScenegraph("box");
        break;
    case KEY_ADD_SPHERE:
        //Call Sphere creation
        view.addToScenegraph("sphere");
        break;
    case KEY_ADD_CYLINDER:
        //Call Cylinder creation
        view.addToScenegraph("cylinder");
        break;
    case KEY_ADD_CONE:
        //Call cone creation
        view.addToScenegraph("cone");
        break;
    case KEY_CLEAR:
        //Clear the scenegraph -- ask user if they would like to.
        clearScene();
        break;
    case KEY_TABULATE_FILE:
        //Format XML file
        view.insertTabs(view.getSgraphFileLocation());
        break;
    case KEY_SAVE_FILE:
        //Save the XML File
        saveFile();
        break;
    case KEY_PRINT:
        //Used for debug printing
        view.printNodeNames();
        break;
    case KEY_X_AXIS_SELECTION:
        set_x_axis();
        break;

    case KEY_Y_AXIS_SELECTION:
        set_y_axis();
        break;
    case KEY_Z_AXIS_SELECTION:
        set_z_axis();
        break;

    case KEY_ALL_AXES_SELECTION:
        //Used to select all axes of object
        toggleAllAxesSelected();
        setSelectedAxis(NONE);
        curr_axis_str = "ALL";
        break;
    case KEY_SELECT_OBJECT:
    {
        //Used to select the current object to manipulate
        CustomDialog d("Select Node", this);
        axis_selected = false;
        curr_axis_str = "";

        d.addLineEdit("Node Name: ", &selected_node_name);

        d.exec();

        if(d.wasCancelled())
            break;

        //Grab our scenegraph
        sgraph::Scenegraph* scenegraph = view.getScenegraph();
        //Ensure node is valid
        bool valid = scenegraph->isValidNodeName(selected_node_name);

        if(!valid)
        {
            QString msg(selected_node_name.c_str());
            QMessageBox::information(this, "Invalid Node Name",
                                     "Invalid node name: " + msg);
            node_selected = false;
            break;
        }
        else
        {
            node_selected = true;
        }
    }

    case KEY_POSITIVE_TRANSLATION:
    {
        switch(selected_axis)
        {
        case X_AXIS:
            parametrizedTranslation(1.0f, 0.0f, 0.0f);
            break;
        case Y_AXIS:
            parametrizedTranslation(0.0f, 1.0f, 0.0f);
            break;
        case Z_AXIS:
            parametrizedTranslation(0.0f, 0.0f, 1.0f);
            break;
        default:
            break;
        }

        break;
    }

    case KEY_NEGATIVE_TRANSLATION:
    {
        switch(selected_axis)
        {
        case X_AXIS:
            parametrizedTranslation(-1.0f, 0.0f, 0.0f);
            break;
        case Y_AXIS:
            parametrizedTranslation(0.0f, -1.0f, 0.0f);
            break;
        case Z_AXIS:
            parametrizedTranslation(0.0f, 0.0f, -1.0f);
            break;
        default:
            break;
        }

        break;
    }

    case KEY_POSITIVE_ROTATION:
    {
        switch(selected_axis)
        {
        case X_AXIS:
            parametrizedRotation(5.0f, true, false, false);
            break;
        case Y_AXIS:
            parametrizedRotation(5.0f, false, true, false);
            break;
        case Z_AXIS:
            parametrizedRotation(5.0f, false, false, true);
            break;
        default:
            break;
        }

        break;
    }

    case KEY_NEGATIVE_ROTATION:
    {
        switch(selected_axis)
        {
        case X_AXIS:
            parametrizedRotation(-5.0f, true, false, false);
            break;
        case Y_AXIS:
            parametrizedRotation(-5.0f, false, true, false);
            break;
        case Z_AXIS:
            parametrizedRotation(-5.0f, false, false, true);
            break;
        default:
            break;
        }

        break;
    }

    case KEY_POSITIVE_SCALE:
    {
        if(all_axes_selected)
        {
            parametrizedScale(1.25f, 1.25f, 1.25f);
            break;
        }

        switch(selected_axis)
        {
        case X_AXIS:
            parametrizedScale(1.25f, 1.0f, 1.0f);
            break;
        case Y_AXIS:
            parametrizedScale(1.0f, 1.25f, 1.0f);
            break;
        case Z_AXIS:
            parametrizedScale(1.0f, 1.0f, 1.25f);
            break;
        default:
            break;
        }

        break;
    }

    case KEY_NEGATIVE_SCALE:
    {
        if(all_axes_selected)
        {
            parametrizedScale(0.8f, 0.8f, 0.8f);
            break;
        }

        switch(selected_axis)
        {
        case X_AXIS:
            parametrizedScale(0.8f, 1.0f, 1.0f);
            break;
        case Y_AXIS:
            parametrizedScale(1.0f, 0.8f, 1.0f);
            break;
        case Z_AXIS:
            parametrizedScale(1.0f, 1.0f, 0.8f);
            break;
        default:
            break;
        }

        break;
    }

    case KEY_TRANSLATE:
    {
        translationDialog();
        break;
    }

    case KEY_ROTATE:
    {
        rotationDialog();
        break;
    }

    case KEY_SCALE:
    {
        scaleDialog();
        break;
    }

   }
}

/**
 * @brief MyGLWidget::tabletEvent
 * Called when any type of tablet related event occurs. These include a stylus
 * coming near the screen, applying pressure to the screen, moving on the
 * screen or releasing from the screen
 *
 * @param e
 * The tablet event to be processed
 */
void MyGLWidget::tabletEvent(QTabletEvent *e)
{
    //Switch to see what type of event we're dealing with
    switch(e->type())
    {
    case QEvent::TabletPress:
        //If control key pressed, want to move screen instead of drawing shape
        if(QApplication::keyboardModifiers() & Qt::ControlModifier)
        {
            this->isDragged = true;
            view.mousePressed(e->x(),e->y());
            break;
        }
        else
        {
            //This is a draw event
            draw_started = true;
            mouse_path.push_back(e->posF());
            break;
        }

    case QEvent::TabletMove:
        //If we are currently drawing a path, add position to the path
        if(draw_started)
        {
            mouse_path.push_back(e->posF());
            break;
        }

    case QEvent::TabletRelease:
        //Once stylus is released, stop tracking positions
        draw_started = false;

        //Do shape recognition
        pair<DrawnShape,vector<float>> shape_pair = determineShape();

        //Can implement determineShape so that it returns a pair denoting
        //the shape and the center for drawing
        if(shape_pair.first == DrawnShape::CIRCLE)
        {

            //TODO: Create/Call circle draw function
            //Call function to draw a circle of the correct radius starting
            //at the center returned


        }

        break;
    }

    e->accept();
    this->update();

}

/**
 * @brief MyGLWidget::determineShape
 * Called to determine what shape, if any, is traced by the mouse positions
 * contained within MyGLWidget::mouse_path
 *
 * @return
 * Returns a pair. First value is an ennumeration stating which type of shape
 * the mouse_path traced (if any). The second value is a vector containing
 * information about the shape. The vector values are as follows:
 * [0]: Least-Squares Error
 * [1]: Center X-Coordinate
 * [2]: Center Y-Coordinate
 * [3]: Radius
 */
pair<DrawnShape,vector<float>>  MyGLWidget::determineShape()
{
    //Run list of mouse positions through each detect function, one with
    //lowest error is the shape we choose (assuming it passes some minimum
    //threshold)
    //First value of pair is error, second is the center coords
    vector<float> circle_values = detectCircle();

    float error_thresh = 9999999999.0f;

    if(circle_values[ERROR] < error_thresh)
    {

        pair<DrawnShape, vector<float>> p(DrawnShape::CIRCLE, circle_values);
        return p;
    }
    else
    {
        pair<DrawnShape, vector<float>> p(DrawnShape::NO_SHAPE, circle_values);
        return p;
    }
}

/**
 * @brief MyGLWidget::detectCircle
 * This function uses the stored mouse_path coordinates to determine whether
 * or not a circle lies upon the traced path.
 *
 * @return Returns a vector of floats. The first value in the vector is the
 * error of the circle, the second value is the x-coord of the circle center,
 * the third value is the y-coord of the circle center and the fourth value is
 * the radius of the circle
 */
vector<float> MyGLWidget::detectCircle()
{
    //Look at current mouse path and return the error as well as a pair of
    //points denoting the center of the detected circle
    float cumulative_error = 0.0f;

    //Here we will implement the math to find the error and the center of the
    //circle
    float sum_x = 0;
    float sum_y = 0;
    float sum_x_squared = 0;
    float sum_y_squared = 0;
    float sum_xy = 0;
    float n = 0;
    float sum_x_x2y2 = 0;
    float sum_y_x2y2 = 0;
    float sum_x2y2 = 0;

    for(auto pos : mouse_path)
    {
        //Calculate required summations for matrices
        sum_x += pos.x();
        sum_y += pos.y();
        sum_x_squared += pow(pos.x(), 2.0f);
        sum_y_squared += pow(pos.y(), 2.0f);
        sum_xy += pos.x() * pos.y();
        n++;
        sum_x_x2y2 += pos.x() * (pow(pos.x(), 2.0f) + pow(pos.y(), 2.0f));
        sum_y_x2y2 += pos.y() * (pow(pos.x(), 2.0f) + pow(pos.y(), 2.0f));
        sum_x2y2 += pow(pos.x(), 2.0f) + pow(pos.y(), 2.0f);


    }

    //Get determinant
    float det_arr[9] = {sum_x_squared, sum_xy, sum_x, sum_xy, sum_y_squared, sum_y, sum_x, sum_y, n};
    glm::mat3 det_mat = glm::make_mat3(det_arr);
    float det = glm::determinant(det_mat);

    //Get Da
    float det_a_arr[9] = {sum_x_x2y2, sum_y_x2y2, sum_x2y2, sum_xy, sum_y_squared, sum_y, sum_x, sum_y, n};
    glm::mat3 det_a_mat = glm::make_mat3(det_a_arr);
    float det_a = glm::determinant(det_a_mat);

    //Get Db
    float det_b_arr[9] = {sum_x_squared, sum_xy, sum_x, sum_x_x2y2, sum_y_x2y2, sum_x2y2, sum_x, sum_y, n};
    glm::mat3 det_b_mat = glm::make_mat3(det_b_arr);
    float det_b = glm::determinant(det_b_mat);

    //Get Dc
    float det_c_arr[9] = {sum_x_squared, sum_xy, sum_x, sum_xy, sum_y_squared, sum_y, sum_x_x2y2, sum_y_x2y2, sum_x2y2};
    glm::mat3 det_c_mat = glm::make_mat3(det_c_arr);
    float det_c = glm::determinant(det_c_mat);

    //Calculate A, B and C
    float A = det_a / det;
    float B = det_b / det;
    float C = det_c / det;

    float center_x = A / -2.0f;
    float center_y = B / -2.0f;
    float radius = sqrt(C + pow(center_x, 2.0f) + pow(center_y, 2.0f));

    //Will now compute cumulative error
    for(auto pos : mouse_path)
    {
        cumulative_error += pow((pow(pos.x(), 2.0f) + pow(pos.y(), 2.0f) - A*pos.x() - B*pos.y() - C),2.0f);
    }

    //Let's now return our error, the proposed center and the calculated radius of the circle
    vector<float> ret_vec = {cumulative_error, center_x, center_y, radius};
    return ret_vec;
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
    if(current_save_file == "")
        saveAs();
    else
    {
        view.saveXMLFile(current_save_file);
        view.insertTabs(current_save_file);
        QMessageBox::information(this, "File Saved", "File successfully saved.");
    }

}


/**
 * @brief MyGLWidget::saveAs
 * Implements save_as functionality for our program
 */
void MyGLWidget::saveAs()
{
    QString save_file_name = QFileDialog::getSaveFileName(this, "Save File", "scenegraphs/", tr("XML(*.xml)"));

    if(save_file_name.isEmpty())
        return;
    else
    {
        current_save_file = save_file_name.toStdString();
        saveFile();
    }
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
    {
        view.clearScenegraph();
        view.addToScenegraph("ground");
        selected_node_name = "";
        curr_axis_str = "";
        axis_selected = false;
        node_selected = false;
    }
    else
        return;
}

/**
 * @brief MyGLWidget::openFile
 * Will open a file explorer, allowing the user to select any XML file to load
 * into the current scene. This clears the current scene.
 */
void MyGLWidget::openFile()
{
    cout << "OPENING FILE..." << endl;

    QString fileName = QFileDialog::getOpenFileName(this, "Select File to Open", "scenegraphs/", tr("Scenegraph Files(*.xml)"));

    if(fileName.toStdString() != "")
    {
        std::cout << fileName.toStdString() << endl;
        view.dispose(*gl);
        view.init(*gl);
        view.initScenegraph(*gl, fileName.toStdString());
    }

}

/**
 * @brief MyGLWidget::set_x_axis
 * Used to set the x-axis as the working axis for transformations
 */
void MyGLWidget::set_x_axis()
{
    //Used to select X-axis
    setSelectedAxis(X_AXIS);
    axis_selected = true;
    all_axes_selected = false;
    curr_axis_str = "X-Axis";
}

/**
 * @brief MyGLWidget::set_y_axis
 * Used to set the y-axis as the working axis for transformations
*/
void MyGLWidget::set_y_axis()
{
    //Used to select Y-axis
    setSelectedAxis(Y_AXIS);
    axis_selected = true;
    all_axes_selected = false;
    curr_axis_str = "Y-Axis";
}

/**
 * @brief MyGLWidget::set_z_axis
 * Used to set the z-axis as the working axis for transformations
 */
void MyGLWidget::set_z_axis()
{
    //Used to select Z-axis
    setSelectedAxis(Z_AXIS);
    axis_selected = true;
    all_axes_selected = false;
    curr_axis_str = "Z-Axis";
}

/**
 * @brief MyGLWidget::translationDialog
 * When called, a dialog box appears to specify the tranlation of a particular
 * node.
 */
void MyGLWidget::translationDialog()
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
        return;

    //Grab our scenegraph
    sgraph::Scenegraph* scenegraph = view.getScenegraph();
    //Find node -- first check if passed name is valid
    bool valid = scenegraph->isValidNodeName(node_name);

    if(!valid)
    {
        QString msg(node_name.c_str());
        QMessageBox::information(this, "Invalid Node Name", "Invalid node name: " + msg);
        return;
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
}

/**
 * @brief MyGLWidget::rotationDialog
 * When called, a dialog box appears to specify the rotation of a particular
 * node
 */
void MyGLWidget::rotationDialog()
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
        return;

    //Grab our scenegraph
    sgraph::Scenegraph* scenegraph = view.getScenegraph();
    //Find node -- first check if passed name is valid
    bool valid = scenegraph->isValidNodeName(node_name);

    if(!valid)
    {
        QString msg(node_name.c_str());
        QMessageBox::information(this, "Invalid Node Name", "Invalid node name: " + msg);
        return;
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
}

/**
 * @brief MyGLWidget::scaleDialog
 * When called, a dialog box appears to specify the scale of a particular node
 */
void MyGLWidget::scaleDialog()
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
        return;

    //Grab our scenegraph
    sgraph::Scenegraph* scenegraph = view.getScenegraph();
    //Find node -- first check if passed name is valid
    bool valid = scenegraph->isValidNodeName(node_name);

    if(!valid)
    {
        QString msg(node_name.c_str());
        QMessageBox::information(this, "Invalid Node Name", "Invalid node name: " + msg);
        return;
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
}

/**
 * @brief MyGLWidget::parametrizedTranslation
 * Translates the currently selected node by the amounts specified
 *
 * @param x
 * Amount to translate node on x-axis
 *
 * @param y
 * Amount to translate node on y-axis
 *
 * @param z
 * Amount to translate node on z-axis
 */
void MyGLWidget::parametrizedTranslation(float x, float y, float z)
{
    //Translates along positive axis if selected
    if(!node_selected)
        return;

    vector<float> data = {x, y, z};
    view.addTransformNode(selected_node_name, View::TRANSLATION, data);


}

/**
 * @brief MyGLWidget::parametrizedRotation
 * Rotates the currently selected node by angle @param angle about any true axis
 *
 * @param angle
 * The angle to roatate by (degrees)
 *
 * @param x
 * True if rotation will occur about x-axis
 *
 * @param y
 * True if rotation will occur about y-axis
 *
 * @param z
 * True if rotation will occur about z-axis
 */
void MyGLWidget::parametrizedRotation(float angle, bool x, bool y, bool z)
{
    if(!node_selected)
        return;

    vector<float> data = {angle, (float)x, (float)y, (float)z};
    view.addTransformNode(selected_node_name, View::ROTATION, data);

}

/**
 * @brief MyGLWidget::parametrizedScale
 * Scales the currently selected node by the amounts specified for each axis
 *
 * @param x
 * Amount to scale by in x-axis
 *
 * @param y
 * Amount to scale by in y-axis
 *
 * @param z
 * Amount to scale by in z-axis
 */
void MyGLWidget::parametrizedScale(float x, float y, float z)
{
    if(!node_selected)
        return;

    vector<float> data = {x, y, z};
    view.addTransformNode(selected_node_name, View::SCALE, data);

}
