#ifndef _GLSCENEGRAPHRENDERER_H_
#define _GLSCENEGRAPHRENDERER_H_

#include "INode.h"
#include "OpenGLFunctions.h"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "Material.h"
#include "TextureImage.h"
#include "ObjectInstance.h"
#include "IVertexData.h"
#include "ShaderLocationsVault.h"
#include <string>
#include <sstream>
#include <map>
#include <stack>
#include <cmath>
#include <fstream>
using namespace std;

namespace sgraph
{

/**
 * This is a scene graph renderer implementation that works specifically
 * with the Qt library
 * It mandates OpenGL 3 and above.
 * \author Amit Shesh
 */
class GLScenegraphRenderer
{

private:
    /**
     * The Qt specific rendering context
     */
    util::OpenGLFunctions *glContext;
    /**
     * A table of shader locations and variable names
     */
protected :
    util::ShaderLocationsVault shaderLocations;
    /**
     * A table of shader variables -> vertex attribute names in each mesh
     */
    map<string,string> shaderVarsToVertexAttribs;

    /**
     * A map to store all the textures
     */
    map<string, util::TextureImage *> textures;

    vector<util::Light> lights;
    /**
     * A table of renderers for individual meshes
     */
    map<string, util::ObjectInstance *> meshRenderers;

    /**
     * A variable tracking whether shader locations have been set. This must be done before
     * drawing!
     */
    bool shaderLocationsSet;

public:
    GLScenegraphRenderer()
    {
        shaderLocationsSet = false;
    }

    /**
     * Specifically checks if the passed rendering context is the correct JOGL-specific
     * rendering context
     * \param obj the rendering context (should be OpenGLFunctions)
     */
    /**
     * @brief setContext
     * Sets our OpenGL rendering context
     *
     * @param obj
     * An OpenGL wrapper class that allows for OpenGL functionality to be used
     * througout the program
     */
    void setContext(util::OpenGLFunctions *obj)
    {
        glContext = obj;

    }

    /**
     * @brief getLights
     * Returns all lights contained within this renderer
     * @return
     * A vector of util::Light
     */
    vector<util::Light> getLights()
    {
        return lights;
    }

    /**
     * @brief addMesh
     * Add a mesh to be drawn later. The rendering context should be set before
     * calling this function, as this function needs it to perform its tasks.
     * This function creates a new sgraph::GLMeshRenderer object for this mesh
     *
     * @param name
     * The name by which this mesh is referred to by the scenegraph
     *
     * @param
     * mesh
     * The util::PolygonMesh object that represents this mesh
     */
    template <class K>
    void addMesh(const string& name,
                 util::PolygonMesh<K>& mesh) throw(runtime_error)
    {
        if (!shaderLocationsSet)
            throw runtime_error("Attempting to add mesh before setting shader variables. Call initShaderProgram first");
        if (glContext==NULL)
            throw runtime_error("Attempting to add mesh before setting GL context. Call setContext and pass it a GLAutoDrawable first.");

        //verify that the mesh has all the vertex attributes as specified in the map
        if (mesh.getVertexCount()<=0)
            return;
        K vertexData = mesh.getVertexAttributes()[0];
        for (map<string,string>::iterator it=shaderVarsToVertexAttribs.begin();
             it!=shaderVarsToVertexAttribs.end();it++) {
            if (!vertexData.hasData(it->second))
                throw runtime_error("Mesh does not have vertex attribute "+it->second);
        }
        util::ObjectInstance *mr = new util::ObjectInstance(name);
        mr->initPolygonMesh<K>(*glContext,
                            shaderLocations,
                            shaderVarsToVertexAttribs,
                            mesh);
        this->meshRenderers[name] = mr;
    }

    /**
     * @brief addTexture
     * Add a new util::TextureImage to this renderer
     *
     * @param name
     * Name of the new texture
     *
     * @param path
     * Path to the texture file
     */
    void addTexture(const string& name,const string& path)
    {
        util::TextureImage *image = NULL;
        try {
            image = new util::TextureImage(path,name);
        } catch (runtime_error e) {
            throw runtime_error("Texture "+path+" cannot be read!");
        }
        QOpenGLTexture *im = image->getTexture();
        im->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
        im->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        im->setWrapMode(QOpenGLTexture::Repeat);
        textures[name]=image;
    }

    /**
     * @brief draw
     * Begin rendering of the scenegraph from the root
     *
     * @param root
     * The root node of the scenegraph
     *
     * @param modelView
     * The composite transformation applied to the rendered scene
     */
    void draw(INode *root, stack<glm::mat4>& modelView)
    {
      glContext->glEnable(GL_TEXTURE_2D);
      glContext->glActiveTexture(GL_TEXTURE0);
      int loc = -1;
      loc = shaderLocations.getLocation("image");
      if (loc >= 0) {
        glContext->glUniform1i(loc, 0);
      }
      lights = root->getLightsInView(modelView);
      this->initLightsInShader(lights);
      root->draw(*this,modelView);
    }

    /**
     * @brief startSave
     * Begin the process of saving the scenegraph to an XML file starting at
     * the root node
     *
     * @param root
     * A reference to the root node of the scenegraph
     *
     * @param output_file
     * An fstream object representing the output XML file
     */
    void startSave(INode* root, fstream& output_file)
    {
        root->saveToXML(output_file);
    }

    /**
     * @brief initLightsInShader
     * Used to add the lights to the shader
     *
     * @param lights
     * A vector of util::Light to pass to the shader
     */
    void initLightsInShader(const vector<util::Light>& lights)
    {
        int loc = -1;


        util::OpenGLFunctions *gl = glContext;

        loc = shaderLocations.getLocation("numLights");
        if (loc >= 0) {
          gl->glUniform1i(loc, lights.size());
        } else {
          throw runtime_error("No shader variable for \" numLights \"");
        }

        for (int i = 0; i < lights.size(); i++)
          {
            stringstream str;
            str << "light[" << i << "].";
            string name = str.str();
            loc = shaderLocations.getLocation(name + "ambient");
            if (loc >= 0) {
              gl->glUniform3fv(loc, 1, glm::value_ptr(lights[i].getAmbient()));
            } else {
              throw runtime_error("No shader variable for \" " + name + "ambient" + " \"");
            }

            loc = shaderLocations.getLocation(name + "diffuse");
            if (loc >= 0) {
              gl->glUniform3fv(loc, 1, glm::value_ptr(lights[i].getDiffuse()));
            } else {
              throw runtime_error("No shader variable for \" " + name + "diffuse" + " \"");
            }

            loc = shaderLocations.getLocation(name + "specular");
            if (loc >= 0) {
              gl->glUniform3fv(loc, 1, glm::value_ptr(lights[i].getSpecular()));
            } else {
              throw runtime_error("No shader variable for \" " + name + "specular" + " \"");
            }

            loc = shaderLocations.getLocation(name + "position");
            if (loc >= 0) {
              gl->glUniform4fv(loc, 1, glm::value_ptr(lights[i].getPosition()));
            } else {
              throw runtime_error("No shader variable for \" " + name + "position" + " \"");
            }


            loc = shaderLocations.getLocation(name + "spotdirection");
            if (loc >= 0) {
              gl->glUniform4fv(loc, 1, glm::value_ptr(lights[i].getSpotDirection()));
            } else {
              throw runtime_error("No shader variable for \" " + name + "spotdirection" + " \"");
            }

            loc = shaderLocations.getLocation(name + "cosSpotCutoff");
            if (loc >= 0) {
              gl->glUniform1f(loc, (float) cos(glm::radians(lights[i].getSpotCutoff())));
            } else {
              throw runtime_error("No shader variable for \" " + name + "cosSpotCutoff" + " \"");
            }
        }


      }


    /**
     * @brief dispose
     * Called when this renderer is destroyed
     */
    void dispose()
    {
        for (map<string,util::ObjectInstance *>::iterator it=meshRenderers.begin();
             it!=meshRenderers.end();it++)
          {
            it->second->cleanup(*glContext);
          }
    }

    /**
     * @brief drawMesh
     * Draws a specific mesh. If the mesh has been added to this renderer, it
     * delegates to its corresponding mesh renderer. This function first passes
     * the material to the shader.
     *
     * @param name
     * The name of the mesh to draw
     *
     * @param material
     * The material to be applied to the mesh during rendering
     *
     * @param textureName
     * The name of the texture to be applied to the mesh during rendering
     *
     * @param transformation
     * The current transformation applied to this mesh (from modelview)
     */
    void drawMesh(const string& name,
                  const util::Material& material,
                  const string& textureName,
                  const glm::mat4& transformation)
    {
        if (meshRenderers.count(name)==1)
        {
            int loc = shaderLocations.getLocation("material.ambient");
            //set the ambient
            if (loc<0)
                throw runtime_error("No shader variable for \" material.ambient \"");

            glContext->glUniform3fv(loc,1,glm::value_ptr(material.getAmbient()));

            loc = shaderLocations.getLocation("material.diffuse");
            //set the diffuse
            if (loc<0)
                throw runtime_error("No shader variable for \" material.diffuse \"");

            glContext->glUniform3fv(loc,1,glm::value_ptr(material.getDiffuse()));

            loc = shaderLocations.getLocation("material.specular");
            //set the specular
            if (loc<0)
                throw runtime_error("No shader variable for \" material.specular \"");

            glContext->glUniform3fv(loc,1,glm::value_ptr(material.getSpecular()));

            loc = shaderLocations.getLocation("material.shininess");
            //set the shininess
            if (loc<0)
                throw runtime_error("No shader variable for \" material.shininess \"");

            glContext->glUniform1f(loc,material.getShininess());


            loc = shaderLocations.getLocation("modelview");
            if (loc<0)
                throw runtime_error("No shader variable for \" modelview \"");

            glContext->glUniformMatrix4fv(loc,
                                  1,
                                  false,glm::value_ptr(transformation));

            loc = shaderLocations.getLocation("normalmatrix");
            if (loc<0)
                throw runtime_error("No shader variable for \" normalmatrix \"");
            glm::mat4 normalmatrix = glm::inverse(glm::transpose(transformation));

            glContext->glUniformMatrix4fv(loc,
                                  1,
                                  false,glm::value_ptr(normalmatrix));

            loc = shaderLocations.getLocation("texturematrix");
            if (loc<0)
                throw runtime_error("No shader variable for \" texturematrix \"");
            glm::mat4 texturematrix = glm::mat4(1.0);
            glContext->glUniformMatrix4fv(loc,
                                  1,
                                  false,glm::value_ptr(texturematrix));


            if (textures.count(textureName)>0)
              textures[textureName]->getTexture()->bind();
            else if (textures.count("checkerboard")>0 && textures.count("checkerboard-box")>0)
            {
                if(name != "box")
                    textures["checkerboard"]->getTexture()->bind();
                else
                    textures["checkerboard-box"]->getTexture()->bind();
            }

            meshRenderers[name]->draw(*glContext);
        }
    }


    /**
     * @brief initShaderProgram
     * Queries the shader program for all variables and locations then adds
     * them to the renderer
     *
     * @param shaderProgram
     * The shader program we are querying
     *
     * @param shaderVarsToVertexAttribs
     * A map containing the shader variables and thier corresponding vertex
     * attributes
     */
    void initShaderProgram(util::ShaderProgram& shaderProgram,
                           map<string,string>& shaderVarsToVertexAttribs)
    {
        if (glContext==NULL)
          throw runtime_error("No context set");

        shaderLocations = shaderProgram.getAllShaderVariables(*glContext);
        this->shaderVarsToVertexAttribs = shaderVarsToVertexAttribs;
        shaderLocationsSet = true;

    }

    /**
     * @brief getShaderLocation
     * Gets the location of a passed shader variable within the shader
     *
     * @param name
     * The name of the variable whose location will be returned
     *
     * @return
     * The location of the passed shader variable
     */
    int getShaderLocation(const string& name)
    {
        return shaderLocations.getLocation(name);
    }
};
}
#endif
