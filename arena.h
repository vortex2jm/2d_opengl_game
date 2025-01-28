#ifndef arena_h
#define arena_h

#include <GL/glu.h>
#include <GL/gl.h>
#include <string>
#include <vector>
#include "utils.h"
#include <array>
#include <map>

/// @brief Class to create the game environment
class Arena {
  
  // Private by default
  double x = 0;
  double y = 0;
  double width = 0;
  double height = 0;
  std::vector<svg_tools::Rect> obstacles = {};

  void draw_rect(
    const double x,
    const double y, 
    const double width, 
    const double height, 
    const double z_index, 
    const std::array<double, 3> color) const;

  public:
    Arena(){}
    void draw() const;
    void setup(const std::vector<svg_tools::Rect> &rectangles);
    
    // getters
    double get_x();
    double get_y();
    double get_width();
    double get_height();
    std::vector<svg_tools::Rect> get_obstacles();
    std::map<std::string, double> get_2dprojection_limits() const;
};

#endif
