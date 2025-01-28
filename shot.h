#ifndef shot_h
#define shot_h

#include <GL/glu.h>
#include <GL/gl.h>

class Shot {
    double x; 
    double y; 
    double x_initial; 
    double y_initial; 
    // double gDirectionAng;
    double radius = 1;
    double velocity = 0.1;
    double direction_vector[2];

private:
    void draw_circle(double radius, double R, double G, double B);

public:
    Shot(double init_point[2], double direct_vec[2]);
    void draw();
    void move(double timeDiff);
    bool is_valid();
    
    // getters
    void get_pos(double &x_out, double &y_out);
};

#endif
