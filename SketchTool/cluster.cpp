#include "cluster.h"


bool Cluster::isPointInCluster(pair<float,float> point)
{
    //Check if point falls within radius of origin
    float point_x = point.first;
    float point_y = point.second;

    float xdist = abs(point_x - origin.first);
    float ydist = abs(point_y - origin.second);

    if(xdist <= radius && ydist <= radius)
        return true;
    else
        return false;
}
