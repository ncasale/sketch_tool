#ifndef _INODE_H_
#define _INODE_H_

#include "OpenGLFunctions.h"
#include <glm/glm.hpp>
#include "Light.h"
#include "Material.h"
#include <vector>
#include <stack>
#include <string>

using namespace std;



namespace sgraph
{
  class Scenegraph;
  class GLScenegraphRenderer;

  enum NodeType{
      GROUP,
      TRANSFORM,
      LEAF
  };

  /**
 * This interface represents all the operations offered by any type of node in our scenegraph.
 * Not all types of nodes are able to offer all types of operations.
 * This is implemented by the sgraph::AbstractNode throwing an exception for all such methods, and
 * appropriate nodes overriding these methods
 *
 * \author Amit Shesh
 */
  class INode
  {

  public:

      //Structure used to hold MyTreeWidgetItem data
      struct GeneratedItem
      {
          INode* node;
          NodeType type;
          INode* parent;
      };

     /**
     * @brief getNode
     * In the scenegraph rooted at this node, get the node whose name is as given
     *
     * @param name
     * The name of the node to be retrieved
     *
     * @return
     * The node with the specified name
     */
    virtual INode *getNode(const string& name)=0;
    INode(){}

    virtual ~INode(){}

    /**
     * Draw the scene graph rooted at this node, using the modelview stack and context
     * \param context the generic renderer context {@link sgraph.IScenegraphRenderer}
     * \param modelView the stack of modelview matrices
     */
    /**
     * @brief draw
     * Draw the scenegraph rooted at this node, using the modelview and context
     *
     * @param context
     * The generic renderer context {@link sgraph.IScenegraphRenderer}
     *
     * @param modelView
     * The stack of modelview transformations applied to this node
     */
    virtual void draw(GLScenegraphRenderer& context,stack<glm::mat4>& modelView)=0;\

    /**
     * @brief clone
     * Returns a deep copy of the scenegraph subtree rooted at this node
     *
     * @return
     * A reference to the root of the copied subtree
     */
    virtual INode *clone()=0;

    /**
     * @brief setParent
     * Set the parent of this node. Each node except the root has a parent
     *
     * @param parent
     * A reference to the desiered parent
     */
    virtual void setParent(INode *parent)=0;

    virtual INode *getParent()=0;

    /**
     * @brief setScenegraph
     * Traverse the scenegraph rooted at this node and store references to the
     * scenegraph object
     *
     * @param graph
     * A reference to the scenegraph object of which this tree is a part
     */
    virtual void setScenegraph(sgraph::Scenegraph *graph)=0;

    /**
     * @brief setName
     * Set the name of this node. The name is not guaranteed to be unique in
     * the tree, but it should be.
     *
     * @param name
     * The name of this node
     */
    virtual void setName(const string& name)=0;

    /**
     * @brief getName
     * Get the name of this node
     *
     * @return
     * The name of this node
     */
    virtual string getName()=0;

    /**
     * @brief addChild
     * Add a child to this node. Not all types of nodes have children. If the
     * node cannot have a child, this function throws a runtime exception
     *
     * @param node
     * The child to add to this node
     */
    virtual void addChild(INode *node) throw(runtime_error)=0;

    /**
     * @brief setTransform
     * Set the transformation associated with this node. Not all nodes can have
     * transformations. If the node cannot store a transformations, this
     * function throws a runtime error
     *
     * @param m
     * The transformation matrix associated with this transformation
     */
    virtual void setTransform(const glm::mat4& m) throw(runtime_error)=0;

    /**
     * @brief setAnimationTransform
     * Set the animation transform associated with this node. Not all nodes can
     * have animation transforms. If this node cannot store an animation
     * transform, this function throws a runtime exception
     *
     * @param m
     * The animation transformation matrix associated with this node
     */
    virtual void setAnimationTransform(const glm::mat4& m) throw(runtime_error)=0;


    /**
     * @brief setMaterial
     * Set the material associated with this node. Not all nodes can have
     * materials. If this node cannot have a material, this function throws
     * a runtime error
     *
     * @param m
     * The material object associated with this node
     */
    virtual void setMaterial(const util::Material& m) throw(runtime_error)=0;

    /**
     * @brief setTextureName
     * Sets the texture associated with this node. Not all nodes can have a
     * texture. If this node can't have a texture, this function throws a
     * runtime error.
     *
     * @param name
     * Name of the texture to be applied to this node
     */
    virtual void setTextureName(const string& name) throw(runtime_error)=0;

    /**
     * @brief addLight
     * Add a new light to this node
     *
     * @param l
     * The light to be added
     */
    virtual void addLight(const util::Light& l)=0;


    /**
     * @brief clearChildren
     * Clear the children of this node
     */
    virtual void clearChildren()=0;

    /**
     * @brief getLightsInView
     * Return a list of all lights in this scenegraph in the view coordinate
     * system. This function is called on the root of the scenegraph. It is
     * assumed that the modelview.top() is set to the world-to-view
     * transformation
     *
     * @param modelview
     * The stack of transformations applied to the scenegraph
     *
     * @return
     * A vector containing all the lights in the scenegraphs view coordinate
     * system
     */
    virtual vector<util::Light> getLightsInView(stack<glm::mat4>& modelview)=0;

    /**
     * @brief saveToXML
     * Saves node to the output file
     *
     * @param output_file
     * The output file to which this function saves the node
     */
    virtual void saveToXML(fstream& output_file) = 0;

    /**
     * @brief getNodeType
     * Used to get the node type of this particular node
     *
     * @return
     * Returns the node type (Leaf, Transform, Group, etc) of this node
     */
    virtual NodeType getNodeType() = 0;

    /**
     * @brief getTextureMatrix
     * Returns the texture matrix of this node (if it has one)
     *
     * @return
     * The texture matrix (if it exists) of this node
     */
    virtual glm::mat4& getTextureMatrix() = 0;


    /**
     * @brief setTextureMatrix
     * Sets the texture matrix of the node
     *
     * @param mat
     * The texture matrix being assigned to the node
     */
    virtual void setTextureMatrix(const glm::mat4&) = 0;

    /**
     * @brief changeNodeTexture
     * Changes the texture of this node
     *
     * @param texture_name
     * The name of the texture to change to. This must be included in XML file
     */
    virtual void changeNodeTexture(const string& texture_name) throw(runtime_error) = 0;

    /**
     * @brief revertNodeTexture
     * Changes texture to previous texture.
     */
    virtual void revertNodeTexture() throw(runtime_error) = 0;

    /**
     * @brief generateScenegraphTreeView
     * Recurses through scenegraph to get information required to generate the
     * tree representation of the sgraph in the right-side pane.
     *
     * @param ret_vec
     * A vector containing GeneratedItems - each GeneratedItem contains info
     * about a particular node that will be represented in the tree view.
     */
    virtual void generateScenegraphTreeView(vector<GeneratedItem>& ret_vec) throw(runtime_error) = 0;

  };
}

#endif
