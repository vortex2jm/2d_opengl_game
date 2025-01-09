#include "arena.h"
#include <iostream>

/// @brief Initialize arena attributes
/// @param rectangles 
void Arena::setup(const std::vector<svg_tools::Rect> &rectangles) {
  for(const svg_tools::Rect& r: rectangles) { 
    if(r.color == "blue"){
      Arena::width = r.width;
      Arena::height = r.height;
      Arena::x = r.x;
      Arena::y = r.y;
      Arena::obstacles = rectangles;  // Copying vector
      break;
    }
  }
}


/// @brief Draw a rectangle with param data
/// @param x 
/// @param y 
/// @param width 
/// @param height 
/// @param z_index 
/// @param color 
void Arena::draw_rect(
  const double x,
  const double y, 
  const double width, 
  const double height, 
  const double z_index, 
  const std::array<double, 3> color) const
{
  glColor3d(color[0], color[1], color[2]);
  glBegin(GL_POLYGON);
    glVertex3f(x+width, y+height, z_index); // top right corner
    glVertex3f(x, y+height, z_index);       // top left corner
    glVertex3f(x, y, z_index);              // bottom left corner
    glVertex3f( x+width, y, z_index);       // bottom right corner
  glEnd();        
}


/// @brief Draws the arena including obstacles
void Arena::draw() const
{
  double z_index = 0;
  std::array<double, 3> color = {};

  for(const svg_tools::Rect& r: Arena::obstacles) {
    r.color == "blue" ? (z_index =0.0, color = BLUE) : (z_index = 1.0, color = BLACK); 
    Arena::draw_rect(r.x, r.y, r.width, r.height, z_index, color);
  }
}


/// @brief Function to get GLOrtho X and Y limits based on arena
/// @return a map of edge limits (left, right, top and bottom)
std::map<std::string, double> Arena::get_2dprojection_limits() const
{
  std::map<std::string, double> limits;
  limits["left"] = Arena::x;
  limits["right"] = Arena::x + Arena::height;
  limits["top"] = Arena::y;
  limits["bottom"] = Arena::y + Arena::height;  // Y grows downward

  return limits;  // Returns by copy
}
