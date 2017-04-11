#ifndef SGRAPHITEMINFO_H
#define SGRAPHITEMINFO_H

#include <string>

using namespace std;

class SGraphItemInfo{
public:
    //Constructor
    SGraphItemInfo();
    SGraphItemInfo(string, float, float, string);

    //Setters
    void setNodeType(string val){node_type = val;}
    void setDrawX(float val){draw_x = val;}
    void setDrawY(float val){draw_y = val;}
    void setNodeName(string val){node_name = val;}

    //Getters
    string getNodeType(){return node_type;}
    string getNodeName(){return node_name;}
    float getDrawX(){return draw_x;}
    float getDrawY(){return draw_y;}

private:
    string node_type;
    float draw_x;
    float draw_y;
    string node_name;
};


#endif // SGRAPHITEMINFO_H
