#ifndef _TRANSFORMNODE_H_
#define _TRANSFORMNODE_H_

#include "AbstractNode.h"
#include "OpenGLFunctions.h"
#include "glm/glm.hpp"
#include "Light.h"
using namespace std;
#include <vector>
#include <stack>
#include <string>
using namespace std;

namespace sgraph
{

  /**
 * This node represents a transformation in the scene graph. It has only one child. The transformation
 * can be viewed as changing from its child's coordinate system to its parent's coordinate system
 * This also stores an animation transform that can be tweaked at runtime
 * \author Amit Shesh
 */
  class TransformNode: public AbstractNode
  {
  protected:
      /**
       * Matrices storing the static and animation transformations separately, so that they can be
       * changed separately
       */
    glm::mat4 transform,animation_transform;

    /**
     * A reference to its only child
     */
    INode *child;

    /**
      * Add transformation container here
      */

  public:
    TransformNode(sgraph::Scenegraph *graph,const string& name)
      :AbstractNode(graph,name)
    {
      this->transform = glm::mat4(1.0);
      animation_transform = glm::mat4(1.0);
      child = NULL;
    }

    ~TransformNode()
    {
      if (child!=NULL)
        delete child;
    }

    /**
     * @brief clearChildren
     * Sets the child of this node to NULL
     */
    void clearChildren() throw(runtime_error)
    {
        child = NULL;
    }

    /**
     * @brief clone
     * Creates a deep copy of the subtree rooted at this node
     *
     * @return
     * A deep copy of the subtree rooted at this node
     */
    INode *clone()
    {
      INode *newchild;

      if (child!=NULL)
        {
          newchild = child->clone();
        }
      else
        {
          newchild = NULL;
        }

      TransformNode *newtransform = new TransformNode(scenegraph,name);
      newtransform->setTransform(this->transform);
      newtransform->setAnimationTransform(animation_transform);

      if (newchild!=NULL)
        {
          try
          {
            newtransform->addChild(newchild);
          }
          catch (runtime_error e)
          {

          }
        }
      return newtransform;
    }

    /**
     * @brief getNode
     * Determines if this node has the specified name and returns itself if so.
     * Otherwise, it recurses into its only child
     *
     * @param name
     * The name of the node to be searched
     *
     * @return
     * The node with a name matching @param name. Null otherwise.
     */
    INode *getNode(const string& name)
    {
      INode *n = AbstractNode::getNode(name);
      if (n!=NULL)
        return n;

      if (child!=NULL)
        {
          return child->getNode(name);
        }

      return NULL;
    }

    /**
     * @brief addChild
     * Set the child of this node. This function will throw a runtime error if
     * this node already has a child
     *
     * @param child
     * The child to be assigned to this node
     */
    void addChild(INode *child) throw(runtime_error)
    {
      if (this->child!=NULL)
        throw runtime_error("Transform node already has a child");
      this->child = child;
      this->child->setParent(this);
    }

    /**
     * @brief draw
     * Draws the scenegraph rooted at this node. After preserving the current
     * top of the modelview stack, this "post-multiplies" its animation
     * transform and then its transform (in that order) to the top of the
     * modelview stack. This function then recurses to this node's child. When
     * the child is drawn, it restores the modelview matrix.
     *
     * @param context
     * The generic renderer context {@link sgraph::IScenegraphRenderer}
     *
     * @param modelView
     * The stack of modelview matrices
     */
    void draw(GLScenegraphRenderer& context,stack<glm::mat4>& modelView)
    {
      modelView.push(modelView.top());
      modelView.top() = modelView.top()
          * animation_transform
          * transform;
      if (child!=NULL)
        child->draw(context,modelView);
      modelView.pop();
    }

    void saveToXML(fstream& output_file)
    {
        //Add transform tag to file
        output_file << "<transform> " << endl;
        //Add set and default scale of 50
        output_file << "<set>" << endl << "<scale>50 50 50 </scale>" << endl << "</set>" << endl;
        if(child != NULL)
            child->saveToXML(output_file);
        //Add ending transform tag
        output_file << "</transform>" << endl;
    }

    /**
     * @brief setAnimationTransform
     * Sets the animation transform of this node
     *
     * @param mat
     * The animation transform of this node
     */
    void setAnimationTransform(const glm::mat4& mat) throw(runtime_error)
    {
      animation_transform = mat;
    }

    /**
     * @brief getTransform
     * Gets the transform at this node (not the animation transform)
     *
     * @return
     * The transform of this node (not the animation transform)
     */
    glm::mat4 getTransform()
    {
      return transform;
    }

    /**
     * @brief setTransform
     * Sets the transformation of this node
     *
     * @param t
     * The transformation of this node
     */
    void setTransform(const glm::mat4& t) throw(runtime_error)
    {
      this->transform = t;
    }

    /**
     * @brief getAnimationTransform
     * Gets the animation transform of this node
     *
     * @return
     * The animation transform of this node
     */
    glm::mat4 getAnimationTransform()
    {
      return animation_transform;
    }

    /**
     * @brief setScenegraph
     * Sets the scenegraph object of which this node is a part, and then
     * recurses to its child
     *
     * @param graph
     * A reference to the scenegraph object of which this tree is a part
     */
    void setScenegraph(sgraph::Scenegraph *graph)
    {
      AbstractNode::setScenegraph(graph);
      if (child!=NULL)
        {
          child->setScenegraph(graph);
        }
    }

    /**
     * @brief getLightsInView
     * Overridden version of @link{AbstractNode}. This version first modifies
     * the modelview matrix to include its transformation and animation
     * transformation. Then it collects all the lights from its child and
     * appends them to lights from this node.
     *
     * This function uses the @link{AbstractNode::getLightsInView} to get the
     * lights contained in this node.
     *
     * @param modelview
     * The stack of modelview matrices
     * @return
     */
    vector<util::Light> getLightsInView(stack<glm::mat4>& modelview)
    {
      vector<util::Light> lights,templights;

      modelview.push(modelview.top());
      modelview.top() = modelview.top() * animation_transform * transform;
      if (child != NULL)
        {
          templights = child->getLightsInView(modelview);
          for (unsigned int i=0;i<templights.size();i++)
            {
              lights.push_back(templights[i]);
            }
        }
      modelview.pop();

      //now get the lights from this node's lights
      templights = AbstractNode::getLightsInView(modelview);
      for (unsigned int i=0;i<templights.size();i++)
        {
          lights.push_back(templights[i]);
        }
      return lights;
    }
  };
}
#endif
