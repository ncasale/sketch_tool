#ifndef _SCENEGRAPH_H_
#define _SCENEGRAPH_H_

#include "GLScenegraphRenderer.h"
#include "INode.h"
#include "OpenGLFunctions.h"
#include "glm/glm.hpp"
#include "IVertexData.h"
#include "PolygonMesh.h"
#include <string>
#include <map>
#include <fstream>
using namespace std;

namespace sgraph
{

  /**
 * A specific implementation of this scene graph. This implementation is still independent
 * of the rendering technology (i.e. OpenGL)
 * \author Amit Shesh
 */

  class Scenegraph
  {
    /**
     * The root of the scene graph tree
     */
  protected:
    INode *root;



    /**
     * A map to store the (name,node) pairs. A map is chosen for efficient search
     */
    map<string,INode *> nodes;

    /**
     * A map to store the (name,path) pairs for textures
     */
    map<string,string> textures;

    /**
     * A map to store the object pairs for objects
     */
    map<string,string> objects;

    /**
     * The associated renderer for this scene graph. This must be set before attempting to
     * render the scene graph
     */
    GLScenegraphRenderer *renderer;


  public:
    Scenegraph()
    {
      root = NULL;
    }

    ~Scenegraph()
    {
      dispose();
    }

    /**
     * @brief dispose
     * Called when the scenegraph is no longer needed
     */
    void dispose()
    {

      if (root!=NULL)
        {
          delete root;
          root = NULL;
        }
    }

    /**
     * @brief getRendererLights
     * Gets all lights stored within the renderer for this scenegraph
     * @return
     * A list of lights contained within the scenegraph's renderer
     */
    vector<util::Light> getRendererLights()
    {
        return renderer->getLights();
    }

    /**
     * @brief setRenderer
     * Sets the renderer and then adds all the meshes to the renderer. This
     * function must be called when the scenegraph is complete, otherwise not
     * all of its meshes will be known to the renderer
     *
     * @param renderer
     * The IScenegraphRenderer object that will act as the scenegraph's
     * renderer
     */
    template <class VertexType>
    void setRenderer(GLScenegraphRenderer *renderer,map<string,
                     util::PolygonMesh<VertexType> >& meshes) throw(runtime_error)
    {
      this->renderer = renderer;

      //now add all the meshes
      for (typename map<string,util::PolygonMesh<VertexType> >::iterator it=meshes.begin();
           it!=meshes.end();
           it++)
        {
          this->renderer->addMesh<VertexType>(it->first,it->second);
        }

      //pass all the texture objects
      for (map<string,string>::iterator it=textures.begin();
           it!=textures.end();
           it++)
        {
          this->renderer->addTexture(it->first,it->second);
        }

    }


    /**
     * Set the root of the scenegraph, and then pass a reference to this scene graph object
     * to all its node. This will enable any node to call functions of its associated scene graph
     * \param root
     */

    /**
     * @brief makeScenegraph
     * Sets the root of the scenegraph and then passes a reference to this
     * scenegraph object to all its nodes. This will enable any node to call
     * functions of its associated scenegraph.
     *
     * @param root
     * The root of the scenegraph
     */
    void makeScenegraph(INode *root)
    {
      this->root = root;
      this->root->setScenegraph(this);

    }

    /**
     * @brief draw
     * Draw this scenegraph. It delegates this operation to the renderer
     *
     * @param modelView
     * The stack of modelview matrices
     */
    void draw(stack<glm::mat4>& modelView) {
      if ((root!=NULL) && (renderer!=NULL))
        {
          renderer->draw(root,modelView);
        }
    }

    void saveToXML(fstream& output_file)
    {
        if((root!=NULL) && (renderer!=NULL))
        {
            renderer->startSave(root, output_file);
        }
    }


    /**
     * @brief getTextures
     * Returns the map of textures associated with this scenegraph
     *
     * @return
     * The map of textures associated with this scenegraph
     */
    map<string,string> getTextures()
    {
        return textures;
    }

    /**
     * @brief getObjects
     * Returns the map of objects associated with this scenegraph
     *
     * @return
     * The map of objects associated with this scenegraph
     */
    map<string,string> getObjects()
    {
        return objects;
    }


    /**
     * @brief animate
     * Used to animate -- (not used in our implementation
     *
     * @param time
     * A simple time reference for animation
     */
    void animate(float time)
    {

    }

    void addNode(const string& name, INode *node) {
      nodes[name]=node;
    }


    /**
     * @brief getRoot
     * Get the root of this scenegraph
     *
     * @return
     * The root of this scenegraph
     */
    INode *getRoot()
    {
      return root;
    }


    /**
     * @brief getNodes
     * Get the map of nodes associated with this scenegraph
     *
     * @return
     * The map of nodes associated with this scenegraph
     */
    map<string, INode *> getNodes()
    {
      return nodes;
    }

    /**
     * @brief addTexture
     * Add a texture to this scenegraph's texture map
     *
     * @param name
     * The name of the new texture
     *
     * @param path
     * The path to the new texture's texture file
     */
    void addTexture(const string& name, const string& path)
    {
      textures[name] = path;
    }

    /**
     * @brief addObject
     * Add an object to this scenegraph's object map
     *
     * @param name
     * The name of the new object
     *
     * @param path
     * The path to the new object's object file
     */
    void addObject(const string& name, const string& path)
    {
        objects[name] = path;
    }
  };
}
#endif
