#ifndef _VERTEXATTRIB_H_
#define _VERTEXATTRIB_H_

#include <glm/glm.hpp>
#include "IVertexData.h"



/*
 * This class represents the attributes of a single vertex. It is useful in building
 * PolygonMesh objects for many examples.
 *
 * It implements the IVertexData interface so that it can be converted into an array
 * of floats, to work with OpenGL buffers
 */
class VertexAttrib:public util::IVertexData
{


public:
    VertexAttrib()
    {
        position = glm::vec4(0,0,0,1);
        normal = glm::vec4(0,0,0,0);
        texcoord = glm::vec4(0,0,0,1);
    }

    ~VertexAttrib(){}


    /**
     * @brief hasData
     * This vertex has three attributes: Position, normal and texcoord. An
     * attribute name is passed to this function and if it is one of these
     * three attributes, true is returned.
     *
     * @param attribName
     * Name of attribute in question
     *
     * @return
     * Returns "true" if attribute one of the three contained in vertex
     * Returns "false" otherwise
     */
    bool hasData(string attribName)
    {

        if ((attribName == "position")
                || (attribName == "normal")
                || (attribName == "texcoord"))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    /**
     * @brief getData
     * Returns the values for a given attribute
     *
     * @param attribName
     * The attribute whose values will be returned
     *
     * @return
     * Returns a vector containing the values for the passed attribute
     */
    vector<float> getData(string attribName) throw(runtime_error)
    {
        vector<float> result;
        stringstream message;


        if (attribName == "position")
        {
            result.push_back(position.x);
            result.push_back(position.y);
            result.push_back(position.z);
            result.push_back(position.w);
        }
        else if (attribName == "normal")
        {
            result.push_back(normal.x);
            result.push_back(normal.y);
            result.push_back(normal.z);
            result.push_back(normal.w);
        }
        else if (attribName == "texcoord")
        {
            result.push_back(texcoord.x);
            result.push_back(texcoord.y);
            result.push_back(texcoord.z);
            result.push_back(texcoord.w);
        }
        else
        {
            message << "No attribute: " << attribName << " found!";
            throw runtime_error(message.str());
        }

        return result;
    }

    /**
     * @brief setData
     * Sets the data for a given attribute
     *
     * @param attribName
     * The name of the attribute whose data will be set
     *
     * @param data
     * The data to store for the passed attribute, represented as a vector of
     * floats
     */
    void setData(string attribName, const vector<float>& data) throw(runtime_error)
    {
        stringstream message;

        if (attribName == "position")
        {
            position = glm::vec4(0,0,0,1);
            switch (data.size()) {
            case 4: position.w = data[3];
            case 3: position.z = data[2];
            case 2: position.y = data[1];
            case 1: position.x = data[0];
                break;
            default:
                message << "Too much data for attribute: " << attribName;
                throw runtime_error(message.str());
            }
        }
        else if (attribName == "normal")
        {
            normal = glm::vec4(0,0,0,0);
            switch (data.size()) {
            case 4: normal.w = data[3];
            case 3: normal.z = data[2];
            case 2: normal.y = data[1];
            case 1: normal.x = data[0];
                break;
            default:
                message << "Too much data for attribute: " << attribName;
                throw runtime_error(message.str());
            }
        }
        else if (attribName == "texcoord")
        {
            texcoord = glm::vec4(0,0,0,1);
            switch (data.size()) {
            case 4: texcoord.w = data[3];
            case 3: texcoord.z = data[2];
            case 2: texcoord.y = data[1];
            case 1: texcoord.x = data[0];
                break;
            default:
                message << "Too much data for attribute: " << attribName;
                throw runtime_error(message.str());
            }
        }
        else
        {
            message << "Attribute: " << attribName << " unsupported!";
            throw runtime_error(message.str());
        }
    }

    /**
     * @brief getAllAttributes
     * Used to get the names of all attributes stored at vertex
     *
     * @return
     * A vector of strings containing the names of all attributes stored at
     * vertex
     */
    vector<string> getAllAttributes()
    {
        vector<string> attributes;

        attributes.push_back("position");
        attributes.push_back("normal");
        attributes.push_back("texcoord");
        return attributes;
    }

private:
    glm::vec4 position;
    glm::vec4 normal;
    glm::vec4 texcoord;
};

#endif
