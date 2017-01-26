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

View::View()
{   
  WINDOW_WIDTH = WINDOW_HEIGHT = 0;
  trackballRadius = 300;
  trackballTransform = glm::mat4(1.0);
  proj = glm::mat4(1.0);
  scenegraph = NULL;

  //Populate the default attributes
  default_attributes.insert(pair<string,vector<float>>("scale", {50, 50, 50}));
  default_attributes.insert(pair<string,vector<float>>("ambient", {0.8f, 0.8f, 0.8f}));
  default_attributes.insert(pair<string,vector<float>>("specular", {0.8f, 0.8f, 0.8f}));
  default_attributes.insert(pair<string,vector<float>>("diffuse", {0.8f, 0.8f, 0.8f}));
  default_attributes.insert(pair<string,vector<float>>("shininess", {100.0f}));

}

View::~View()
{
  if (scenegraph!=NULL)
    delete scenegraph;
}

void View::clearScenegraph()
{
    //Revert sketch.xml back to default
    //Now let's append to xml -- will be done by parsing for group tag and then appending
    ifstream default_file;
    default_file.open(sgraph_default);
    ofstream sketch_file;
    sketch_file.open(sgraph_file_location);

    //Copy each line of default file into sketch.xml
    string line;
    while(getline(default_file, line))
    {
        sketch_file << line << endl;
    }

    //Close files
    sketch_file.close();
    default_file.close();
}

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
  program.disable(gl);

}

void View::recreateScenegraph(util::OpenGLFunctions &gl, const string& filename) throw(runtime_error)
{
    if(scenegraph!=NULL)
        delete scenegraph;

    //Allow group creation in XML
    new_group = true;

    //Create scenegraph
    sgraph::ScenegraphInfo<VertexAttrib> sinfo;
    sinfo = sgraph::SceneXMLReader::importScenegraph<VertexAttrib>(filename);
    scenegraph = sinfo.scenegraph;

    //Pair scenegraph with renderer
    scenegraph->setRenderer(&renderer, sinfo.meshes);

}

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

void View::draw(util::OpenGLFunctions& gl)
{
  gl.glClearColor(0,0,0,1);
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
      glm::lookAt(glm::vec3(70.0f,100.0f,-80.0f),
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

void View::mousePressed(int x,int y)
{
  mousePos = glm::vec2(x,y);
}

void View::mouseReleased(int x,int y)
{

}

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

//Shape Creation Functions
void View::createShape(string shape)
{
    //This function will add a cube at the origin to the scenegraph. Done by
    //appending information to the scenegraph file
    string xml_to_add = generateXML(shape, default_attributes);

    /*
    //Add the transformations -- for now we will set this to a scale of 50
    xml_to_add += "\n\t\t<transform>\n\t\t\t<set>\n\t\t\t\t<scale>50 50 50 </scale>\n\t\t\t</set>\n";

    //Add the object
    xml_to_add += "\n\t\t\t<object instanceof=\"";
    xml_to_add += shape;
    xml_to_add += "\">\n\t\t\t\t<material>\n\t\t\t\t\t<ambient>0.8 0.8 0.8</ambient>\n\t\t\t\t\t<diffuse>0.8 0.8 0.8</diffuse>\n";
    xml_to_add += "\t\t\t\t\t<specular>0.8 0.8 0.8</specular>\n\t\t\t\t\t<shininess>100</shininess>\n\t\t\t\t</material>\n\t\t\t</object>\n\t\t</transform>\n";
    */

    //Now let's append to xml -- will be done by parsing for group tag and then appending
    ifstream xml_read_file;
    xml_read_file.open(sgraph_file_location);
    ofstream copy_file;
    copy_file.open("temp.txt");
    string group_str = "<group>";
    string append_str = "<!-- append -->"; //How else to denote insertion point?

    //Copy each line of xml into temp file -- when group tag has been parsed, add new xml
    string line;
    while(getline(xml_read_file, line))
    {
        if(!new_group)
        {
            if(line.find(group_str) != std::string::npos)
            {
                copy_file << line << xml_to_add;
            }
            else
            {
                copy_file << line << endl;
            }
        }
        else
        {
            if(line.find(append_str) != std::string::npos)
            {
                copy_file << line << "\n\t<group>"<< xml_to_add << "\t</group>" << endl;
            }
            else
            {
                copy_file << line << endl;
            }

        }

    }

    //Set to false until new group is created
    new_group = false;

    //Close files
    copy_file.close();
    xml_read_file.close();

    cout << xml_to_add;

    //Once file copied, rename sgraph file name
    string delete_name = "scenegraphs/delete.xml";
    std::rename(sgraph_file_location.c_str(), "scenegraphs/delete.xml");
    int bad = std::rename("temp.txt", sgraph_file_location.c_str());
    if(bad) {cout << "Couldn't rename\n"; }

    //Delete old scene file
    std::remove(delete_name.c_str());
}

string View::generateXML(string shape, map<string,vector<float>> attributes)
{
    ///This function will generate the XML we will add to our file
    string xml_to_add = "\n";

    //Add transform/set tags
    xml_to_add += "<transform>\n<set>\n</set>\n";

    //Add object/material tags with correct shape
    xml_to_add += "\n\n<object instanceof=\"";
    xml_to_add += shape;
    xml_to_add += "\">\n<material>\n</material>\n</object>\n</transform>\n";

    //Now we'll parse attributes and add each to XML
    xml_to_add = parseAttributes(xml_to_add, attributes);

    return xml_to_add;
}

string View::parseAttributes(string xml, map<string,vector<float>> attributes)
{
    //At this point we have our transform, set, material and object tags in place. If we are
    //doing a transformation, we will put inside of set, otherwise we will put inside
    //of material
    size_t insert_pos;
    string insert_str;
    string curr_val;
    for(auto entry : attributes)
    {
        //Construct insert string
        string tag = entry.first;
        insert_str = "<" + tag + ">";

        if(tag == "scale" || tag == "rotate" || tag == "translate")
        {
            //Do basic transform and put in <set>
            insert_pos = xml.find("</set>"); //This should definitely be found because it is hardcoded
            if(insert_pos == string::npos)
            {
                //Error
                cerr << "Could not find set tag" << endl;
            }
        }
        else
        {
            //Put inside <material>
            insert_pos = xml.find("</material>");
            if(insert_pos == string::npos)
            {
                //Error
                cerr << "Could not find material tag" << endl;
            }
        }

        //Iterate through values and add each
        for(auto val : entry.second)
        {
            curr_val = to_string(val);
            insert_str += curr_val + " ";
        }
        //After values inserted, close tag
        insert_str += "</" + tag + ">\n";
        //Insert new attribute string into xml
        xml.insert(insert_pos, insert_str);
    }

    //Return new xml string
    return xml;
}

void View::insertTabs(string& xml)
{
    ///This function will insert tabs into our xml string so it looks properly formatted

}

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

void View::dispose(util::OpenGLFunctions& gl)
{
  //clean up the OpenGL resources used by the object
  scenegraph->dispose();
  renderer.dispose();
  //release the shader resources
  program.releaseShaders(gl);
}
