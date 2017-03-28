#include "View.h"
#include "PolygonMesh.h"
#include "sgraph/ScenegraphInfo.h"
#include "sgraph/SceneXMLReader.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <cstdio>
using namespace std;

/**
 * @brief View::View
 * Default constructor. Sets window height and width, establishes trackball
 * starting parameters and resets relevant members.
 */
View::View()
{   
  WINDOW_WIDTH = WINDOW_HEIGHT = 0;
  trackballRadius = 300;
  trackballTransform = glm::mat4(1.0);
  proj = glm::mat4(1.0);
  scenegraph = NULL;
}

/**
 * @brief View::~View
 * View destructor. Deletes scenegraph when view is destroyed.
 */
View::~View()
{
  if (scenegraph!=NULL)
    delete scenegraph;
}

/**
 * @brief View::init
 * Initializes our view by identifying shaders/shader variables
 *
 * @param gl
 * A wrapper for OpenGL functionality
 */
void View::init(util::OpenGLFunctions& gl) throw(runtime_error)
{
  //do this if your initialization throws an error (e.g. shader not found,
  //some model not found, etc.
  //  throw runtime_error("Some error happened!");

  //create the shader program
  program.createProgram(gl,
                        string("shaders/phong-multiple.vert"),
                        string("shaders/phong-multiple.frag"));

  //assuming it got created, get all the shader variables that it uses
  //so we can initialize them at some point
  shaderLocations = program.getAllShaderVariables(gl);
}

/**
 * @brief View::draw
 * Main draw function. This is originally called from MyGLWidget::paintGL().
 * Applies current trackball transformation to modelview matrix and then
 * passes this modelview to the scenegraph for drawing.
 *
 * @param gl
 * Wrapper for OpenGL functionality
 */
void View::draw(util::OpenGLFunctions& gl)
{
  gl.glClearColor(1,1,1,1);
  gl.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gl.glEnable(GL_DEPTH_TEST);

  if (scenegraph==NULL)
    return;

  program.enable(gl);

  while (!modelview.empty())
    modelview.pop();

  /*
         *In order to change the shape of this triangle, we can either move the vertex positions above, or "transform" them
         * We use a modelview matrix to store the transformations to be applied to our triangle.
         * Right now this matrix is identity, which means "no transformations"
         */
  modelview.push(glm::mat4(1.0));
  modelview.top() = modelview.top() *
      glm::lookAt(glm::vec3(0.0f,0.0f,-4.0f),
                  glm::vec3(0.0f,0.0f,0.0f),
                  glm::vec3(0.0f,1.0f,0.0f)) *
      trackballTransform;


  /*
        *Supply the shader with all the matrices it expects.
        */
  gl.glUniformMatrix4fv(shaderLocations.getLocation("projection"),
                        1,
                        false,
                        glm::value_ptr(proj));

  //gl.glPolygonMode(GL.GL_FRONT_AND_BACK,GL3.GL_LINE); //OUTLINES

  scenegraph->draw(modelview);
  gl.glFlush();

  program.disable(gl);
}

/**
 * @brief View::reshape
 * This function is called whenever window is resized
 *
 * @param gl
 * Wrapper for OpenGL functionality
 *
 * @param width
 * The new width of the window
 *
 * @param height
 * The new height of the window
 */
void View::reshape(util::OpenGLFunctions& gl,int width,int height)
{
  //record the new width and height
  WINDOW_WIDTH = width;
  WINDOW_HEIGHT = height;

  /*
     * The viewport is the portion of the screen window where the drawing
     * would be placed. We want it to take up the entire area of the window
     * so we set the viewport to be the entire window.
     * Look at documentation of glViewport
     */

  gl.glViewport(0, 0, width, height);

  /*
     * This sets up the part of our virtual world that will be visible in
     * the screen window. Since this program is drawing 2D, the virtual world
     * is 2D. Thus this window can be specified in terms of a rectangle
     * Look at the documentation of glOrtho2D, which glm::ortho implements
     */

  proj = glm::perspective(glm::radians(120.0f),(float)width/height,0.1f,10000.0f);

}

/**
 * @brief View::dispose
 * Called when current window is no longer needed
 * @param gl
 * Wrapper for OpenGL functionality
 */
void View::dispose(util::OpenGLFunctions& gl)
{
  //clean up the OpenGL resources used by the object
  scenegraph->dispose();
  renderer.dispose();
  //release the shader resources
  program.releaseShaders(gl);
}

/**
 * @brief View::initScenegraph
 * Initializes the scenegraph used to render scene
 *
 * @param gl
 * Wrapper for OpenGL functionality
 *
 * @param filename
 * Name of the file from which to load the scenegraph
 */
void View::initScenegraph(util::OpenGLFunctions &gl, const string& filename) throw(runtime_error)
{
  if (scenegraph!=NULL)
    delete scenegraph;

  program.enable(gl);
  sgraph::ScenegraphInfo<VertexAttrib> sinfo;
  sinfo = sgraph::SceneXMLReader::importScenegraph<VertexAttrib>(filename);
  scenegraph = sinfo.scenegraph;

  renderer.setContext(&gl);
  map<string,string> shaderVarsToVertexAttribs;
  shaderVarsToVertexAttribs["vPosition"] = "position";
  shaderVarsToVertexAttribs["vNormal"] = "normal";
  shaderVarsToVertexAttribs["vTexCoord"] = "texcoord";
  renderer.initShaderProgram(program,shaderVarsToVertexAttribs);
  scenegraph->setRenderer<VertexAttrib>(&renderer,sinfo.meshes);

  //Get all names of nodes
  for(auto entry : scenegraph->getNodes())
  {
      scenegraph->addNodeName(entry.first);
  }

  program.disable(gl);

}

/**
 * @brief View::addToScenegraph
 * Adds a new model to the scenegraph
 *
 * @param shape
 * The type of model we want to add to scene. Options are:
 *  - "box"
 *  - "sphere"
 *  - "cylinder"
 *  - "cone"
 */
void View::addToScenegraph(string shape, vector<float> shape_params)
{
    //Start by creating a new group node
    sgraph::INode* group_node = new sgraph::GroupNode(scenegraph, "");
    //Make group node child of root to start
    scenegraph->getRoot()->addChild(group_node);


    //Now create a transform node
    sgraph::TransformNode* transform_node = new sgraph::TransformNode(scenegraph, "");

    //If we have passed a vector of shape parameters, use those to construct a shape
    float ground_width = 1000.0f;
    float ground_height = 1.0f;
    float ground_depth = 1000.0f;

    if(!shape_params.empty() && shape == "sphere")
    {
        //First value is center_x, second is center_y, third is radius (circle)
        float radius = shape_params[2];
        float center_x = shape_params[0];
        float center_y = shape_params[1];
        transform_node->addScale(radius, radius, radius);
        transform_node->addTranslation(center_x, center_y, 0.0f);
    }
    else if(shape == "ground")
    {

        transform_node->addScale(ground_width, ground_height, ground_depth);
        transform_node->addTranslation(0.0f, -5.0f, 0.0f);
    }

    //Make transfrom node a child of group node
    group_node->addChild(transform_node);

    //Now let's create leaf (object) node
    //Want to check if we're trying to create our ground
    sgraph::LeafNode* leaf_node;
    if(shape == "ground")
    {
        leaf_node = new sgraph::LeafNode("box", scenegraph, "");
        leaf_node->setTextureName("checkerboard");
        glm::mat4 tex_mat = glm::mat4(1.0f);
        tex_mat *= glm::scale(glm::mat4(1.0f), glm::vec3(ground_width/8.0f, ground_depth/8.0f, ground_height));
        leaf_node->setTextureMatrix(tex_mat);
    }
    else
    {
        leaf_node = new sgraph::LeafNode(shape, scenegraph, "");
    }

    //Material for leaf
    util::Material mat;
    mat.setAmbient(0.4f, 0.4f, 0.4f);
    mat.setDiffuse(0.8f, 0.8f, 0.8f);
    mat.setSpecular(0.8f, 0.8f, 0.8f);
    mat.setShininess(1.0f);
    //Apply material
    leaf_node->setMaterial(mat);
    //Make leaf node child of transform node
    transform_node->addChild(leaf_node);

    //Name each node according to respective counts
    string group_node_name = "group_" + to_string(group_node_count);
    string transform_node_name = "transform_" + to_string(transform_node_count);
    string leaf_node_name = "leaf_" + to_string(leaf_node_count);

    if(group_node->getName() == "")
        group_node->setName(group_node_name);
    if(transform_node->getName() == "")
        transform_node->setName(transform_node_name);
    if(leaf_node->getName() == "")
        leaf_node->setName(leaf_node_name);

    //Add these new names to the node_names member of the scenegraph
    scenegraph->addNodeName(group_node_name);
    scenegraph->addNodeName(transform_node_name);
    scenegraph->addNodeName(leaf_node_name);

    //Increment node counts
    group_node_count++;
    transform_node_count++;
    leaf_node_count++;

    return;
}



/**
 * @brief View::clearScenegraph
 * Deletes all children of scenegraph root node - effectively clearing the
 * scenegraph
 */
void View::clearScenegraph()
{
    //This will clear the scenegraph by deleting all children nodes
    //of the root
    scenegraph->getRoot()->clearChildren();
    trackballTransform = glm::mat4(1.0f);
}

/**
 * @brief View::init
 * Initializes view by configuring shaders. Also the first function in which
 * OpenGL functions will work -- don't try to use OpenGL in the constructor
 *
 * @param gl
 * Wrapper for OpenGL functionality
 */

/**
* @brief View::saveXMLFile
* Used to save the current scenegraph to an XML file. Works similar to
* scenegraph::draw function in that it recurses through graph structure
* and creates XML snippets for Objects, Textures, Lights and Nodes.
*
* @param file_name: name of file to write to
**/
void View::saveXMLFile(string file_name)
{
    //Open file for writing
    fstream output_file;
    output_file.open(file_name, fstream::out);

    //Parse scenegraph and add to file
    if(output_file.is_open())
    {
        //Add beginning scene tag
        output_file << "<scene>" << endl;
        //Add all objects
        saveObjects(output_file);
        //Add all textures
        saveTextures(output_file);
        //Add all lights -- cannot handle lights attached to nodes
        saveLights(output_file);
        //Add all nodes in sgraph
        scenegraph->saveToXML(output_file);
        //Add end scene tag
        output_file << "</scene>";
    }
}

/**
 * @brief View::addTransformNode
 * Will add a transform node to the View's scenegraph and set its child
 * to be the node named by @param name
 *
 * @param name
 * The name of the node to which we want to add a transform node
 */
void View::addTransformNode(const string& name, View::TransfromType type, vector<float>& data)
{

    //Traverse scenegraph to find node
    sgraph::INode* node = scenegraph->getRoot()->getNode(name);
    //If parent is TransformNode, add translation to it
    if(node->getParent()->getNodeType() == sgraph::TRANSFORM)
    {
        sgraph::TransformNode* t_node = (sgraph::TransformNode*) node->getParent();
        switch(type)
        {
        case TRANSLATION:
            t_node->addTranslation(data[0], data[1], data[2]);
            break;
        case SCALE:
            t_node->addScale(data[0], data[1], data[2]);
            break;
        case ROTATION:
            t_node->addRotation(data[0], data[1], data[2], data[3]);
            break;
        }
    }

    else
    {
        //If parent is not Transform node, add a transform node as parent
        //and set parent of transform node to old parent
        sgraph::TransformNode* new_t_node = new sgraph::TransformNode(scenegraph, "added_trans");
        switch(type)
        {
        case TRANSLATION:
            new_t_node->addTranslation(data[0], data[1], data[2]);
            break;
        case SCALE:
            new_t_node->addScale(data[0], data[1], data[2]);
            break;
        case ROTATION:
            new_t_node->addRotation(data[0], data[1], data[2], data[3]);
            break;
        }
        new_t_node->setParent(node->getParent());
        new_t_node->addChild(node);
        node->setParent(new_t_node);
    }

}

/**
 * @brief View::saveObjects
 * Creates and saves XML snippet for each
 * object file used in the rendering of scenegraph
 *
 * @param output_file:
 * fstream object representing the file to which we are writing XML
 */
void View::saveObjects(fstream& output_file)
{
    //Grab objects map from scenegraph
    map<string,string> objects = scenegraph->getObjects();
    //Iterate thorugh map and add each object to XML
    for(auto entry : objects)
    {
        string str_to_add = "<instance name=\"";
        str_to_add += entry.first;
        str_to_add += "\" path=\"";
        str_to_add += entry.second;
        str_to_add += "\" ></instance>";

        //Add new texture entry to xml file
        output_file << str_to_add << endl;
    }
}

/**
 * @brief View::saveTextures
 * Creates and saves XML snippet for each texture used in rendering of
 * scenegraph
 *
 * @param output_file
 * fstream object representing the file to which we are writing XML
 */
void View::saveTextures(fstream& output_file)
{
    //Grab texture map from scenegraph
    map<string,string> textures = scenegraph->getTextures();
    //Iterate through map and add each texture
    for(auto entry : textures)
    {
        string str_to_add = "<image name=\"";
        str_to_add += entry.first;
        str_to_add += "\" path=\"";
        str_to_add += entry.second;
        str_to_add += "\" />";

        //Add new texture entry to xml file
        output_file << str_to_add << endl;

    }
}

/**
 * @brief View::saveLights
 * Creates and saves XML snippet for each texture used in rendering of
 * scenegraph
 *
 * @param output_file
 * fstream object representing the file to which we are writing XML
 */
void View::saveLights(fstream& output_file)
{
    //Grab lights from scenegraph renderer
    vector<util::Light> lights = scenegraph->getRendererLights();
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec4 position;
    float spotangle;
    glm::vec4 spotdirection;

    //Iterate through lights, grab properties and insert into output file
    for(auto light : lights)
    {
        //Set light properties
        ambient = light.getAmbient();
        diffuse = light.getDiffuse();
        specular = light.getSpecular();
        position = light.getPosition();
        spotangle = light.getSpotCutoff();
        spotdirection = light.getSpotDirection();

        //Add light tag
        output_file << "<light>" << endl;
        //Add light properties
        output_file << "<ambient> " << to_string(ambient[0]) << " " << to_string(ambient[1]) << " "
                    << to_string(ambient[2]) << " </ambient>" << endl;
        output_file << "<diffuse> " << to_string(diffuse[0]) << " " << to_string(diffuse[1]) << " "
                    << to_string(diffuse[2]) << " </diffuse>" << endl;
        output_file << "<specular> " << to_string(specular[0]) << " " << to_string(specular[1]) << " "
                    << to_string(specular[2]) << " </specular>" << endl;
        output_file << "<position> " << to_string(position[0]) << " " << to_string(position[1]) << " "
                    << to_string(position[2]) << " </position>" << endl;
        output_file << "<spotangle> " << to_string(spotangle) << " </spotangle>" << endl;
        output_file << "<spotdirection> " << to_string(spotdirection[0]) << " " << to_string(spotdirection[1]) << " "
                    << to_string(spotdirection[2]) << " </spotdirection>" << endl;
        //Add ending light tag
        output_file << "</light>" << endl;

    }

}

/**
 * @brief View::insertTabs
 * When XML is written to output file using View::saveXMLFile, it is not
 * formatted in a visually appealing way. View::insertTabs parses the output
 * file and tabs out portions of the XML to improve readability
 *
 * @param filename
 * The name of the file we want to parse and insert tabs into
 */
void View::insertTabs(string filename)
{
    ///This function will insert tabs into our xml file for formatting
    //Open XML File
    fstream xml_file;
    ofstream temp_file;
    xml_file.open(filename);
    string temp_file_name = "scenegraphs/temp_format.xml";
    temp_file.open(temp_file_name);

    //Bookkeeping
    int current_tab = 0;
    bool in_set = false;
    bool in_material = false;

    string line;
    string formatted_line;

    if(xml_file.is_open())
    {
        while(getline(xml_file, line))
        {
            //Clear formatted line
            formatted_line.clear();

            //Decrement tab count if needed
            if(decrementTab(line))
                current_tab--;

            //Add correct amount of tabs
            for(int i = 0; i < current_tab; i++)
            {
                formatted_line += "\t";
            }

            //Put line back into file
            formatted_line += line;
            temp_file << formatted_line << endl;

            //Increment tab count if necessary
            if(incrementTab(line))
                current_tab++;
        }
    }

    //Close files
    xml_file.close();
    temp_file.close();

    //Once file copied, rename sgraph file name
    string delete_name = "scenegraphs/delete.xml";
    std::rename(filename.c_str(), delete_name.c_str());
    int bad = std::rename(temp_file_name.c_str(), filename.c_str());
    if(bad) {cout << "Couldn't rename\n"; }

    //Delete old scene file
    std::remove(delete_name.c_str());
}

/**
 * @brief View::incrementTab
 * Used to determine whether or not View::insertTabs should increment its
 * internal count of the number of tabs to apply to the current line.
 *
 * @param line
 * The current line of the XML output file. This parameter is parsed for
 * specific tags that correspond to tab count being incremented.
 *
 * @return
 * Returns true if View::insertTabs should increment its internal tab count.
 * Returns false otherwise.
 */
bool View::incrementTab(string line)
{
    ///Returns true if we should increment tab count for xml file
    if(line.find("<scene") != string::npos)
        return true;

    if(line.find("<light") != string::npos)
        return true;

    if(line.find("<group") != string::npos)
        return true;

    if(line.find("<transform") != string::npos)
        return true;

    if(line.find("<material") != string::npos)
        return true;

    if(line.find("<object") != string::npos)
        return true;

    return false;
}

/**
 * @brief View::decrementTab
 * Used to determine wheter or not View::insertTabs should decrement its
 * internal count of the number of tabs to apply to the current line
 *
 * @param line
 * The current line of the XML output file. This parameter is parsed for
 * specific tags that each correspond to tab count being decremented.
 *
 * @return
 * Returns true if View::insertTabs should decrement its internal tab count
 * Returns false otherwise
 */
bool View::decrementTab(string line)
{
    ///Returns true if we should decrement tab count for xml file
    if(line.find("</scene") != string::npos)
        return true;

    if(line.find("</light") != string::npos)
        return true;

    if(line.find("</group") != string::npos)
        return true;

    if(line.find("</transform") != string::npos)
        return true;

    if(line.find("</material") != string::npos)
        return true;

    if(line.find("</object") != string::npos)
        return true;

    return false;
}

/**
 * @brief View::mousePressed
 * Called from the MyGLWidget when a mouse press event occurs.
 * @param x
 * The x-coordinate of the mouse
 *
 * @param y
 * The y-coordinate of the mouse
 */
void View::mousePressed(int x,int y)
{
  mousePos = glm::vec2(x,y);
}

/**
 * @brief View::mouseReleased
 * Called from the MyGLWidget when the mouse is released
 *
 * @param x
 * The x-coordinate of the mouse
 *
 * @param y
 * The y-coordinate of the mouse
 */
void View::mouseReleased(int x,int y)
{

}

/**
 * @brief View::mouseDragged
 * Called from the MyGLWidget when the mouse is pressed/dragged
 *
 * @param x
 * The x-coordinate of the mouse after the drag
 *
 * @param y
 * The y-coordinate of the mouse after the drag
 */
void View::mouseDragged(int x,int y)
{
  glm::vec2 newM = glm::vec2((float)x,(float)y);

  glm::vec2 delta = glm::vec2((float)(newM.x-mousePos.x),(float)(newM.y-mousePos.y));
  mousePos = newM;

  trackballTransform =
      glm::rotate(glm::mat4(1.0),delta.x/trackballRadius,glm::vec3(0.0f,1.0f,0.0f)) *
      glm::rotate(glm::mat4(1.0),delta.y/trackballRadius,glm::vec3(-1.0f,0.0f,0.0f)) *
      trackballTransform;
}
