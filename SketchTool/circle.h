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
    void set_radius(float val) {radius = val;}

    //Getters
    float get_radius() {return radius;}

private:
    float radius;
};


#endif // CIRCLE_H
