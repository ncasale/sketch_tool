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

//PI
#define PI                              3.1415926535

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
#define KEY_ERASE_LINES                 Qt::Key_E
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
#define KEY_INCREMENT_TRANSFORMATION    Qt::Key_Up
#define KEY_DECREMENT_TRANSFORMATION    Qt::Key_Down


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
    //painter.fillRect(0, 0, width(), height(), Qt::white);
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


    //Draw line if needed
    if(drawing_line)
    {

        drawLineTo(&painter);

    }

    //Draw current lines in lines vector
    for(auto line : lines)
    {
        drawExistingLine(&painter, line);
    }
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
        //This is a draw event
        draw_started = true;
        QPointF mouse_pos;
        mouse_pos.setX(e->x());
        mouse_pos.setY(e->y());
        mouse_path.push_back(mouse_pos);

        //Want to draw a line starting at this point
        drawing_line = true;
        last_pen_point = e->pos();
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

    if(draw_started)
    {
        //Get mouse position for mouse_path
        QPointF mouse_pos;
        mouse_pos.setX(e->x());
        mouse_pos.setY(e->y());
        mouse_path.push_back(mouse_pos);

        //Draw line
        drawing_line = true;
        curr_pen_point = e->pos();
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
    if(isDragged)
    {
        isDragged = false;
        view.mouseReleased(e->x(),e->y());
    }
    else
    {
        draw_started = false;
        curr_pen_point = e->pos();
        drawing_line = false;
        //Do shape recognition
        Circle circle = detectCircle();
        Line line = detectLine();
        DrawnShape shape_to_draw = determineShape(circle.get_error(), line.get_error());


        //Can implement determineShape so that it returns a pair denoting
        //the shape and the center for drawing
        if(shape_to_draw == CIRCLE)
        {
            //First check if it fits the bill for a cylinder, if so, draw a cylinder
            bool cylinder_detected = detectCylinder();
            if(cylinder_detected)
            {
                view.addToScenegraph("cylinder");
                lines.clear();
                clusters.clear();
                return;
            }


            //TODO: Create/Call circle draw function
            //Call function to draw a circle of the correct radius starting
            //at the center returned
            float radius_scaling = 100.0f;
            //Do some math to figure out where to place sphere on screen
            QPoint gl_geo = this->geometry().center();
            int geo_center_x = gl_geo.x();
            int geo_center_y = gl_geo.y();
            float horiz_unit = (float)geo_center_x / -8.0f;
            float vert_unit = (float)geo_center_y / -6.0f;

            float circle_center_x = (circle.get_center_x() - (float)geo_center_x) / horiz_unit;
            float circle_center_y = (circle.get_center_y() - (float)geo_center_y) / vert_unit;
            float circle_rad = circle.get_radius() / radius_scaling;
            if(circle.get_radius() > 0.1f)
            {
                vector<float> params = {circle_center_x, circle_center_y, circle_rad};
                view.addToScenegraph("sphere", params);
            }


        }
        else if(shape_to_draw == LINE)
        {
            //TODO: Create/Call line drawing function
            lines.push_back(line);

            //Add this line to a cluster
            addLineToCluster(line);

            //Detect any shapes
            bool cone_detected = detectCone();
            bool cube_detected = detectCube();

            //If any shapes are detected, draw them and clear lines/clusters
            if(cone_detected)
            {
                view.addToScenegraph("cone");
                lines.clear();
                clusters.clear();
            }
            else if(cube_detected)
            {
                view.addToScenegraph("box");
                lines.clear();
                clusters.clear();
            }

            //Cylinder detection done in above if statement -- happens after
            //a circle is drawn, not a line.


        }
    }


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
    case KEY_ERASE_LINES:
        //Erase any lines drawn
        eraseLines();
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
        string previously_selected_obj = selected_node_name;

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
            selected_node_name = "";
            break;
        }
        else
        {
            node_selected = true;
            scenegraph->selectNode(selected_node_name);

            if(scenegraph->isValidNodeName(previously_selected_obj))
                scenegraph->revertNodeTexture(previously_selected_obj);
        }
    }

    case KEY_INCREMENT_TRANSFORMATION:
    {
        //Check to see if we have a transformation selected
        if(axis_selected)
        {
            if(translate_state)
            {
                //If we are translating, simply parametrize a translation along
                //the selected axis
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
                }

                break;
            }
            else if(rotate_state)
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
                }

                break;
            }
            else if(scale_state)
            {
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
                case ALL:
                    parametrizedScale(1.25f, 1.25f, 1.25f);
                    break;
                }

                break;
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }

    }

    case KEY_DECREMENT_TRANSFORMATION:
    {
        //Check to see if we have a transformation selected
        if(axis_selected)
        {
            if(translate_state)
            {
                //If we are translating, simply parametrize a translation along
                //the selected axis
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
                }

                break;
            }
            else if(rotate_state)
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
                }

                break;
            }
            else if(scale_state)
            {
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
                case ALL:
                    parametrizedScale(0.8f, 0.8f, 0.8f);
                    break;
                }

                break;
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
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
        Circle circle = detectCircle();
        Line line = detectLine();
        DrawnShape shape_to_draw = determineShape(circle.get_error(), line.get_error());


        //Can implement determineShape so that it returns a pair denoting
        //the shape and the center for drawing
        if(shape_to_draw == CIRCLE)
        {

            //TODO: Create/Call circle draw function
            //Call function to draw a circle of the correct radius starting
            //at the center returned
            if(circle.get_radius() > 0.1f)
            {
                vector<float> params = {circle.get_center_x(), circle.get_center_y(), circle.get_radius()};
                view.addToScenegraph("sphere", params);
            }


        }
        else if(shape_to_draw == LINE)
        {
            //TODO: Create/Call line drawing function
            //Append line to lines
            lines.push_back(line);

            //Once we have enough lines, see if we have a shape
            if(lines.size() >= 2)
            {
                //detectCube();
            }

            if(lines.size() >= 3)
            {
                //detectCube();
                //detectCone();
            }

            if(lines.size() >= 4)
            {
                //detectCube();
                //detectCone();
                //detectCylinder();
            }
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
 * Returns a DrawnShape indicating which of the shapes passed (if any) has
 * been drawn by the mouse path. If no shape sufficiently matches the mouse
 * path, then this returns NO_SHAPE
 */
DrawnShape MyGLWidget::determineShape(float circle_error, float line_error)
{
    float lowest_error = 9999999999.0f;
    float error_thresh = 9999999999.0f;
    DrawnShape ret_shape = NO_SHAPE;

    if(circle_error < error_thresh && circle_error < lowest_error)
    {
        ret_shape = CIRCLE;
        lowest_error = circle_error;

    }
    else
    {
        ret_shape = LINE;
    }

    //Clear mouse path
    mouse_path.clear();

    return ret_shape;
}

/**
 * @brief MyGLWidget::detectCircle
 * This function uses the stored mouse_path coordinates to determine whether
 * or not a circle lies upon the traced path.
 *
 * @return
 * Returns a circle object containing the error, center x/y coordinates and radius
 */
Circle MyGLWidget::detectCircle()
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

    float center_x = A / 2.0f;
    float center_y = B / 2.0f;
    float radius = sqrt(C + pow(center_x, 2.0f) + pow(center_y, 2.0f));

    //Will now compute cumulative error
    for(auto pos : mouse_path)
    {
        cumulative_error += pow((pow(pos.x(), 2.0f) + pow(pos.y(), 2.0f) - A*pos.x() - B*pos.y() - C),2.0f);
    }

    //Let's now return our error, the proposed center and the calculated radius of the circle
    Circle ret_circle(cumulative_error, center_x, center_y, radius);

//    //Check to make sure that we have drawn enough of an arc to determine
//    //circle -- at least 200 degrees
//    pair<float,float> start_point(mouse_path[0].x(), mouse_path[0].y());
//    pair<float,float> end_point(mouse_path[mouse_path.size()-1].x(),
//            mouse_path[mouse_path.size()-1].y());

//   //Perform the following calculation:
//   //1. Find vector from center to start point and vector from center to end
//   //   point
//   //2. Find dot product between two vectors
//   //3. Find magnitude of each vector and multiply together to get product, p
//   //4. Divide dot product by p to get cos(angle)
//   //5. Take arccos(dot/p) to get angle
//   glm::vec2 start_vec = glm::vec2(start_point.first-center_x, start_point.second-center_y);
//   glm::vec2 end_vec = glm::vec2(end_point.first-center_x, end_point.second-center_y);

//   float angle =
//   angle *= 180.0f/PI;


//   if(angle >= 200.0f)
//       return ret_circle;
//   else
//   {
//       ret_circle.set_error(9999999999.0f);
//       return ret_circle;
//   }

    //Do span from first point to middle of path and then middle to end of path
    //add together and that should produce the desired absolute angle

    //Let's add the start/end points of the circle to clusters
    pair<float,float> start_point(mouse_path[0].x(), mouse_path[0].y());
    pair<float,float> end_point(mouse_path[mouse_path.size()-1].x(),
                                mouse_path[mouse_path.size()-1].y());

    addPointToCluster(start_point);
    addPointToCluster(end_point);

   return ret_circle;
}


/**
 * @brief MyGLWidget::detectLine
 * Iterates through mouse_path and determines a line of best fit for the
 * provided points
 *
 * @return
 * The line determined from the points
 */
Line MyGLWidget::detectLine()
{
    float xbar = 0;
    float ybar = 0;
    float xmin = 9999999999;
    float xmax = -999999999;
    float ymin = 9999999999;
    float ymax = -999999999;
    float pos_count = 0;

    //Iterate through all x/y positions and find the minimum/max
    for(auto pos : mouse_path)
    {
        //X values
        xbar += pos.x();
        if(pos.x() < xmin)
            xmin = pos.x();
        if(pos.x() > xmax)
            xmax = pos.x();

        //Y values
        ybar += pos.y();
        if(pos.y() < ymin)
            ymin = pos.y();
        if(pos.y() > ymax)
            ymax = pos.y();

    }

    //Get averages
    xbar = xbar/(float)mouse_path.size();
    ybar = ybar/(float)mouse_path.size();

    //Compute sxx, sxy, syy
    float sxx = 0;
    float sxy = 0;
    float syy = 0;

    for(auto pos : mouse_path)
    {
        sxx += ((pos.x() - xbar) * (pos.x() - xbar));
        syy += ((pos.y() - ybar) * (pos.y() - ybar));
        sxy += ((pos.x() - xbar) * (pos.y() - ybar));
    }

    float q = 2.0f * sxy/(sxx-syy);

    float theta1 = atan(q);
    float theta2 = PI + theta1;
    float theta;

    //Find which is min and which is max
    float f = (syy - sxx)*cos(theta1) - 2.0f*sxy*sin(theta1);
    if(f < 0)
    {
        //Try other one
        f = (syy-sxx)*cos(theta2) - 2.0f*sxy*sin(theta2);
        if(f < 0)
        {
            //Error
            Line l;
            l.set_error(9999999999.0f);
            return l;
        }
        else
        {
            theta = (float)(theta2/2.0f);
        }

    }
    else
    {
        theta = (float)(theta1/2.0f);
    }

    //Compute line
    float a;
    float b;
    float c;

    a = (float)(cos(theta));
    b = (float)(sin(theta));
    c = -a*xbar - b*ybar;


    //Choose the bigger range to find line start/end pos
    float start_x;
    float start_y;
    float end_x;
    float end_y;

    if((xmax-xmin) > (ymax - ymin))
    {
        start_x = xmin;
        end_x = xmax;
        start_y = (-(c + a * xmin) / b);
        end_y = (-(c + a * xmax) / b);
    }
    else
    {
        start_y = ymin;
        end_y = ymax;
        start_x = (-(c + b * ymin) / a);
        end_x  = (-(c + b * ymin) / a);
    }

    //Create line
    pair<float,float> start_position(start_x, start_y);
    pair<float,float> end_position(end_x, end_y);

    Line l;
    l.setStartPoint(start_position);
    l.setEndPoint(end_position);

    return l;

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


void MyGLWidget::setAllAxes()
{
    setSelectedAxis(SelectedAxis::ALL);
    axis_selected = true;
    all_axes_selected = true;
    curr_axis_str = "All Axes";
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

/**
 * @brief MyGLWidget::drawLineTo
 * Draws the current mouse path taken while tracing a shape to draw
 *
 * @param painter
 * The painter used to draw the line
 */
void MyGLWidget::drawLineTo(QPainter *painter)
{
    //Iterate through tracked mouse positions and draw line between them

    painter->setPen(QPen(pen_color, pen_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    //Draw line on image
    for(int i = 0; i < mouse_path.size(); i++)
    {
        if(i < mouse_path.size() - 1)
        {

            last_pen_point = mouse_path[i];
            curr_pen_point = mouse_path[i+1];
            painter->drawLine(last_pen_point, curr_pen_point);
            //Only update area around line
            //int rad = (pen_width/2) + 2;
            //update(QRect(last_pen_point, curr_pen_point).normalized().adjusted(-rad, -rad, +rad, +rad));
        }

    }
}

/**
 * @brief MyGLWidget::drawExistingLine
 * Draws a line onto the GLWidget
 *
 * @param painter
 * The painter used to draw this line
 *
 * @param l
 * The line to draw upon the widget
 */
void MyGLWidget::drawExistingLine(QPainter *painter, Line l)
{
    //Set painter pen
    painter->setPen(QPen(pen_color, pen_width + 4.0f, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QPointF start_point(l.getStartPoint().first, l.getStartPoint().second);
    QPointF end_point(l.getEndPoint().first, l.getEndPoint().second);
    painter->drawLine(start_point, end_point);
}

/**
 * @brief MyGLWidget::getScenegraph
 * Gets the scenegraph associated with the view
 *
 * @return
 * The view's scenegraph
 */
sgraph::Scenegraph* MyGLWidget::getScenegraph()
{
    return view.getScenegraph();
}

/**
 * @brief MyGLWidget::adjustCameraToSelectedNode
 * Finds the position of the selected node in the world coordinate system and
 * then moves the camera to look at this node from an elevated perspective.
 * Makes applying transformations to the node easier to visualize.
 */
void MyGLWidget::adjustCameraToSelectedNode()
{
    //Find selected node position --  for now, just going to elevate and look at from angle
    if(!node_selected)
        return;

    //Find the modelview of this particular node
    sgraph::Scenegraph* scenegraph = view.getScenegraph();
    sgraph::LeafNode* object = (sgraph::LeafNode*) scenegraph->getNodeByName(this->selected_node_name);

    if(object == nullptr)
        return;
    else
    {
        //Transform origin by modelview to get position of center of object
        //in world space
        glm::vec4 origin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        glm::mat4 modelview = object->getModelviewForDrawing();
        glm::vec4 obj_world_pos = modelview * origin;

        //Translate this position slightly away from object
        obj_world_pos = glm::vec4(obj_world_pos.x - 10.0f, obj_world_pos.y + 10.0f, obj_world_pos.z - 10.0f, 1.0f);

        //Set lookAt eye of view to calculated position
        view.setLookAtEye(glm::vec3(obj_world_pos.x, obj_world_pos.y, obj_world_pos.z));
    }

//    view.setLookAtEye(glm::vec3(-10.0f, 10.0f, -10.0f));
}

/**
 * @brief MyGLWidget::setTranslationState
 * Sets this node's transformation state to translate. Transformation state is
 * used to delegate what action pressing KEY_INCREMENT_TRANSFORMATION and
 * KEY_DECREMENT_TRANSFORMATION will do.
 */
void MyGLWidget::setTranslationState()
{

    translate_state = true;
    rotate_state = false;
    scale_state = false;

}

/**
 * @brief MyGLWidget::setRotationState
 * Sets this node's transformation state to rotate. Transformation state is
 * used to delegate what action pressing KEY_INCREMENT_TRANSFORMATION and
 * KEY_DECREMENT_TRANSFORMATION will do.
 */
void MyGLWidget::setRotationState()
{

    translate_state = false;
    rotate_state = true;
    scale_state = false;

}

/**
 * @brief MyGLWidget::setScaleState
 * Sets this node's transformation state to scale. Transformation state is
 * used to delegate what action pressing KEY_INCREMENT_TRANSFORMATION and
 * KEY_DECREMENT_TRANSFORMATION will do.
 */
void MyGLWidget::setScaleState()
{

    translate_state = false;
    rotate_state = false;
    scale_state = true;

}

/**
 * @brief MyGLWidget::setNoTransformationState
 * Sets all transformation states to false. Transformation state is
 * used to delegate what action pressing KEY_INCREMENT_TRANSFORMATION and
 * KEY_DECREMENT_TRANSFORMATION will do.
 */
void MyGLWidget::setNoTransformationState()
{
    translate_state = false;
    rotate_state = false;
    scale_state = false;
}

/**
 * @brief MyGLWidget::revertCamera
 * Reverts the camera back to its original position.
 */
void MyGLWidget::revertCamera()
{
    setNoTransformationState();
    view.setLookAtEye(glm::vec3(0.0f, 0.0f, -4.0f));
}

/**
 * @brief MyGLWidget::selectNode
 * Selects a particular node from the scenegraph. The selected node is the only
 * one who can be transformed. The texture of the selected node changes to
 * indicate that it is selected. Only one node can (currently) be selected
 * at a time.
 *
 * @param node
 * The name of the node to select
 */
void MyGLWidget::selectNode(string node)
{
    string previously_selected_obj = selected_node_name;

    //Grab our scenegraph
    sgraph::Scenegraph* scenegraph = view.getScenegraph();
    //Ensure node is valid
    bool valid = scenegraph->isValidNodeName(node);


    if(!valid)
    {
        QString msg(selected_node_name.c_str());
        QMessageBox::information(this, "Invalid Node Name",
                                 "Invalid node name: " + msg);
        node_selected = false;
        selected_node_name = "";

        return;
    }
    else
    {
        node_selected = true;
        selected_node_name = node;
        scenegraph->selectNode(selected_node_name);

        if(scenegraph->isValidNodeName(previously_selected_obj))
            scenegraph->revertNodeTexture(previously_selected_obj);
    }

}

/**
 * @brief MyGLWidget::addToCluster
 * Adds a particular line's start/end points to a cluster. If the points don't
 * fall into any existing cluster, new clusters will be created.
 *
 * @param line
 * The line whose endpoints are being put into clusters
 */
void MyGLWidget::addLineToCluster(Line line)
{
    //Frist see if line end points fall into existing clusters
    pair<float,float> start_point = line.getStartPoint();
    pair<float,float> end_point = line.getEndPoint();

    bool start_in_cluster = false;
    bool end_in_cluster = false;

    Cluster sc;
    Cluster ec;

    if(!clusters.empty())
    {
        for(auto cluster : clusters)
        {
            //Check if start point is in cluster
            if(cluster.isPointInCluster(start_point))
            {
                //If so, set flag
                start_in_cluster = true;

            }

            //Check if end point is  in cluster
            if(cluster.isPointInCluster(end_point))
            {
                //If so, set flag
                end_in_cluster = true;
            }
        }

        //If start point not in a cluster, create a new cluster around start
        if(!start_in_cluster)
        {
            sc.setOrigin(start_point);
            clusters.push_back(sc);
        }

        //If end point not in a cluster, create a new cluster around end
        if(!end_in_cluster)
        {
            ec.setOrigin(end_point);
            clusters.push_back(ec);
        }
    }
    else
    {
        //If no clusters already exist, create new ones for start and end point
        sc.setOrigin(start_point);
        ec.setOrigin(end_point);

        clusters.push_back(sc);
        clusters.push_back(ec);
    }
}

/**
 * @brief MyGLWidget::addPointToCluster
 * Add a passed point to a cluster. If point does not fall into existing
 * clusters, create a new cluster centered at the point.
 *
 * @param point
 * The point which we are going to add to a cluster.
 */
void MyGLWidget::addPointToCluster(pair<float,float> point)
{
    bool in_cluster = false;

    Cluster c;

    if(!clusters.empty())
    {
        for(auto cluster : clusters)
        {
            if(cluster.isPointInCluster(point))
                in_cluster = true;
        }

        if(!in_cluster)
        {
            c.setOrigin(point);
            clusters.push_back(c);
        }
    }
    else
    {
        //If no clusters already exists, create new one
        c.setOrigin(point);
        clusters.push_back(c);
    }
}

/**
 * @brief MyGLWidget::detectCone
 * Detects whether or not the lines/clusters on screen represent a triangle.
 * Done by detecting if there are exactly 3 lines drawn and exactly 3 clusters.
 *
 * @return
 * True if triangle detected, false otherwise.
 */
bool MyGLWidget::detectCone()
{
    //Want to make sure there are exactly 3 lines
    if(lines.size() != 3)
        return false;

    //Also want to make sure there are exactly 3 clusters
    if(clusters.size() != 3)
        return false;

    //TODO:Check line orientations

    //Otherwise, this is a triangle (cone)
    return true;
}

/**
 * @brief MyGLWidget::detectCube
 * Detects whether or not the lines/clusters on screen represent a square.
 * Done by detecting if there are exactly 4 lines drawn and exactly 4 clusters.
 *
 * @return
 * True if square detected, false otherwise.
 */
bool MyGLWidget::detectCube()
{
    //Want to make sure there are exactly 4 lines
    if(lines.size() != 4)
        return false;

    //Also want to make sure there are exactly 4 clusters
    if(clusters.size() != 4)
        return false;

    //TODO:Check line orientation

    //Otherwise, this is a square (cube)
    return true;
}

/**
 * @brief MyGLWidget::detectCylinder
 * Will be implemented in the future.
 *
 * @return
 */
bool MyGLWidget::detectCylinder()
{
    //Detect that there is exactly 1 line drawn and 1 circle
    if(lines.size()!=1)
        return false;

    //Will need two clusters
    if(clusters.size() != 2)
        return false;


    return true;
}

/**
 * @brief MyGLWidget::eraseLines
 * Used to erase any lines drawn on the screen while sketching
 */
void MyGLWidget::eraseLines()
{
    //Clear lines vector
    lines.clear();
}
