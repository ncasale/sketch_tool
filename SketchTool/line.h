#ifndef LINE_H
#define LINE_H
#include "QPoint"
#include "shape.h"

using namespace std;

class Line : public Shape
{
public:
    Line();
    Line(float, pair<float,float>, pair<float,float>, float);

    //Setters
    void setStartPoint(pair<float,float> val) {start_point = val;}
    void setEndPoint(pair<float,float> val) {end_point = val;}
    void setLength(float val) {length = val;}

    //Getters
    pair<float,float> getStartPoint() {return start_point;}
    pair<float,float> getEndPoint() {return end_point;}
    float getLength() {return length;}

private:
    pair<float,float> start_point;
    pair<float,float> end_point;
    float length;


};

#endif // LINE_H