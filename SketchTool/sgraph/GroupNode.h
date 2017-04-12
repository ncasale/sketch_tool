#ifndef _GROUPNODE_H_
#define _GROUPNODE_H_

#include "OpenGLFunctions.h"
#include "AbstractNode.h"
#include "glm/glm.hpp"
#include "Light.h"
#include <vector>
#include <stack>
#include <string>
using namespace std;

namespace sgraph
{
  /**
 * This class represents a group node in the scenegraph. A group node is simply a logical grouping
 * of other nodes. It can have an arbitrary number of children. Its children can be nodes of any type
 * \author Amit Shesh
 */
  class GroupNode:public AbstractNode
  {

  protected:
    /**
     * A list of this node's children
     */
    vector<INode *> children;

  public:
    GroupNode(sgraph::Scenegraph *graph,const string& name)
      :AbstractNode(graph,name)
    {
        scenegraph->addNode(name, this);
    }

    ~GroupNode()
    {
      for (int i=0;i<children.size();i++)
        {
          delete children[i];
        }
    }

    /**
     * @brief clearChildren
     * Will clear all children of this node
     */
    void clearChildren() throw(runtime_error)
    {
        children.clear();
    }

    /**
     * Searches recursively into its subtree to look for node with specified name.
     * \param name name of node to be searched
     * \return the node whose name this is if it exists within this subtree, null otherwise
     */
    /**
     * @brief getNode
     * Searches recursively into this node's subtree to look for a node with
     * specified name
     *
     * @param name
     * The name of the node to be found
     *
     * @return
     * Node with specified name, null if node not found
     */
    INode *getNode(const string& name)
    {
      INode *n = AbstractNode::getNode(name);
      if (n!=NULL)
        {
          return n;
        }

      int i=0;
      INode *answer = NULL;

      while ((i<children.size()) && (answer == NULL))
        {
          answer = children[i]->getNode(name);
          i++;
        }
      return answer;
    }

    glm::mat4& getTextureMatrix() throw(runtime_error)
    {
        throw(runtime_error("Not a leaf node -- cannot get texture matrix"));
    }

    void setTextureMatrix(const glm::mat4 &mat) throw(runtime_error)
    {
        throw(runtime_error("Not a leaf node -- cannot set texture matix"));

    }

    /**
     * @brief setScenegraph
     * Sets the reference to the scenegraph object for this node, and then
     * recurses down to children to do the same
     *
     * @param graph
     * The reference to the scenegraph object
     */
    void setScenegraph(sgraph::Scenegraph *graph)
    {
      AbstractNode::setScenegraph(graph);
      for (int i=0;i<children.size();i++)
        {
          children[i]->setScenegraph(graph);
        }
    }

    /**
     * @brief draw
     * To draw this node, we simply delegate drawing to each of its children
     *
     * @param context
     * The generic renderer context sgraph::IScenegraphRenderer
     *
     * @param modelView
     * The stack of modelview matrices (current transformation applied to node)
     */
    void draw(GLScenegraphRenderer& context,stack<glm::mat4>& modelView)
    {
      for (int i=0;i<children.size();i++)
        {
          children[i]->draw(context,modelView);
        }
    }


    /**
     * @brief saveToXML
     * Used to save XML for this node to the specified output file. Recursively
     * instructs children to do the same.
     *
     * @param output_file
     * The file to which the generated XML is saved
     */
    void saveToXML(fstream& output_file)
    {
        //Add group tag to output file
        if(name == "")
            output_file << "<group>" << endl;
        else
        {
            string tag_string = "<group name=\"" + name + "\">";
            output_file << tag_string << endl;
        }
        for(int i=0; i < children.size(); i++)
        {
            children[i]->saveToXML(output_file);
        }
        //Add ending group tag to output file
        output_file << "</group>" << endl;
    }

    /**
     * @brief clone
     * Makes a deep copy of the subtree rooted at this node
     *
     * @return
     * A deep copy of the subtree rooted at this node
     */
    INode *clone()
    {
      vector<INode *> newc;

      for (int i=0;i<children.size();i++)
        {
          newc.push_back(children[i]->clone());
        }

      GroupNode *newgroup = new GroupNode(scenegraph,name);

      for (int i=0;i<children.size();i++)
        {
          try
          {
            newgroup->addChild(newc[i]);
          }
          catch (runtime_error e)
          {

          }
        }
      return newgroup;
    }

    /**
     * @brief addChild
     * Add a child to this node
     *
     * @param child
     * The child to add to this node
     */
    void addChild(INode *child) throw(runtime_error)
    {
      children.push_back(child);
      child->setParent(this);
    }

    /**
     * @brief getChildren
     * Get a list of all this node's children
     *
     * @return
     * A list of all this node's children
     */
    vector<INode *> getChildren()
    {
      return children;
    }

    /**
     * @brief getLightsInView
     * Overridden version of @link{AbstractNode}. This version first collects
     * all the lights from its children, and then appends to them the lights
     * from this node as well. This function uses the original version to
     * get the lights in this node.
     *
     * @param modelview
     * The stack of transformations currently applied to this node
     *
     * @return
     * A vector containing the lights currently in the view
     */
    vector<util::Light> getLightsInView(stack<glm::mat4>& modelview)
    {
      vector<util::Light> lights,templights;
      for (unsigned int i = 0; i < children.size(); i++)
        {
          templights = children[i]->getLightsInView(modelview);
          for (unsigned int j=0;j<templights.size();j++)
            {
              lights.push_back(templights[j]);
            }
        }
      //now get the lights from this node's lights
      templights = AbstractNode::getLightsInView(modelview);
      for (unsigned int i=0;i<templights.size();i++)
        {
          lights.push_back(templights[i]);
        }
      return lights;
    }

    NodeType getNodeType()
    {
        return GROUP;
    }

    void changeNodeTexture(const string& texture_name) throw(runtime_error)
    {
        throw runtime_error("Group Node has no texture to change.");
    }

    void revertNodeTexture() throw(runtime_error)
    {
        throw runtime_error("Group Node has no texture to revert.");
    }

  };
}

#endif
