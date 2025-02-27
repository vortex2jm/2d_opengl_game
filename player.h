#ifndef player_h
#define player_h

#include <GL/glu.h>
#include <GL/gl.h>
#include <array>

#include "utils.h"
#include "shot.h"

// Size propotions
#define HEAD_PROP   0.30
#define ARMS_PROP   0.30
#define TRUNK_PROP  0.40
#define LEGS_PROP   0.30
#define ARM_ANGLE_BASE_RIGHT -90
#define ARM_ANGLE_BASE_LEFT 90

// Z-coord
#define PLAYER_Z_INDEX 1.0

// Legs movements adjustment
#define LEGS_FREQUENCY 0.5

// Jump physics
#define CORRECT_FACTOR 200000


class Player {
  // Private
  double cx = 0;  // centroid
  double cy = 0;
  double height;
  double velocity = 0.05;
  double jump_velocity = 0.075;

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
  double arms_angle = ARM_ANGLE_BASE_RIGHT;
  double arms_angle_base = ARM_ANGLE_BASE_RIGHT;

  //trunk===============
  double trunk_width = 0;
  double trunk_height = 0;

  //head==================
  double head_diameter = 0;  

  //jump control===================
  double fall_time = 0;
  double jump_time = 0.0;
  double jump_button_last_state = 0;
  JumpPhase jump_phase = JumpPhase::Up;

  // walk control
  HorizontalMoveDirection walk_direction = HorizontalMoveDirection::Right;
  HorizontalMoveDirection last_walk_direction = HorizontalMoveDirection::Right;

  // Methods======
  void draw_trunk(double z_index, std::array<double, 3> color) const;
  void draw_circle(double radius, double z_index, std::array<double, 3> color) const;
  void draw_head(double x, double y, double z_index, std::array<double, 3> color) const;
  void draw_arm(double x, double y, double theta, double z_index, std::array<double, 3> color) const;
  void draw_rect_by_base(double width, double height, double z_index, std::array<double, 3> color) const;
  void draw_rect_by_center(double width, double height, double z_index, std::array<double, 3> color) const;
  void draw_leg(double x, double y, double theta1, double theta2, double z_index, std::array<double, 3> color) const;

  //====
  public:
    Player(){}
    void setup(const svg_tools::Circ &circle);
    void draw() const;
    
    // walk control
    void walk(double time_diff, HorizontalMoveDirection direction);
    void reset_legs_position();

    // fall control
    int fall(double time_diff, double acc, bool collide);
    
    // jump control
    int jump(double time_diff, double acc, int button_state, bool collide);

    // getters
    double get_cx();
    double get_cy();
    double get_velocity();
    double get_initial_cx();
    JumpPhase get_jump_phase();
    double get_top_edge() const;
    double get_left_edge() const;
    double get_right_edge() const;
    double get_bottom_edge() const;
    HorizontalMoveDirection get_walk_direction();

    // setters
    void set_cx(double cx);
    void set_cy(double cy);
    void revert_walk_direction();
    void set_arm_angle(double angle);
    void set_velocity(double velocity);
    void set_arm_angle_base(double angle);
    
    // external items
    Shot * shoot();
};

#endif
