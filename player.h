#ifndef player_h
#define player_h

#include <GL/glu.h>
#include <GL/gl.h>
#include <array>

#include "utils.h"


// Size propotions
#define HEAD_PROP   0.30
#define ARMS_PROP   0.30
#define TRUNK_PROP  0.40
#define LEGS_PROP   0.30

// Z-coord
#define PLAYER_Z_INDEX 1.0

// Legs movements adjustment
#define LEGS_FREQUENCY 0.5


class Player {
  // Private
  double cx = 0;  // centroid
  double cy = 0;
  double height;
  double velocity = 0.05;

  // Initial position
  double initial_cx = 0;
  double initial_cy = 0;

  //legs=====================
  double legs_width = 0;
  double legs_height = 0;
  
  double hip_joint_angle1 = 0;  // -15
  double knee_joint_angle1 = 0;   // 0
  double hip_joint_angle2 = 0;   // 15
  double knee_joint_angle_2 = 0; // 30

  double x_variation_leg1 = 0;
  double x_variation_leg2 = 0;

  //arms===================
  double arms_width = 0;
  double arms_height = 0;

  //trunk===============
  double trunk_width = 0;
  double trunk_height = 0;

  //head==================
  double head_diameter = 0;  

  //jump control===================
  JumpPhase jump_phase = JumpPhase::Up;
  double jump_time = 0.0;
  double jump_button_last_state = 0;
  double fall_time = 0;

  // Methods======
  void draw_circle(double radius, double z_index, std::array<double, 3> color) const;
  void draw_rect_by_center(double width, double height, double z_index, std::array<double, 3> color) const;
  void draw_rect_by_base(double width, double height, double z_index, std::array<double, 3> color) const;
  void draw_trunk(double z_index, std::array<double, 3> color) const;
  void draw_head(double x, double y, double z_index, std::array<double, 3> color) const;
  void draw_arm(double x, double y, double theta, double z_index, std::array<double, 3> color) const;
  void draw_leg(double x, double y, double theta1, double theta2, double z_index, std::array<double, 3> color) const;
  
  // TODO (maybe) colision control is outside
  // void check_colision(double cx, std::vector<svg_tools::Rect> rectangles, std::string direction) const;

  public:
    Player(){}
    void setup(const svg_tools::Circ &circle);
    void draw() const;
    void walk(double time_diff);
    void reset_legs_position();
    int jump(double time_diff, int button_state, bool collide);
    int fall(double time_diff, bool collide);
    
    void set_fall_time(); // unusable
    void set_jump_phase_to_up();
    void set_jump_phase_to_down();

    double get_cx();
    double get_cy();
    double get_width();
    double get_height();
    JumpPhase get_jump_phase();
    double get_left_edge();
    double get_right_edge();
    double get_top_edge();
    double get_bottom_edge();
    double get_velocity();

};  

#endif
