#ifndef SHAPE_H
#define SHAPE_H

class Shape
{
public:
    //Constructor
    Shape();

    //Setters
    void set_center_x(float val) {center_x = val;}
    void set_center_y(float val) {center_y = val;}
    void set_error(float val) {error = val;}

    //Getters
    float get_center_x() {return center_x;}
    float get_center_y() {return center_y;}
    float get_error() {return error;}

protected:
    float center_x;
    float center_y;
    float error;
};

#endif // SHAPE_H
