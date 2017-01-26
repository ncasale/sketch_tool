#ifndef VIEW_H
#define VIEW_H

#include "OpenGLFunctions.h"
#include <exception>
using namespace std;
#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "sgraph/Scenegraph.h"
#include "ShaderLocationsVault.h"
#include "ObjectInstance.h"
#include "VertexAttrib.h"
#include "sgraph/GLScenegraphRenderer.h"
#include <stack>
#include <fstream>
using namespace std;

/*
 * This class encapsulates all our program-specific details. This makes our
 * design better if we wish to port it to another C++-based windowing
 * library
 */

class View
{
public:
    View();
    ~View();
    /*
     * This is called when the application is being initialized. We should
     * do all our initializations here. This is also the first function where
     * OpenGL commands will work (i.e. don't do any OpenGL related stuff in the
     * constructor!)
     */
    void init(util::OpenGLFunctions& e) throw(runtime_error);

    //Scenegraph Functions
    void initScenegraph(util::OpenGLFunctions& e,const string& in) throw(runtime_error);
    void recreateScenegraph(util::OpenGLFunctions&, const string&) throw(runtime_error);
    /*
     * This function is called whenever the window is to be redrawn
     */
    void draw(util::OpenGLFunctions& e);

    /*
     * This function is called whenever the window is reshaped
     */
    void reshape(util::OpenGLFunctions& gl,int width,int height);

    /*
     * This function is called whenever the window is being destroyed
     */
    void dispose(util::OpenGLFunctions& gl);

    //Mouse Functions
    void mousePressed(int x,int y);
    void mouseReleased(int x,int y);
    void mouseDragged(int x,int y);

    //Shape Creation Functions
    void createShape(string shape);
    string generateXML(string shape, map<string,vector<float>> attributes);
    string parseAttributes(string xml, map<string,vector<float>>);
    void insertTabs(string&);
    void clearScenegraph();



private:
    //record the current window width and height
    int WINDOW_WIDTH,WINDOW_HEIGHT;
    //the projection matrix
    glm::mat4 proj;
    //the trackball transform
    glm::mat4 trackballTransform;
    //the radius of the virtual trackball
    float trackballRadius;
    //the mouse position
    glm::vec2 mousePos;
    //the modelview matrix
    stack<glm::mat4> modelview;
    //the scene graph
    sgraph::Scenegraph *scenegraph;
    //the list of shader variables and their locations within the shader program
    util::ShaderLocationsVault shaderLocations;
   //the GLSL shader
    util::ShaderProgram program;
    sgraph::GLScenegraphRenderer renderer;

    //Scenegraph File Location
    string sgraph_file_location = "scenegraphs/sketch.xml";
    string sgraph_updated = "scenegraphs/sketch_updated.xml";
    string sgraph_default = "scenegraphs/sketch_default.xml";

    //Used for auto-tabbing of XML
    bool new_group = true;

    //Used to assign attributes to new xml objects
    map<string,vector<float>> default_attributes;
};

#endif // VIEW_H
