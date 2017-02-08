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
    /**
     * The name of the object instance that this leaf contains. All object instances are stored
     * in the scene graph itself, so that an instance can be reused in several leaves
     */
protected:
    string objInstanceName;
    /**
     * The material associated with the object instance at this leaf
     */
    util::Material material;

    string textureName;

public:
    LeafNode(const string& instanceOf,sgraph::Scenegraph *graph,const string& name)
        :AbstractNode(graph,name)
    {
        this->objInstanceName = instanceOf;
    }
	
	~LeafNode(){}



    /*
	 *Set the material of each vertex in this object
	 */
    void setMaterial(const util::Material& mat) throw(runtime_error)
    {
        material = mat;
    }

    /**
     * Set texture ID of the texture to be used for this leaf
     * \param name
     */
    void setTextureName(const string& name) throw(runtime_error)
    {
        textureName = name;
    }

    /*
     * gets the material
     */
    util::Material getMaterial()
    {
        return material;
    }

    INode *clone()
    {
        LeafNode *newclone = new LeafNode(this->objInstanceName,scenegraph,name);
        newclone->setMaterial(this->getMaterial());
        return newclone;
    }


    /**
     * Delegates to the scene graph for rendering. This has two advantages:
     * <ul>
     *     <li>It keeps the leaf light.</li>
     *     <li>It abstracts the actual drawing to the specific implementation of the scene graph renderer</li>
     * </ul>
     * \param context the generic renderer context {@link sgraph.IScenegraphRenderer}
     * \param modelView the stack of modelview matrices
     * \throws runtime_error
     */
    void draw(GLScenegraphRenderer& context,stack<glm::mat4>& modelView) throw(runtime_error)
    {
        if (objInstanceName.length()>0)
        {
            context.drawMesh(objInstanceName,material,textureName,modelView.top());
        }
    }

    void clearChildren() throw(runtime_error)
    {
        throw runtime_error("Not a group node -- cannot clear children");
    }

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
            output_file << "<object instanceof =\"" << objInstanceName << "\">" << endl;
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
};
}
#endif
