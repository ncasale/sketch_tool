#ifndef LINE_H
#define LINE_H
#include "QPoint"
#include "shape.h"

using namespace std;

class Line : public Shape
{
public:
    Line();
    Line(float, pair<float,float>, pair<float,float>);

    //Setters
    /**
     * @brief setStartPoint
     * Sets the start point of this line
     *
     * @param val
     * The desired start point
     */
    void setStartPoint(pair<float,float> val) {start_point = val;}

    /**
     * @brief setEndPoint
     * Sets the end point of this line
     *
     * @param val
     * The desired end point
     */
    void setEndPoint(pair<float,float> val) {end_point = val;}


    //Getters
    /**
     * @brief getStartPoint
     * Gets the start point of the line
     *
     * @return
     * The start point of the line
     */
    pair<float,float> getStartPoint() {return start_point;}

    /**
     * @brief getEndPoint
     * Gets the end point of the line
     *
     * @return
     * The end point of the line
     */
    pair<float,float> getEndPoint() {return end_point;}

private:
    /**
     * @brief start_point
     * The starting x,y coordinate of the line
     */
    pair<float,float> start_point;

    /**
     * @brief end_point
     * The ending x,y coordinate of the line
     */
    pair<float,float> end_point;


};

#endif // LINE_H
