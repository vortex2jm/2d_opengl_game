#include "player.h"
#include <cmath>

void Player::setup(const svg_tools::Circ &circle)
{
  Player::cx = circle.cx;
  Player::cy = circle.cy;
  Player::height = circle.r * 2;

  Player::legs_height = circle.r * 2 * LEGS_PROP;
  Player::legs_width = legs_height * WIDTH_PROP_FACTOR;
  
  Player::trunk_height = circle.r * 2 * TRUNK_PROP;
  Player::trunk_width = trunk_height * WIDTH_PROP_FACTOR;

  Player::arms_height = circle.r * 2 * ARMS_PROP;
  Player::arms_width = arms_height * WIDTH_PROP_FACTOR;

  Player::head_diameter = circle.r * 2 * HEAD_PROP;
}


void Player::draw_circle(double radius, double z_index, const std::array<double, 3> color) const
{
  glColor3d(color[0], color[1], color[2]);
  glBegin(GL_POINTS);
    for(int i=0; i<360; i+=20){
      double rad = i * M_PI / 180.0;
      double x = radius * cos(rad);
      double y = radius * sin(rad);
      glVertex3d(x, y, z_index);
    }
  glEnd();
}


void Player::draw_rect_by_center(double width, double height, double z_index, std::array<double, 3> color) const
{
  glColor3d(color[0], color[1], color[2]);
  // Anticlokwise
  glBegin(GL_POLYGON);
    glVertex3f(-width/2, -height/2, z_index);   // top left corner
    glVertex3f(-width/2, height/2, z_index);    // bottom left corner
    glVertex3f(width/2, height/2, z_index);     // bottom right corner
    glVertex3f(width/2, -height/2, z_index);    // top right corner
  glEnd();  
}


void Player::draw_rect_by_base(double width, double height, double z_index, std::array<double, 3> color) const
{
  glColor3d(color[0], color[1], color[2]);
  // Anticlokwise
  glBegin(GL_POLYGON);
    glVertex3f(-width/2, 0, z_index);         // top left corner
    glVertex3f(-width/2, height, z_index);    // bottom left corner
    glVertex3f(width/2, height, z_index);     // bottom right corner
    glVertex3f(width/2, 0, z_index);          // top right corner
  glEnd(); 
}


void Player::draw_trunk(std::array<double, 3> color) const
{ 
  // Redundancy (helps legibility)
  // The trunk is the centroid of the player
  // All the other body parts will be drawn from this centroid using matrix transformations
  Player::draw_rect_by_center(Player::trunk_width, Player::trunk_height, PLAYER_Z_INDEX, color);
}


void Player::draw_head(double x, double y, double z_index, std::array<double, 3> color) const
{
  glPushMatrix();
    glTranslated(x, y, 0);
    Player::draw_circle(Player::head_diameter/2; PLAYER_Z_INDEX, color);
  glPopMatrix();
}


void Player::draw_arms(double x, double y, double theta, double z_index, std::array<double, 3> color) const
{
  glPushMatrix();
    glTranslated(x, y, 0);
    glRotated(theta, 0, 0 , 1);
    Player::draw_rect_by_base(Player::arms_width, Player::arms_height, PLAYER_Z_INDEX, color);
  glPopMatrix();
}


void Player::draw() const
{
  
}
