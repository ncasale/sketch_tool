#include "circle.h"

//Constructor
/**
 * @brief Circle::Circle
 * Default constructor -- does nothing.
 */
Circle::Circle()
{
    //Do Nothing
}

/**
 * @brief Circle::Circle
 * Constructor allows creation of circle with defined parameters
 *
 * @param e
 * The error of the circle
 *
 * @param cx
 * The center x-coordinate
 *
 * @param cy
 * The center y-coordinate
 *
 * @param r
 * The radius of the circle
 */
Circle::Circle(float e, float cx, float cy, float r)
{
    error = e;
    center_x = cx;
    center_y = cy;
    radius = r;
}
