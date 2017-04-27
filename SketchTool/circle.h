#ifndef CIRCLE_H
#define CIRCLE_H
#include "shape.h"

class Circle : public Shape
{
public:
    //Constructor
    Circle();
    Circle(float, float, float, float);

    //Setters
    /**
     * @brief set_radius
     * Sets the raidus of this circle
     *
     * @param val
     * The desired radius
     */
    void set_radius(float val) {radius = val;}

    //Getters
    /**
     * @brief get_radius
     * Gets the radius of this circle
     *
     * @return
     * The radius of this circle
     */
    float get_radius() {return radius;}

private:
    /**
     * @brief radius
     * The raidus of the circle
     */
    float radius;
};


#endif // CIRCLE_H
