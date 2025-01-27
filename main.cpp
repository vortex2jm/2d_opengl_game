#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <math.h>

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <random>

#include "tinyxml2.h"
#include "player.h"
#include "utils.h"
#include "arena.h"
#include "shot.h"

#define MOUSE_LEFT 254
#define MOUSE_RIGHT 255
#define GRAVITY 28
#define SHOT_INTERVAL 500 // ms

// Window dimensions
const int Width = 500;
const int Height = 500;

// keyboard
int key_status[256] = {0};

// Jump controls
JumpState jump_state = JumpState::NotJumping;
FallState fall_state = FallState::NotFalling;

// enemy controls
double shot_timer = 0;

// Callback declarations
void init(void);
void idle(void);
void resetKeyStatus();
void renderScene(void);
void keyUp(unsigned char key, int x, int y);
void keyPress(unsigned char key, int x, int y);
void mouseClick(int button, int state, int x, int y);
void mouseMotion(int x, int y);

// utilities
void set_camera(double time, double velocity, HorizontalMoveDirection direction);
double get_time_diff();

// game_tools
bool is_player_into_arena_horizontally(Player player, Arena arena, HorizontalMoveDirection direction);
bool walking_collision(Player &player, Arena arena, std::list<Player> enemies, HorizontalMoveDirection direction, double timeDiff);
bool jumping_collision(Player &player, Arena arena, std::list<Player> enemies, double timeDiff);
bool falling_collision(Player &player, Arena arena, std::list<Player> enemies, double timeDiff);

//svg data===================================
std::vector<svg_tools::Rect> rectangles = {};
std::vector<svg_tools::Circ> circles = {};

// game components
Arena ring;
Player self;
std::list<Shot*> shots;
std::list<Player> enemies;


//=============================//
// MAIN                        //
//=============================//
int main(int argc, char *argv[])
{
  // CLI validation
  if(argc < 2){
    std::cerr << "Missing .svg file!" << std::endl;
    exit(1);
  }

  // Reading .svg and setting up ring==============
  svg_tools::readSvg(argv[1], rectangles, circles);  //vectors passed by referece   
  ring.setup(rectangles);
  
  // Setting up players===================
  for(const svg_tools::Circ &c: circles){
    if(c.color == "green"){
      self.setup(c);
      continue;
    }
    Player p;
    p.setup(c);
    enemies.push_back(p); // copying instance into global vector
  }

  // Setting up GLUT===================
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // Creating the window.
  glutInitWindowSize(Width, Height);
  glutInitWindowPosition(150,50);
  glutCreateWindow("2D Shot Game");

  // Defining callbacks.
  glutIdleFunc(idle);
  glutKeyboardUpFunc(keyUp);
  glutKeyboardFunc(keyPress);
  glutMouseFunc(mouseClick);
  glutPassiveMotionFunc(mouseMotion);
  glutDisplayFunc(renderScene);

  // Setup
  init();

  glutMainLoop();
  return 0;
}

//=============
void init(void)
{
  // Erasing frames and keys
  resetKeyStatus();
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black, no opacity(alpha).
  
  // Setting up projection
  std::map<std::string, double> limits = ring.get_2dprojection_limits();

  glMatrixMode(GL_PROJECTION);             
  
  // projection limits must to be proportional with the window aspect ratio
  glOrtho(
    self.get_cx() - (ring.get_height()/2), //left edge
    // limits["left"],     // left edge
    // limits["right"],    // right edge
    self.get_cx() + (ring.get_height()/2), //right edge
    limits["bottom"],   // bottom edge
    limits["top"],      // top edge
    -100,               // “near” plane
    100                 // “far” plane
  );               
  
  // Translates the world to left in front of camera (player movement)
  // glTranslatef(-100, 0, 0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}


//=============================//
// Callbacks implementations   //
//=============================//
void renderScene(void)
{
  // Erasing buffer
  glClear(GL_COLOR_BUFFER_BIT);
  
  // Drawing elements
  ring.draw();
  self.draw();
  for(const Player &p: enemies){
    p.draw();
  }
  for(Shot * shot: shots) {
    shot->draw();
  }

  // Processing new frame
  glutSwapBuffers(); 
}


//========================================
void keyUp(unsigned char key, int x, int y){
  key_status[key] = 0;
  
  // reseting legs to initial position
  if(key == 'a' or key == 'd') {
    self.reset_legs_position();
  }

  glutPostRedisplay();
}


//==================
void resetKeyStatus(){
  for(int x=0; x<256; x++){
    key_status[x] = 0;
  }
  glutPostRedisplay();  
}


//============================================
void keyPress(unsigned char key, int x, int y){
  switch (key)
  {
  case 'a':
  case 'A':
    key_status['a'] = 1;
    break;

  case 'd':
  case 'D':
    key_status['d'] = 1;
    break;

  case 0x1b:  // ESC
    exit(0);

  default:
    break;
  }

  glutPostRedisplay();
}


//=============
void idle(void){
  // Don't delete this part=================================
  // static double previousTime = glutGet(GLUT_ELAPSED_TIME);
  // double currentTime, timeDifference;
  // currentTime = glutGet(GLUT_ELAPSED_TIME);
  // timeDifference = currentTime - previousTime;
  // previousTime = currentTime;
  //========================================================

  // Setting time passed between iteration
  double timeDifference = get_time_diff();

  // Horizontal left motion===========
  if(key_status['a']) {
    // Checking arena limits
    if(is_player_into_arena_horizontally(self, ring, HorizontalMoveDirection::Left)) {
      // Checking collision against obstacles
      if(!walking_collision(self, ring, enemies,HorizontalMoveDirection::Left, timeDifference)) {
        // Walking
        self.walk(timeDifference, HorizontalMoveDirection::Left);
        set_camera(timeDifference, self.get_velocity(), HorizontalMoveDirection::Left);
      }
    }
  }

  // Horizontal right motion=========
  if(key_status['d']) {
    // Checking arena limits
    if(is_player_into_arena_horizontally(self, ring, HorizontalMoveDirection::Right)) {
      // Checking collision against obstacles
      if(!walking_collision(self, ring, enemies,HorizontalMoveDirection::Right, timeDifference)) {
        // Walking
        self.walk(timeDifference, HorizontalMoveDirection::Right);
        set_camera(timeDifference, self.get_velocity(), HorizontalMoveDirection::Right);
      }
    }
  }


  //Gravity physics=========================
  if(jump_state == JumpState::NotJumping) {
    if(self.fall(
        timeDifference, 
        GRAVITY,
        falling_collision(self, ring, enemies, timeDifference)  
      )
    ) {
      fall_state = FallState::Falling;
    } else {
      fall_state = FallState::NotFalling;
    }
  }


  // Jump==============================
  if(jump_state == JumpState::Jumping){
    // If jump() returns 0, jump finished
    if(!self.jump(
        timeDifference, 
        GRAVITY,
        key_status[MOUSE_RIGHT],
        jumping_collision(self, ring, enemies, timeDifference))
      ) {
      jump_state = JumpState::NotJumping;
    }
  }


  // Treating shots=====================================
  for (auto shot = shots.begin(); shot != shots.end();) {
    bool is_shot_deleted = false;

    // Motion update
    if(*shot) {
        (*shot)->move(timeDifference); 
    }

    // Getting position
    double shot_x, shot_y;
    (*shot)->get_pos(shot_x, shot_y);
    
    // Checking collision against enemies
    for(auto enemy = enemies.begin(); enemy != enemies.end();) {  
      if(
        shot_x > (*enemy).get_left_edge() && 
        shot_x < (*enemy).get_right_edge() &&
        shot_y > (*enemy).get_top_edge() &&
        shot_y < (*enemy).get_bottom_edge()  
      ){
        enemy = enemies.erase(enemy);
        delete (*shot);
        shot = shots.erase(shot);
        is_shot_deleted = true;
        break;
      } 
      else {
        ++ enemy;
      }
    }

    // Prevent seg fault
    if(is_shot_deleted) continue;

    // Checking collision against obstacles
    for(const svg_tools::Rect& r: ring.get_obstacles()){
      if(
        shot_x > r.x && 
        shot_x < (r.x + r.width) &&
        shot_y > r.y &&
        shot_y < (r.y + r.height)
      ){
        delete (*shot);
        shot = shots.erase(shot);
        is_shot_deleted = true;
        break;
      }
    }

    // Prevent seg fault
    if(is_shot_deleted) continue;


    // Shot lifecycle
    if (not (*shot)->is_valid()) {
        delete (*shot);
        shot = shots.erase(shot);
    } 
    else {
     ++shot;
    }
  }


  // Enemies motion=====
  for(Player &enemy: enemies){

    // enemies  always aim to self player
    double self_distance_x = self.get_cx() - enemy.get_cx();
    double self_distance_y = self.get_cy() - enemy.get_cy();
    double rad = atan2(self_distance_y, abs(self_distance_x));
    double deg = rad * 180.0/M_PI;
    enemy.set_arm_angle(deg);

    // if(shot_timer >= 1000){
    //   // std::cout << "shoot" << std::endl;
    //   shots.push_back(enemy.shoot());
    //   shot_timer = 0.0;
    // }

  }


  // Choosing random enemy to shot
  if(!enemies.empty() and shot_timer >= SHOT_INTERVAL){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, enemies.size() - 1);
    int random_index = distrib(gen);

    auto enemy = enemies.begin(); // Inicializa o iterador
    std::advance(enemy, random_index); 

    if(enemy != enemies.end()){
      shots.push_back((*enemy).shoot());
    }

    shot_timer = 0.0;
  }

  // Updating timer
  shot_timer += timeDifference;
  glutPostRedisplay();
}


//===================================================
void mouseClick(int button, int state, int x, int y) {
  if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {

    // The jump key can be activated only when the player is not jumping
    if(jump_state == JumpState::NotJumping and fall_state == FallState::NotFalling){
      jump_state = JumpState::Jumping;
      key_status[MOUSE_RIGHT] = 1;
    }
    return;
  }

  if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
    key_status[MOUSE_RIGHT] = 0;  // it's necessary to compute when mouse button is pressed to keep the jump up
    return;
  }

  if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    shots.push_back(self.shoot());
    return;
  }
}

//============================
void mouseMotion(int x, int y)
{
  // Mapping mouse position into virtual world
  // Getting displacement proportion within window and
  // transposing this proportion to virtual world
  double mapped_mouse_pos_y = (-ring.get_height() * ((double)y/(double)Height)) - ring.get_y(); 
  double mapped_mouse_pos_x = (ring.get_height() * ((double)x/(double)Width)) + ring.get_x();
  
  double mapped_mouse_displacement_y = mapped_mouse_pos_y - (-self.get_cy());
  double mapped_mouse_displacement_x = 
    mapped_mouse_pos_x - 
    self.get_cx() + 
    (self.get_cx() - self.get_initial_cx()); // offset because arena is too large

  // Calculating arms angle based on mouse angle with player                                                                          
  double rad = atan2(mapped_mouse_displacement_y, abs(mapped_mouse_displacement_x)); // abs(x) for 1 and 4 quadrants
  double deg = rad * 180.0/M_PI;

  self.set_arm_angle(-deg);
  
  glutPostRedisplay();
}


//===============================================================================
void set_camera(double time, double velocity, HorizontalMoveDirection direction)
{
  double displacement = time * velocity; 

  if(direction == HorizontalMoveDirection::Right){
    displacement *= -1;
  }

  glMatrixMode(GL_PROJECTION);             
    glTranslated(displacement, 0, 0);
  glMatrixMode(GL_MODELVIEW);
}


//===================
double get_time_diff()
{
  static double previousTime = glutGet(GLUT_ELAPSED_TIME);
  double currentTime, timeDifference;
  currentTime = glutGet(GLUT_ELAPSED_TIME);
  timeDifference = currentTime - previousTime;
  previousTime = currentTime;
  return timeDifference;
}


//=============================================================================
// TODO 
bool is_player_into_arena_horizontally(Player player, Arena arena, HorizontalMoveDirection direction)
{
  if(direction == HorizontalMoveDirection::Left) {
    return (player.get_left_edge() >= arena.get_x()); 
  }
  // Right
  return (player.get_right_edge() <= (arena.get_x() + arena.get_width()));
}


//=======================================================================================================
// TODO - Create arena hitbox functions
//=======================================
bool walking_collision(Player &player, Arena arena, std::list<Player> enemies, HorizontalMoveDirection direction, double timeDiff) {
  
  double vertical_offset = timeDiff * player.get_velocity() + 0.1;
  
  // Right motion
  if(direction == HorizontalMoveDirection::Right) {

    // Obstacles collision================================
    for(const svg_tools::Rect& r: arena.get_obstacles()) {
      if(
        // by width 
        (player.get_right_edge() >= (r.x)) &&  
        (player.get_right_edge() <= (r.x + r.width)) &&
        // by height
        ( 
          (((r.y + r.height - vertical_offset) >= player.get_top_edge()) && 
           ((r.y) <= player.get_top_edge())
          ) || 

          ( ((r.y + vertical_offset) <= player.get_bottom_edge()) &&
            ((r.y + r.height) >= player.get_bottom_edge())
          ) ||
  
          (((r.y) >= player.get_top_edge()) && 
           (r.y + r.height) <= player.get_bottom_edge()
          )
        )
      ){

        // double new_cx = player.get_cx() - (player.get_right_edge() - r.x);
        // player.set_cx(new_cx);

        return true;
      }
    }

    // Enemy collision====================
    for(const Player &enemy: enemies) {
      if(
        // by width 
        (player.get_right_edge() >= enemy.get_left_edge()) &&  
        (player.get_right_edge() <= enemy.get_right_edge()) &&
        // by height
        ( 
          (((enemy.get_bottom_edge() - vertical_offset) >= player.get_top_edge()) && 
           (enemy.get_top_edge() <= player.get_top_edge())
          ) || 

          ( ((enemy.get_top_edge() + vertical_offset) <= player.get_bottom_edge()) &&
            (enemy.get_bottom_edge() >= player.get_bottom_edge())
          ) ||

          ((enemy.get_top_edge() >= player.get_top_edge()) && 
           enemy.get_bottom_edge() <= player.get_bottom_edge()
          )
        )
      ){

        double new_cx = player.get_cx() - (player.get_right_edge() - enemy.get_left_edge());
        player.set_cx(new_cx);

        return true;
      }
    }

    return false;
  }

  // Left
  for(const svg_tools::Rect& r: arena.get_obstacles()) {
    if( 
      (player.get_left_edge() <= (r.x + r.width)) &&  
      (player.get_left_edge() >= (r.x)) &&
      (
        (((r.y + r.height - vertical_offset) >= player.get_top_edge()) && 
         ((r.y) <= player.get_top_edge())
        ) ||
        
        ( ((r.y + vertical_offset) <= player.get_bottom_edge()) &&
          ((r.y + r.height) >= player.get_bottom_edge())
        ) ||
        (((r.y) >= player.get_top_edge()) &&
         (r.y + r.height) <= player.get_bottom_edge()
        )
      )
    ){

      // double new_cx = player.get_cx() + ((r.x + r.width) - player.get_left_edge());
      // player.set_cx(new_cx);
      
      return true;
    }
  }

  for(const Player &enemy: enemies) {
    if( 
      (player.get_left_edge() <= enemy.get_right_edge()) &&  
      (player.get_left_edge() >= enemy.get_left_edge()) &&
      (
        (((enemy.get_bottom_edge() - vertical_offset) >= player.get_top_edge()) && 
         (enemy.get_top_edge() <= player.get_top_edge())
        ) ||
        
        ( ((enemy.get_top_edge() + vertical_offset) <= player.get_bottom_edge()) &&
          (enemy.get_bottom_edge() >= player.get_bottom_edge())
        ) ||
        ((enemy.get_top_edge() >= player.get_top_edge()) &&
         enemy.get_bottom_edge() <= player.get_bottom_edge()
        )
      )
    ){

      double new_cx = player.get_cx() + (enemy.get_right_edge() - player.get_left_edge());
      player.set_cx(new_cx);

      return true;
    }
  }

  return false;
}


//=============================================================================
bool jumping_collision(Player &player, Arena arena, std::list<Player> enemies, double timeDiff)
{ 
  // This factor avoid player halting horizontally against the obstacles when it's jumping.
  //
  // This is a hitbox adjustment.
  //
  // The horizontal motion is controlled by another function and the player stops when
  // it's against the obstacle limit. This factor does a little adjustment in this limit 
  // for the vertical motion function, once the vertical motion function also leads
  // with horizontal limits and stops the jump based on horizontal limits too.
  //
  // Without this adjustment, the player stucks in the wall when it's jumping


  // TODO - this offset is not working because the displacement grows with time
  double horizontal_offset = timeDiff * player.get_velocity() + 0.1;


  if(player.get_jump_phase() == JumpPhase::Up) {
    
    // Obstacles collision==============================
    for(const svg_tools::Rect& r: arena.get_obstacles()) {
      if(
        (((player.get_top_edge() <= (r.y + r.height)) && (player.get_top_edge() >= r.y))  && 
        (
          ((player.get_right_edge() >= r.x + horizontal_offset) && (player.get_left_edge() <= r.x)) ||
          ((player.get_left_edge() <= (r.x + r.width - horizontal_offset)) && (player.get_right_edge() >= (r.x + r.width))) ||
          ((player.get_left_edge() >= r.x) && (player.get_right_edge() <= (r.x + r.width)))
        )) ||
        (player.get_top_edge() <= arena.get_y())
      ){
        return true;
      } 
    }

    // Enemy coliision===============
    for(const Player &enemy: enemies) {
      if(
        (((player.get_top_edge() <= enemy.get_bottom_edge()) && (player.get_top_edge() >= enemy.get_top_edge()))  && 
        (
          ((player.get_right_edge() >= enemy.get_left_edge() + horizontal_offset) && (player.get_left_edge() <= enemy.get_left_edge())) ||
          ((player.get_left_edge() <= (enemy.get_right_edge() - horizontal_offset)) && (player.get_right_edge() >= enemy.get_right_edge())) ||
          ((player.get_left_edge() >= enemy.get_left_edge()) && (player.get_right_edge() <= enemy.get_right_edge()))
        ))
      ){
        return true;
      } 
    }

    return false;
  }

  //Down
  for(const svg_tools::Rect& r: arena.get_obstacles()) {
    if(
      ((player.get_bottom_edge() >= (r.y)) && (player.get_bottom_edge() <= (r.y + r.height))) && 
      (
        ((player.get_right_edge() >= r.x + horizontal_offset) && (player.get_left_edge() <= r.x)) ||
        ((player.get_left_edge() <= (r.x + r.width - horizontal_offset)) && (player.get_right_edge() >= (r.x + r.width))) ||
        ((player.get_left_edge() >= r.x) && (player.get_right_edge() <= (r.x + r.width)))
      )
    ){
      
      // Respecting hitboxes
      double new_cy = player.get_cy() - (player.get_bottom_edge() - r.y);
      player.set_cy(new_cy);

      return true;
    } 
  }

   for(const Player &enemy: enemies) {
    if(
      ((player.get_bottom_edge() >= enemy.get_top_edge()) && (player.get_bottom_edge() <= enemy.get_bottom_edge())) && 
      (
        ((player.get_right_edge() >= enemy.get_left_edge() + horizontal_offset) && (player.get_left_edge() <= enemy.get_left_edge())) ||
        ((player.get_left_edge() <= (enemy.get_right_edge() - horizontal_offset)) && (player.get_right_edge() >= enemy.get_right_edge())) ||
        ((player.get_left_edge() >= enemy.get_left_edge()) && (player.get_right_edge() <= enemy.get_right_edge()))
      )
    ){

      double new_cy = player.get_cy() - (player.get_bottom_edge() - enemy.get_top_edge());
      player.set_cy(new_cy);

      return true;
    } 
   }

  return false;
}


//=================================================================
bool falling_collision(Player &player, Arena arena, std::list<Player> enemies, double timeDiff)
{
  double horizontal_offset = timeDiff * player.get_velocity() + 0.1;

  // obstacles collision
  for(const svg_tools::Rect& r: arena.get_obstacles()) {
    if(
      ((player.get_bottom_edge() >= (r.y)) && (player.get_bottom_edge() <= (r.y + r.height))) && 
      (
        ((player.get_right_edge() >= r.x + horizontal_offset) && (player.get_left_edge() <= r.x)) ||
        ((player.get_left_edge() <= (r.x + r.width - horizontal_offset)) && (player.get_right_edge() >= (r.x + r.width))) ||
        ((player.get_left_edge() >= r.x) && (player.get_right_edge() <= (r.x + r.width)))
      )
    ){

      // Respecting hitboxes
      double new_cy = player.get_cy() - (player.get_bottom_edge() - r.y);
      player.set_cy(new_cy);
      
      return true;
    } 
  }

  // Enemy collision
  for(const Player &enemy: enemies) {
    if(
    ((player.get_bottom_edge() >= enemy.get_top_edge()) && (player.get_bottom_edge() <= enemy.get_bottom_edge())) && 
    (
      ((player.get_right_edge() >= enemy.get_left_edge() + horizontal_offset) && (player.get_left_edge() <= enemy.get_left_edge())) ||
      ((player.get_left_edge() <= (enemy.get_right_edge() - horizontal_offset)) && (player.get_right_edge() >= enemy.get_right_edge())) ||
      ((player.get_left_edge() >= enemy.get_left_edge()) && (player.get_right_edge() <= enemy.get_right_edge()))
    )
    ){

      double new_cy = player.get_cy() - (player.get_bottom_edge() - enemy.get_top_edge());
      player.set_cy(new_cy);
      
      return true;
    } 
  }  

  return false;
}
