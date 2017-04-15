#ifndef CLUSTER_H
#define CLUSTER_H

#include "line.h"

class Cluster
{
public:

    //Setters
    void setRaidus(float val) {radius = val;}
    void setOrigin(pair<float,float> val) {origin = val;}
    void setOriginX(float val) {origin.first = val;}
    void setOriginY(float val) {origin.second = val;}

    //Getters
    float getRadius() {return radius;}
    pair<float,float> getOrigin() {return origin;}
    float getOriginX() {return origin.first;}
    float getOriginY() {return origin.second;}

    //Helpers
    bool isPointInCluster(pair<float,float>);


private:
    float radius = 100.0f;
    pair<float,float> origin;

};

#endif // CLUSTER_H
