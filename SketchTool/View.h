#ifndef VIEW_H
#define VIEW_H

#include "OpenGLFunctions.h"
#include <exception>
#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "ShaderLocationsVault.h"
#include "ObjectInstance.h"
#include "VertexAttrib.h"
#include <stack>
#include <fstream>
#include "sgraph/scenegraphinfo.h"
#include "sgraph/GLScenegraphRenderer.h"
#include "sgraph/Scenegraph.h"


using namespace std;

/*
 * This class encapsulates all our program-specific details. This makes our
 * design better if we wish to port it to another C++-based windowing
 * library
 */

class View
{

public:
    enum TransfromType{
        TRANSLATION,
        ROTATION,
        SCALE
    };
    //Constructor/Destructor
    View();
    ~View();

    //Getters
    string getSgraphFileLocation(){return sgraph_file_location;}
    int getGroupNodeCount(){return group_node_count;}
    int getTransformNodeCount(){return transform_node_count;}
    int getLeafNodeCount(){return leaf_node_count;}
    sgraph::Scenegraph* getScenegraph(){return scenegraph;}

    //Rendering Functions
    void init(util::OpenGLFunctions& e) throw(runtime_error);
    void draw(util::OpenGLFunctions& e);
    void reshape(util::OpenGLFunctions& gl,int width,int height);
    void dispose(util::OpenGLFunctions& gl);

    //Scenegraph Functions
    void initScenegraph(util::OpenGLFunctions& e,const string& in) throw(runtime_error);
    void addToScenegraph(string shape, vector<float> = {});
    void clearScenegraph();
    void addTransformNode(const string&, TransfromType, vector<float>&);

    //Save functions
    void saveXMLFile(string);
    void saveObjects(fstream&);
    void saveTextures(fstream&);
    void saveLights(fstream&);
    void insertTabs(string);
    bool incrementTab(string);
    bool decrementTab(string);

    //Mouse Functions
    void mousePressed(int x,int y);
    void mouseReleased(int x,int y);
    void mouseDragged(int x,int y);

    //Random Debug
    void printNodeNames()
    {
        scenegraph->printNodeNames();
    }

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

    //location of current scenegraph file
    string sgraph_file_location = "scenegraphs/sketch.xml";

    //location of default (cleared) scenegraph file
    string sgraph_default = "scenegraphs/sketch_default.xml";

    //Counts of each of the types of nodes
    int group_node_count = 0;
    int transform_node_count = 0;
    int leaf_node_count = 0;
};

#endif // VIEW_H
