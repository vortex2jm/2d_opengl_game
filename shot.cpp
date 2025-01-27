#include "shot.h"
#include <math.h>
#include <iostream>

#define DISTANCIA_MAX 500

Shot::Shot(double init_point[2], double direct_vec[2])
{
  Shot::x_initial = init_point[0]; 
  Shot::y_initial = init_point[1]; 
  Shot::x = init_point[0]; 
  Shot::y = init_point[1];
  // gDirectionAng = directionAng; 
  Shot::direction_vector[0] = direct_vec[0];
  Shot::direction_vector[1] = direct_vec[1];
}


void Shot::draw_circle(double radius, double R, double G, double B)
{
  glColor3f(R, G, B);
  glBegin(GL_POLYGON);
    for(float i=0; i<360; i+=0.1){
      double rad = i * M_PI / 180.0;
      double x = radius * cos(rad);
      double y = radius * sin(rad);
      glVertex3d(x, y, 3); 
    }
  glEnd();
}


void Shot::draw()
{
  glPushMatrix();
    glTranslatef(Shot::x, Shot::y, 0);
    Shot::draw_circle(Shot::radius, 1, 1, 1);  
  glPopMatrix();
}


void Shot::move(double timeDiff)
{
  double aux_vec[2] = { 
    Shot::direction_vector[0] * Shot::velocity * timeDiff,   // Distance = velocity * time
    Shot::direction_vector[1] * Shot::velocity * timeDiff 
  };

  x += aux_vec[0];
  y += aux_vec[1];
}


bool Shot::is_valid()
{
  return !(Shot::x > DISTANCIA_MAX or Shot::y > DISTANCIA_MAX);
}


void Shot::get_pos(double &x_out, double &y_out)
{
  x_out = Shot::x;
  y_out = Shot::y;
}
