#ifndef _ISCENEGRAPH_H_
#define _ISCENEGRAPH_H_


#include "OpenGLFunctions.h"
#include <glm/glm.hpp>
#include "IVertexData.h"
#include "GLScenegraphRenderer.h"
#include "INode.h"
#include "ShaderProgram.h"
#include "SGraphItemInfo.h"
#include <vector>
using namespace util;

#include <map>
#include <stack>
using namespace std;
namespace sgraph
{

    /**
 * This virtual class captures all the operations that a scene graph should offer.
 * It is designed to be a generic scene graph that is independent of the actual
 * rendering library. It achieves this by working with an
 * sgraph::IScenegraphRenderer virtual class.
 *
 * The scene graph provides functions to store actual geometry
 * (object instances) within itself so that they can be shared within many
 * leaves. Each node of the scene graph keeps a reference to it, enabling any
 * node of the scene graph to directly call its functions.
 *
 * Conversely the scene graph also stores references to all the nodes keyed by
 * their name. This way the scene graph can directly refer to any of its nodes
 * by name instead of traversing the tree every time to find it. This is useful
 * when nodes must be identified and animated in specific ways.
 * \author Amit Shesh
 */

    class IScenegraph
    {
    public:
        /**
         * @brief setRenderer
         * Set the renderer to this scenegraph. All specific rendering commands
         * are delegated to this renderer, making the scenegraph independent
         * of the rendering implementation
         *
         * @param renderer
         * The IScenegraphRenderer object that will act as the renderer
         */
        virtual void setRenderer(GLScenegraphRenderer *renderer)=0;

        /**
         * @brief makeScenegraph
         * Initialize the supplied root to be the root of this scenegraph. This
         * is supposed to overwrite any previous roots
         *
         * @param root
         * The root of the scenegraph
         */
        virtual void makeScenegraph(INode *root)=0;

        /**
         * @brief draw
         * Draw this scenegraph using the stack of modelview matrices provided.
         * This scenegraph will use this stack as it navigates the tree.
         *
         * @param modelView
         * Stack of modelview matrices
         */
        virtual void draw(stack<glm::mat4>& modelView)=0;

        /**
         * @brief addPolygonMesh
         * Add a polygon mesh that will be used by one or more leaves in this
         * scenegraph
         *
         * @param name
         * A unique name by which this mesh may be referred to in the future
         *
         * @param obj
         * The {@link uti::PolygonMesh} object
         */
        virtual void addPolygonMesh(const string& name,
                                    util::PolygonMesh<VertexType>& obj)=0;

        /**
         * @brief animate
         * Specific scenegraph implementations should be put in the code to
         * animate specific nodes in the scenegraph based on a time provided
         * by the caller
         *
         * @param time
         * Provides a simple time reference for animation
         */
        virtual void animate(float time)=0;

        /**
         * @brief addNode
         * Adds a node to the scenegraph. This should be stored in a suitable
         * manner by an implementation, so that it is possible to look up a
         * specific node by name
         *
         * @param name
         * Name of the node (hopefully unique, but not required)
         *
         * @param node
         * The node object
         */
        virtual void addNode(const string& name,INode *node)=0;

        /**
         * @brief getRoot
         * Get the root of this scenegraph
         *
         * @return
         * The root of this scenegraph
         */
        INode *getRoot();

        /**
         * @brief getPolygonMeshes
         * Get a mapping of all (name,mesh) pairs that have been added to this
         * scenegraph. This function is useful in case all meshes of one
         * scenegraph have to be added to another in an attempt to merge the
         * two.
         *
         * @return
         * A mapping of all (name,mesh) pairs in the scenegraph
         */
        virtual map<string,util::PolygonMesh<VertexType> *> getPolygonMeshes()=0;

        /**
         * @brief getNodes
         * Get a mapping of all (name,INode) pair for all nodes in this
         * scenegraph. This function is useful in case all meshes of one
         * scenegraph have to be added to another in an attempt to merge the
         * two graphs.
         *
         * @return
         * A mapping of all (name,INode) pairs in the scenegraph
         */
        virtual map<string,INode *> getNodes()=0;

        /**
         * @brief addTexture
         * Add a new texture to the scenegraph
         *
         * @param name
         * The name of the new texture
         *
         * @param path
         * The path to the texture file
         */
        virtual void addTexture(const string& name,const string& path)=0;

        /**
         * @brief dispose
         * Called whenever the scenegraph is destroyed
         */
        virtual void dispose()=0;
    };
}

#endif
