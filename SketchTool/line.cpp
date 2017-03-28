#include "line.h"

/**
 * @brief Line::Line
 * Default constructor -- sets line error to max value
 */
Line::Line()
{
    //Do Nothing
    error = 999999999999.0f;
}

/**
 * @brief Line::Line
 * Parametrized Constructor -- Allows line creation provided we have error
 * value, starting point, end point and a length
 *
 * @param e
 * The error of the line
 *
 * @param sp
 * The starting point of the line
 *
 * @param ep
 * The end point of the line
 *
 * @param l
 * The length of the line
 */
Line::Line(float e, pair<float,float> sp, pair<float,float> ep, float l)
{
    this->set_error(e);
    this->setStartPoint(sp);
    this->setEndPoint(ep);
    this->setLength(l);

}
