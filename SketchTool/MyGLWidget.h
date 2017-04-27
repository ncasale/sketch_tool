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
#include <QPointF>
#include "View.h"
#include "sgraph/Scenegraph.h"
#include "sgraph/INode.h"
#include "sgraph/TransformNode.h"
#include "sgraph/LeafNode.h"
#include "sgraph/GroupNode.h"
#include <QTabletEvent>
#include <vector>
#include "shape.h"
#include "circle.h"
#include "line.h"
#include "cluster.h"

/*
 * This is the main OpenGL-based window in our application
 * The QOpenGLWindow class exposes some convenient functionality for any OpenGL program to override
 *
*/
enum SelectedAxis{
    NONE,
    X_AXIS,
    Y_AXIS,
    Z_AXIS,
    ALL
};

enum DrawnShape{
    NO_SHAPE,
    CIRCLE,
    CUBE,
    CYLINDER,
    CONE,
    LINE
};

class MyGLWidget : public QOpenGLWidget
{
    Q_OBJECT
    public:
        explicit MyGLWidget(QWidget *parent=0);
        virtual ~MyGLWidget();

        //Setters
        void setAnimating(bool enabled);
        /**
         * @brief setSelectedAxis
         * Sets which axis is currently selected -- used during transformations
         *
         * @param val
         * The currently selected axis
         */
        void setSelectedAxis(SelectedAxis val) {selected_axis = val;}

        /**
         * @brief setSelectedNodeName
         * Sets the name of the currently selected node
         *
         * @param name
         * The name of the currently selected node
         */
        void setSelectedNodeName(string name) {selected_node_name = name;}

        /**
         * @brief toggleAllAxesSelected
         * Toggles whether or not all axes are currently selected -- useful in
         * scales when all axes may need to be selected.
         */
        void toggleAllAxesSelected() {all_axes_selected = !all_axes_selected;}
        void setMousePath(vector<QPointF> p) {mouse_path = p;}

        //Getters
        float getFrameRate() {return framerate;}
        sgraph::Scenegraph* getScenegraph();
        bool isNodeSelected(){return node_selected;}

        //Actions
        void saveFile();
        void saveAs();
        void clearScene();
        void openFile();
        void eraseLines();

        //Axis commands
        void set_x_axis();
        void set_y_axis();
        void set_z_axis();
        void setAllAxes();

        //Object selection
        void selectNode(string node);

        //Transformations
        void translationDialog();
        void rotationDialog();
        void scaleDialog();
        void parametrizedTranslation(float, float, float);
        void parametrizedRotation(float, bool, bool, bool);
        void parametrizedScale(float, float, float);

        //Shape detection functions
        DrawnShape determineShape(float, float);
        Circle detectCircle();
        Line detectLine();
        bool detectCone();
        bool detectCube();
        bool detectCylinder();
        void addLineToCluster(Line);
        void addPointToCluster(pair<float,float>);

        //Draw a line while tracing shape
        void drawLineTo(QPainter *);
        void drawExistingLine(QPainter *, Line l);

        //Camera adjustment
        void adjustCameraToSelectedNode();
        void revertCamera();

        //Transformation States
        void setTranslationState();
        void setRotationState();
        void setScaleState();
        void setNoTransformationState();

    protected:
        //OpenGL Functions
        void initializeGL();
        void paintGL();
        void resizeGL(int w,int h);

        //Mouse/Keyboard Events
        void mouseMoveEvent(QMouseEvent *);
        void mousePressEvent(QMouseEvent *);
        void mouseReleaseEvent(QMouseEvent *);
        void keyPressEvent(QKeyEvent *);

        //Tablet/Shape Detection Events
        void tabletEvent(QTabletEvent *);




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
        SelectedAxis selected_axis = NONE;
        string selected_node_name = "";
        bool node_selected = false;
        bool axis_selected = false;
        bool all_axes_selected = false;
        string current_save_file = "";
        string curr_axis_str = "";

        //Used to track tablet movement
        bool draw_started = false;
        vector<QPointF> mouse_path;

        //Pen parameters for drawing
        bool drawing_line = false;
        int pen_width = 1;
        QColor pen_color = Qt::red;
        QPointF last_pen_point;
        QPointF curr_pen_point;
        QPoint line_start;
        QImage pen_image;

        //Transformation states
        bool translate_state = false;
        bool rotate_state = false;
        bool scale_state = false;

        //Used for shape detection
        vector<Line> lines;
        vector<Cluster> clusters;


};



#endif // OPENGLWINDOW_H
