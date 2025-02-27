#include "arena.h"
#include <iostream>


/// @brief Initialize arena attributes
/// @param rectangles 
void Arena::setup(const std::vector<svg_tools::Rect> &rectangles) {

  // Copying vector
  Arena::obstacles = rectangles;
  int index_of_arena = 0;

  // Checking for arena properties
  for(const svg_tools::Rect& r: Arena::obstacles) { 
    if(r.color == "blue"){
      Arena::width = r.width;
      Arena::height = r.height;
      Arena::x = r.x;
      Arena::y = r.y;
      break;
    }
    index_of_arena++;
  }

  // Removing arena from obstacles
  Arena::obstacles.erase(Arena::obstacles.begin() + index_of_arena);
}


/// @brief Draws a rectangle growing downward and rightward
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

  // Anticlokwise
  glBegin(GL_POLYGON);
    glVertex3f(x, y, z_index);              // top left corner
    glVertex3f(x, y+height, z_index);       // bottom left corner
    glVertex3f(x+width, y+height, z_index); // bottom right corner
    glVertex3f( x+width, y, z_index);       // top right corner
  glEnd();        
}


/// @brief Draws a fixed arena including obstacles
void Arena::draw() const
{
  // Drawing arena
  Arena::draw_rect(Arena::x, Arena::y, Arena::width, Arena::height, 0.0, BLUE);

  // Drawing obstacles in front off arena (z-index = 1.0)
  for(const svg_tools::Rect& r: Arena::obstacles) {
    // r.color == "blue" ? (z_index =0.0, color = BLUE) : (z_index = 1.0, color = BLACK); 
    Arena::draw_rect(r.x, r.y, r.width, r.height, 1.0, BLACK);
  }
}


/// @brief Function to get GLOrtho X and Y limits based on the arena
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


// Getters===========
double Arena::get_x()
{
  return Arena::x;
}

double Arena::get_y()
{
  return Arena::y;
}

double Arena::get_width()
{
  return Arena::width;
}

double Arena::get_height()
{
  return Arena::height;
}

std::vector<svg_tools::Rect> Arena::get_obstacles()
{
  return Arena::obstacles;
}
