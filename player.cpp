#include "player.h"
#include <cmath>
#include <iostream>


void Player::setup(const svg_tools::Circ &circle)
{
  Player::cx = circle.cx;
  Player::cy = circle.cy;
  Player::height = circle.r * 2;

  Player::initial_cx = cx;
  Player::initial_cy = cy;

  //legs========
  Player::legs_height = circle.r * 2 * LEGS_PROP;
  Player::legs_width = legs_height * 0.1;
  
  //trunk=======
  Player::trunk_height = circle.r * 2 * TRUNK_PROP;
  Player::trunk_width = trunk_height * 0.5;

  //arms========
  Player::arms_height = circle.r * 2 * ARMS_PROP;
  Player::arms_width = arms_height * 0.3;

  // head=======
  Player::head_diameter = circle.r * 2 * HEAD_PROP;
}


void Player::draw_circle(double radius, double z_index, const std::array<double, 3> color) const
{
  glColor3d(color[0], color[1], color[2]);
  glBegin(GL_POLYGON);
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


void Player::draw_trunk(double z_index, std::array<double, 3> color) const
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
    Player::draw_circle(Player::head_diameter/2, z_index, color);
  glPopMatrix();
}


void Player::draw_arm(double x, double y, double theta, double z_index, std::array<double, 3> color) const
{
  glPushMatrix();
    glTranslated(x, y, 0);
    glRotated(theta, 0, 0 , 1);
    Player::draw_rect_by_base(Player::arms_width, Player::arms_height, z_index, color);
  glPopMatrix();
}


void Player::draw_leg(double x, double y, double theta1, double theta2, double z_index, std::array<double, 3> color) const
{
  glPushMatrix();
    glTranslated(x, y, 0);
    glRotated(theta1, 0, 0, 1);   // Hip joint
    Player::draw_rect_by_base(Player::legs_width, Player::legs_height/2, z_index, color);
    glTranslated(0, Player::legs_height/2, 0);
    glRotated(theta2, 0, 0, 1);   //Knee joint
    Player::draw_rect_by_base(Player::legs_width, Player::legs_height/2, z_index, color);
  glPopMatrix();
}


void Player::draw() const
{
  glPushMatrix();
    glTranslated(Player::cx, Player::cy, 0);
    Player::draw_trunk(PLAYER_Z_INDEX ,GREEN);
    Player::draw_head(
      0, -((Player::trunk_height/2) + (Player::head_diameter/2)), PLAYER_Z_INDEX, GREEN
    );
    Player::draw_arm(0, 0, -90, PLAYER_Z_INDEX, YELLOW);
    Player::draw_leg(
      0, 
      Player::trunk_height/2, 
      Player::hip_joint_angle1,
      Player::knee_joint_angle1,
      PLAYER_Z_INDEX, 
      RED
    );   // Front leg
    Player::draw_leg(
      0, 
      Player::trunk_height/2, 
      Player::hip_joint_angle2, 
      Player::knee_joint_angle_2, 
      PLAYER_Z_INDEX, 
      RED
    );   // Back leg
  glPopMatrix();
}


//=================================
void Player::walk(double time_diff) 
{
  // Legs motion must be described by a periodic function
  // lower legs ->  y = 15 * (sin(kx) + 1)
  // upper legs  ->  y = 15 * sin(kx)
  // While one lag is moving frontward, the other is moving backward
  // The frequency has been set to simulates a natural movement
  
  // Translating player
  Player::cx += time_diff * Player::velocity;
  
  // The legs have opposite phases
  Player::x_variation_leg1 += time_diff * Player::velocity; 
  Player::x_variation_leg2 -= time_diff * Player::velocity;
  
  // Movement frequency adjustment
  double k = LEGS_FREQUENCY;

  //Calculating periodic functions based on each leg parameter================
  // Leg 1
  double upper_legs_motion_angle1 = 15 * sin(k*Player::x_variation_leg1);
  double lower_legs_motion_angle1 = 25 * (sin(k*Player::x_variation_leg1) + 1);
  // Leg 2
  double upper_legs_motion_angle2 = 15 * sin(k*Player::x_variation_leg2);
  double lower_legs_motion_angle2 = 25 * (sin(k*Player::x_variation_leg2) + 1);
  
  // Updating joints angles======================= 
  // Leg 1
  Player::hip_joint_angle1 = upper_legs_motion_angle1;
  Player::knee_joint_angle1 = lower_legs_motion_angle1;
  //Leg 2
  Player::hip_joint_angle2 = upper_legs_motion_angle2;
  Player::knee_joint_angle_2 = lower_legs_motion_angle2;
}


//================================
void Player::reset_legs_position()
{
  Player::hip_joint_angle1 = 0;
  Player::knee_joint_angle1 = 0;
  Player::hip_joint_angle2 = 0;
  Player::knee_joint_angle_2 =0;
}

//================================================================
int Player::jump(double time_diff, int button_state, bool collide)
{
  // Calculating rise and fall velocity
  // v = v0 + at
  double acc = 9.8;
  double correction_factor = 200000;  
  double rise_velocity = (Player::velocity + (-acc * jump_time / correction_factor));
  double fall_velocity = (0 + (acc * jump_time /correction_factor));

  if(button_state == 0){
    if(jump_button_last_state == 1 and jump_phase == JumpPhase::Up){
      jump_time = 0.0;  // Resetting jump time to start falling
    }
    jump_phase = JumpPhase::Down;
    jump_button_last_state = 0;
  }

  // Rising
  if(button_state == 1 and jump_phase == JumpPhase::Up) {
    if(rise_velocity <= 0 or collide){
      jump_phase = JumpPhase::Down;
      jump_time = 0.0;
      return 1;
    }

    // Rising up
    Player::cy -= time_diff * rise_velocity;
    jump_button_last_state = 1;
  }

  // Falling
  else if(Player::jump_phase == JumpPhase::Down) {
    if(Player::cy >= Player::initial_cy or collide) {
      jump_phase = JumpPhase::Up;
      jump_time = 0.0;
      return 0;
    }

    // Falling down
    Player::cy += time_diff * fall_velocity;
  }

  // Updating jump time
  Player::jump_time += time_diff;
  Player::reset_legs_position();
  return 1;
}


//==============================================
int Player::fall(double time_diff, bool collide)
{
  double acc = 9.8;
  double correction_factor = 200000;
  double fall_velocity = (0 + (acc * fall_time /correction_factor));

  if(Player::cy >= Player::initial_cy or collide) {
    // Restarting fall time
    fall_time = 0.0;
    return 0;
  }

  Player::cy += time_diff * fall_velocity; 
  Player::fall_time += time_diff;
  Player::reset_legs_position();
  return 1;  
}


// Getters=======================
JumpPhase Player::get_jump_phase()
{
  return Player::jump_phase;
}

double Player::get_left_edge()
{
  return Player::cx - (Player::trunk_width/2);
}

double Player::get_right_edge()
{
  return Player::cx + (Player::trunk_width/2);
}

double Player::get_top_edge()
{
  return Player::cy - (Player::height/2);
}

double Player::get_bottom_edge()
{
  return Player::cy + (Player::height/2);
}

double Player::get_velocity()
{
  return Player::velocity;
}
