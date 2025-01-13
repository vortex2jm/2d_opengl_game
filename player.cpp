#include "player.h"
#include <cmath>

void Player::setup(const svg_tools::Circ &circle)
{
  Player::cx = circle.cx;
  Player::cy = circle.cy;
  Player::height = circle.r * 2;

  Player::legs_height = circle.r * 2 * LEGS_PROP;
  Player::legs_width = legs_height * 0.1;
  
  Player::trunk_height = circle.r * 2 * TRUNK_PROP;
  Player::trunk_width = trunk_height * 0.5;

  Player::arms_height = circle.r * 2 * ARMS_PROP;
  Player::arms_width = arms_height * 0.3;

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

void Player::horizontal_move(double x)
{
  // Legs motion must be described by a periodic function
  // lower legs ->  y = 15 * (sin(kx) + 1)
  // upper legs  ->  y = 15 * sin(kx)
  // While one lag is moving frontward, the other is moving backward
  // The frequency has been set to simulates a natural movement
  
  // Translating player
  Player::cx += x;
  
  // The legs have opposite phases
  Player::x_variation_leg1 += x;
  Player::x_variation_leg2 -= x;
  
  // Movement frequency adjustment
  double k = 0.5;

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


void Player::reset_legs_position()
{
  Player::hip_joint_angle1 = 0;
  Player::knee_joint_angle1 = 0;
  Player::hip_joint_angle2 = 0;
  Player::knee_joint_angle_2 =0;
}

