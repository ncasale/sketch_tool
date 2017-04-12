#ifndef _TRANSFORMNODE_H_
#define _TRANSFORMNODE_H_

#include "AbstractNode.h"
#include "OpenGLFunctions.h"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Light.h"
using namespace std;
#include <vector>
#include <stack>
#include <string>
using namespace std;

namespace sgraph
{

enum transformation_type{
    TRANSLATE,
    ROTATE,
    SCALE
};

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
    vector<pair<transformation_type,vector<float>>> transformation_order;

    /**
      * The translation data associated with this transform node
      */
    vector<float> translation_data = {0.0f, 0.0f, 0.0f};


    /**
      * The rotation data for the x axis
      */
    vector<float> x_rotation_data = {0.0f, 1.0f, 0.0f, 0.0f};


    /**
      * The rotation data for the y axis
      */
    vector<float> y_rotation_data = {0.0f, 0.0f, 1.0f, 0.0f};


    /**
      * The rotation data for the z axis
      */
    vector<float> z_rotation_data = {0.0f, 0.0f, 0.0f, 1.0f};


    /**
      * The scale data
      */
    vector<float> scale_data = {1.0f, 1.0f, 1.0f};

  public:
    /**
     * @brief TransformNode
     * Default constructor. Sets transforms to default values and nulls out child.
     *
     * @param graph
     * The scenegraph to which this node belongs.
     *
     * @param name
     * The name of this node
     */
    TransformNode(sgraph::Scenegraph *graph,const string& name)
      :AbstractNode(graph,name)
    {
      this->transform = glm::mat4(1.0);
      animation_transform = glm::mat4(1.0);
      scenegraph->addNode(name, this);
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

    glm::mat4& getTextureMatrix() throw(runtime_error)
    {
        throw(runtime_error("Not a leaf node -- cannot get texture matrix"));
    }

    void setTextureMatrix(const glm::mat4 &mat) throw(runtime_error)
    {
        throw(runtime_error("Not a leaf node -- cannot set texture matix"));

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
        if(name == "")
            output_file << "<transform> " << endl;
        else
        {
            string tag_string = "<transform name=\"" + name + "\">";
            output_file << tag_string << endl;
        }

        //Add set tag
        output_file << "<set>" << endl;
        //Iterate through transformation_order and apply correct tags
        string xml_string = "";

        //Apply translate first
        xml_string = "<translate> " + to_string(translation_data[0]) + " " +
                to_string(translation_data[1]) + " " + to_string(translation_data[2]) +
                "</translate>";
        output_file << xml_string << endl;

        //Apply rotation second -- start with x, then y, then z
        xml_string = "<rotate> " + to_string(x_rotation_data[0]) + " " +
                to_string(x_rotation_data[1]) + " " + to_string(x_rotation_data[2]) + " "
                + to_string(x_rotation_data[3]) + "</rotate>";
        output_file << xml_string << endl;

        xml_string = "<rotate> " + to_string(y_rotation_data[0]) + " " +
                to_string(y_rotation_data[1]) + " " + to_string(y_rotation_data[2]) + " "
                + to_string(y_rotation_data[3]) + "</rotate>";
        output_file << xml_string << endl;

        xml_string = "<rotate> " + to_string(z_rotation_data[0]) + " " +
                to_string(z_rotation_data[1]) + " " + to_string(z_rotation_data[2]) + " "
                + to_string(z_rotation_data[3]) + "</rotate>";
        output_file << xml_string << endl;

        //Apply scale last
        xml_string = "<scale> " + to_string(scale_data[0]) + " " +
                to_string(scale_data[1]) + " " + to_string(scale_data[2]) +
                "</scale>";
        output_file << xml_string << endl;

        //Close set tag
        output_file << "</set>" << endl;

        //Recurse to saving children
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

    /**
     * @brief addScale
     * Used to apply this scale to the node's transform
     *
     * @param x_scale
     * The scaling factor in the x direction
     *
     * @param y_scale
     * The scaling factor in the y direction
     *
     * @param z_scale
     * The scaling factor in the z direction
     */
    void addScale(float x_scale, float y_scale, float z_scale)
    {
        //Multiply scale into existing scale
        scale_data[0] *= x_scale;
        scale_data[1] *= y_scale;
        scale_data[2] *= z_scale;

        //Apply scale to node's transform
        transform *= glm::scale(glm::mat4(1.0f), glm::vec3(x_scale, y_scale, z_scale));
    }

    /**
     * @brief addRotation
     * Used to apply this rotation to node's transform.
     *
     * @param angle
     * The angle of the rotation in degrees
     *
     * @param x_axis
     * 1 if rotation of angle @param angle about x-axis should be applied , 0 otherwise
     *
     * @param y_axis
     * 1 if rotation of angle @param angle about y-axis should be applied , 0 otherwise
     *
     * @param z_axis
     * 1 if rotation of angle @param angle about z-axis should be applied , 0 otherwise
     */
    void addRotation(float angle, float x_axis, float y_axis, float z_axis)
    {
        //Apply rotation to each axis
        if(x_axis == 1.0f)
        {
            x_rotation_data[0] += angle;
        }
        if(y_axis == 1.0f)
        {
            y_rotation_data[0] += angle;
        }
        if(z_axis == 1.0f)
        {
            z_rotation_data[0] += angle;
        }

        //Apply rotation to node's transformation
        transform *= glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(x_axis, y_axis, z_axis));

    }

    /**
     * @brief addTranslation
     * Used to apply this translation to node's transform.
     *
     * @param x_trans
     * Amount to translate along the x-axis
     *
     * @param y_trans
     * Amount to translate along the y-axis
     *
     * @param z_trans
     * Amount to translate along the z-axis
     */
    void addTranslation(float x_trans, float y_trans, float z_trans)
    {
        //Apply translation to existing translation data
        translation_data[0] += x_trans;
        translation_data[1] += y_trans;
        translation_data[2] += z_trans;

        //Apply translation to transformation member
        transform *= glm::translate(glm::mat4(1.0f), glm::vec3(x_trans, y_trans, z_trans));
    }

    NodeType getNodeType()
    {
        return TRANSFORM;
    }

    void changeNodeTexture(const string& texture_name) throw(runtime_error)
    {
        throw runtime_error("Transform Node has no texture to change.");
    }

    void revertNodeTexture() throw(runtime_error)
    {
        throw runtime_error("Transform Node has no texture to revert.");
    }
  };
}
#endif
