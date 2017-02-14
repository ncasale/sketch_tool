#ifndef _LEAFNODE_H_
#define _LEAFNODE_H_

#include "AbstractNode.h"
#include "OpenGLFunctions.h"
#include "Material.h"
#include "glm/glm.hpp"
#include <map>
#include <stack>
#include <string>
using namespace std;

namespace sgraph
{

/**
 * This node represents the leaf of a scene graph. It is the only type of node that has
 * actual geometry to render.
 * \author Amit Shesh
 */
class LeafNode: public AbstractNode
{

protected:
    /**
     * The name of the object instance that this leaf contains. All object instances are stored
     * in the scene graph itself, so that an instance can be reused in several leaves
     */
    string objInstanceName;

    /**
     * The material associated with the object instance at this leaf
     */
    util::Material material;

    /**
     * The name of the texture associated with the object instance at tbis leaf
     */
    string textureName;

public:
    LeafNode(const string& instanceOf,sgraph::Scenegraph *graph,const string& name)
        :AbstractNode(graph,name)
    {
        this->objInstanceName = instanceOf;
    }
	
	~LeafNode(){}

    /**
     * @brief setMaterial
     * Set the material of each vertex in this object
     *
     * @param mat
     * The material to set
     */
    void setMaterial(const util::Material& mat) throw(runtime_error)
    {
        material = mat;
    }

    /**
     * @brief setTextureName
     * Set the texture ID of the texture to be used for this leaf
     *
     * @param name
     * Name of desired texture
     */
    void setTextureName(const string& name) throw(runtime_error)
    {
        textureName = name;
    }

    /**
     * @brief getMaterial
     * Get the material associated with the object instance at this leaf
     *
     * @return
     * The material associated with the object instance at this leaf
     */
    util::Material getMaterial()
    {
        return material;
    }

    /**
     * @brief clone
     * Makes a copy of this leaf node
     *
     * @return
     * A copy of this leaf node
     */
    INode *clone()
    {
        LeafNode *newclone = new LeafNode(this->objInstanceName,scenegraph,name);
        newclone->setMaterial(this->getMaterial());
        return newclone;
    }

    /**
     * @brief draw
     * Delegates to the scenegraph for rendering. This has two advantages:
     *  1. It keeps the leaf light
     *  2. It abstracts the actual drawing to the specific implementation of
     *     the scenegraph renderer
     *
     * @param context
     * The generic renderer context {@link sgraph::IScenegraphRenderer}
     *
     * @param modelView
     * The stack of modelview matrices
     */
    void draw(GLScenegraphRenderer& context,stack<glm::mat4>& modelView) throw(runtime_error)
    {
        if (objInstanceName.length()>0)
        {
            context.drawMesh(objInstanceName,material,textureName,modelView.top());
        }
    }

    /**
     * @brief clearChildren
     * Throws a runetime error since leaf nodes cannot have children
     */
    void clearChildren() throw(runtime_error)
    {
        throw runtime_error("Not a group node -- cannot clear children");
    }

    /**
     * @brief saveToXML
     * Saves an XML snippet for the object at this leaf node into the specified
     * output file.
     *
     * @param output_file
     * An fstream object representing the XML output file.
     */
    void saveToXML(fstream& output_file)
    {
        if(objInstanceName.length() > 0)
        {

            //Material Properties
            glm::vec4 ambient = material.getAmbient();
            glm::vec4 diffuse = material.getDiffuse();
            glm::vec4 specular = material.getSpecular();
            float shininess = material.getShininess();

            //Add object tag
            if(name == "")
                output_file << "<object instanceof =\"" << objInstanceName << "\">" << endl;
            else
            {
                string tag_string = "<object instanceof=\"" + objInstanceName +
                        "\" name=\"" + name + "\">";
                output_file << tag_string << endl;
            }
            //Add material tag
            output_file << "<material>" << endl;
            //Add material properties
            output_file << "<ambient>" << to_string(ambient[0]) << " " << to_string(ambient[1])
                    << " " << ambient[2] << "</ambient>" << endl;
            output_file << "<diffuse>" << to_string(diffuse[0]) << " " << to_string(diffuse[1])
                    << " " << diffuse[2] << "</diffuse>" << endl;
            output_file << "<specular>" << to_string(specular[0]) << " " << to_string(specular[1])
                    << " " << specular[2] << "</specular>" << endl;
            output_file << "<shininess>" << to_string(shininess) << "</shininess>" << endl;
            //Add end material tag
            output_file << "</material>" << endl;
            //Add end object rag
            output_file << "</object>" << endl;
        }
    }

    NodeType getNodeType()
    {
        return LEAF;
    }
};
}
#endif
