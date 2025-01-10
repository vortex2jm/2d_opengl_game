#ifndef player_h
#define player_h

#include <GL/glu.h>
#include <GL/gl.h>
#include <array>

#include "utils.h"

// Size propotions
#define HEAD_PROP   0.15
#define ARMS_PROP   0.30
#define TRUNK_PROP  0.40
#define LEGS_PROP   0.45
#define WIDTH_PROP_FACTOR 0.30 

#define PLAYER_Z_INDEX 1


class Player {
  // Private
  double cx = 0;  // centroid
  double cy = 0;
  double height;

  double legs_width = 0;
  double legs_height = 0;

  double arms_width = 0;
  double arms_height = 0;

  double trunk_width = 0;
  double trunk_height = 0;

  double head_diameter = 0;  

  void draw_circle(double radius, double z_index, std::array<double, 3> color) const;
  void draw_rect_by_center(double width, double height, double z_index, std::array<double, 3> color) const;
  void draw_rect_by_base(double width, double height, double z_index, std::array<double, 3> color) const;
  void draw_trunk(std::array<double, 3> color) const;
  void draw_head(double x, double y, double z_index, std::array<double, 3> color) const;
  void draw_arms(double x, double y, double theta, double z_index, std::array<double, 3> color) const;
  void draw_legs() const;
  void draw_body() const;

  public:
    Player(){}
    void setup(const svg_tools::Circ &circle);
    void draw() const;
};  

#endif
