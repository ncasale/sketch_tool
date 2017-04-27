#ifndef CLUSTER_H
#define CLUSTER_H

#include "line.h"

class Cluster
{
public:

    //Setters
    /**
     * @brief setRaidus
     * Sets the radius of the cluster
     *
     * @param val
     * The desired radius
     */

    void setRaidus(float val) {radius = val;}
    /**
     * @brief setOrigin
     * Sets the origin (center) of the cluster
     *
     * @param val
     * The desired origin
     */
    void setOrigin(pair<float,float> val) {origin = val;}

    /**
     * @brief setOriginX
     * Sets just the x coordinate of the origin
     *
     * @param val
     * The desired x coordinate
     */
    void setOriginX(float val) {origin.first = val;}

    /**
     * @brief setOriginY
     * Sets just the y coordinate of the origin
     *
     * @param val
     * The desired y coordinate
     */
    void setOriginY(float val) {origin.second = val;}

    //Getters
    /**
     * @brief getRadius
     * Gets the raidus of the cluster
     *
     * @return
     * The radius of the cluster
     */
    float getRadius() {return radius;}

    /**
     * @brief getOrigin
     * Gets the origin (center) of the cluster
     *
     * @return
     * The origin (center) of the cluster
     */
    pair<float,float> getOrigin() {return origin;}

    /**
     * @brief getOriginX
     * Gets just the x coordinate of the cluster origin
     *
     * @return
     * The x coordinate of the cluster origin
     */
    float getOriginX() {return origin.first;}

    /**
     * @brief getOriginY
     * Gets just the y coordinate of the cluster origin
     *
     * @return
     * The y coordinate of the cluster origin
     */
    float getOriginY() {return origin.second;}

    //Helpers
    /**
     * @brief isPointInCluster
     * Checks if the passed point lies within the cluster. I.e. does the point
     * fall within a circle with its center at the cluster's origin and a
     * radius equal to the cluster's radius
     *
     * @return
     * True if the point lies within the cluster, false otherwise.
     */
    bool isPointInCluster(pair<float,float>);


private:
    /**
     * @brief radius
     * The raidus of a circle originating from the origin of the cluster. Any
     * points that lie within this circle are considered to be part of the
     * cluster.
     */
    float radius = 150.0f;

    /**
     * @brief origin
     * The center point of the cluster.
     */
    pair<float,float> origin;

};

#endif // CLUSTER_H
