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
#include <iostream>

using namespace sgraph;
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

    /**
     * @brief node_names
     * A vector containing the names of all nodes in the scenegraph
     */
    vector<string> node_names;

    /**
     * @brief object_select_tex
     * The name of the texture to change to when an object is selected
     */
    string object_select_tex = "selected_object";


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

    /**
     * @brief saveToXML
     * Starts recursion through scenegraph to save every node to XML file
     *
     * @param output_file
     * The XML file to which we are saving this scenegraph to
     */
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

    /**
     * @brief addNode
     * Adds a node to the nodes map
     *
     * @param name
     * The name of the node to add
     *
     * @param node
     * The node object to add to the map
     */
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

    /**
     * @brief addNodeName
     * Used to add a name to node_names vector. This vector keeps track of
     * the names used by nodes in the scenegraph.
     *
     * @param name
     * The name to add to the node_names vector
     */
    void addNodeName(const string& name)
    {
        node_names.push_back(name);
    }

    /**
     * @brief printNodeNames
     * Will print out all names contained in member node_names
     */
    void printNodeNames()
    {
        for(auto name : node_names)
            std::cout << name << endl;
    }

    /**
     * @brief isValidNodeName
     * Returns 'true' if passed name is the name of a node in scenegraph,
     * 'false' otherwise.
     *
     * @param name
     * The name of the node being searched for
     *
     * @return
     * 'true' if @param name is the name of a node in the scenegraph, 'false'
     * otherwise.
     */
    bool isValidNodeName(const string& name)
    {
        for(auto entry : node_names)
        {
            if(name == entry)
                return true;
        }

        return false;
    }

    /**
     * @brief changeNodeTexture
     * Change the texture of a particular node within the scenegraph
     *
     * @param node_name
     * The name of the node whose texture is to be changed
     *
     * @param texture_name
     * The name of the texture to change to - must be contained within the
     * XML file
     */
    void changeNodeTexture(const string& node_name, const string& texture_name)
    {
        //Find node by name
        if(isValidNodeName(node_name))
        {
            if(nodes[node_name]->getNodeType() == LEAF)
                nodes[node_name]->changeNodeTexture(texture_name);

        }
        else
        {
            return;
        }
    }

    /**
     * @brief selectNode
     * Changes a particular node to have the 'selected_object' texture
     *
     * @param node_name
     * The name of the node we are selecting
     */
    void selectNode(const string& node_name)
    {
        changeNodeTexture(node_name, object_select_tex);
    }

    /**
     * @brief revertNodeTexture
     * Reverts a particular node to its previously held texture. Used when
     * an object is unselected to change it back to its original texture
     *
     * @param node_name
     * The name of the node whose texture we want to revert
     */
    void revertNodeTexture(const string& node_name)
    {
        if(isValidNodeName(node_name))
        {
            if(nodes[node_name]->getNodeType() == LEAF)
                nodes[node_name]->revertNodeTexture();
        }
        else
            return;
    }

    /**
     * @brief getNodeByName
     * Returns a particular node whose name matches the parameter
     *
     * @param name
     * The name of the node to return
     *
     * @return
     * The node object with a name matching the name parameter. Returns a
     * nullptr if node with given name does not exist.
     */
    INode* getNodeByName(const string& name)
    {
        //Search map of nodes to get node
        if(isValidNodeName(name))
        {
            return nodes[name];
        }
        else
        {
            return nullptr;
        }
    }

    /**
     * @brief generateScenegraphTreeView
     * Starts recursion through scenegraph nodes to generate tree view of
     * scenegraph in the right-side pane of the GUI
     *
     * @param ret_vec
     * A vector containing GeneratedItem objects who hold information about
     * each of the nodes that will be represented in the tree view of the
     * scenegraph.
     */
    void generateScenegraphTreeView(vector<INode::GeneratedItem>& ret_vec)
    {
        if(root != NULL)
        {
            root->generateScenegraphTreeView(ret_vec);
        }
    }
  };
}
#endif
