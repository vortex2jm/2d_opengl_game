#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include <iostream>
#include <string>
#include <vector>

#include "tinyxml2.h"
#include "player.h"
#include "utils.h"
#include "arena.h"

#define MOUSE_RIGHT 255

// Window dimensions
const GLint Width = 700;
const GLint Height = 700;

// keyboard
int key_status[256] = {0};

// Jump controls
JumpState jump_state = JumpState::NotJumping;
FallState fall_state = FallState::NotFalling;

// Callback declarations
void init(void);
void idle(void);
void resetKeyStatus();
void renderScene(void);
void keyUp(unsigned char key, int x, int y);
void keyPress(unsigned char key, int x, int y);
void mouseClick(int button, int state, int x, int y);

// game_tools
bool is_player_into_arena_horizontally(Player player, Arena arena, HorizontalMoveDirection direction);
bool walking_collision(Player player, Arena arena, HorizontalMoveDirection direction, double timeDiff);
bool jumping_collision(Player player, Arena arena, double timeDiff);
bool falling_collision(Player player, Arena arena, double timeDiff);

//svg data===================================
std::vector<svg_tools::Rect> rectangles = {};
std::vector<svg_tools::Circ> circles = {};

// game components
Arena ring;
Player self;
std::vector<Player> enemies = {};


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
  glutMouseFunc(mouseClick);
  glutKeyboardFunc(keyPress);
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
    limits["left"],     // left edge
    limits["right"],    // right edge
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
  // Setting frame rate
  static double previousTime = glutGet(GLUT_ELAPSED_TIME);
  double currentTime, timeDifference;
  currentTime = glutGet(GLUT_ELAPSED_TIME);
  timeDifference = currentTime - previousTime;
  previousTime = currentTime;

  // Horizontal left motion
  if(key_status['a']) {
    // Checking arena limits
    if(is_player_into_arena_horizontally(self, ring, HorizontalMoveDirection::Left)) {
      // Checking collision against obstacles
      if(!walking_collision(self, ring, HorizontalMoveDirection::Left, timeDifference)) {
        // Walking
        self.walk(-timeDifference);
        // Updating camera position
        glMatrixMode(GL_PROJECTION);             
          glTranslated((timeDifference* self.get_velocity()), 0, 0);
        glMatrixMode(GL_MODELVIEW);
      }
    }
  }

  // Horizontal right motion
  if(key_status['d']) {
    // Checking arena limits
    if(is_player_into_arena_horizontally(self, ring, HorizontalMoveDirection::Right)) {
      // Checking collision against obstacles
      if(!walking_collision(self, ring, HorizontalMoveDirection::Right, timeDifference)) {
        
        // Walking
        self.walk(timeDifference);
        
        glMatrixMode(GL_PROJECTION);             
          glTranslated(-(timeDifference*self.get_velocity()), 0, 0);
        glMatrixMode(GL_MODELVIEW);

      }
    }
  }


  //Gravity physics
  if(jump_state == JumpState::NotJumping) {
    if(self.fall(
        timeDifference, 
        falling_collision(self, ring, timeDifference)  
      )
    ) {
      fall_state = FallState::Falling;
    } else {
      fall_state = FallState::NotFalling;
    }
  }


  // Jump
  if(jump_state == JumpState::Jumping){
    // If jump() returns 0, jump finished
    if(!self.jump(
        timeDifference, 
        key_status[MOUSE_RIGHT],
        jumping_collision(self, ring, timeDifference)) // Checking collision
      ) {
      jump_state = JumpState::NotJumping;
    }
  }

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
  }
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
bool walking_collision(Player player, Arena arena,  HorizontalMoveDirection direction, double timeDiff) {
  
  double vertical_offset = timeDiff * player.get_velocity() + 0.1;
  
  if(direction == HorizontalMoveDirection::Right) {
    for(const svg_tools::Rect& r: arena.get_obstacles()) {
      if(
        // by width 
        (player.get_right_edge() >= (r.x)) &&  
        (player.get_right_edge() <= (r.x + r.width)) &&
        // by height
        ( 
          // First case=======
          //     ___
          //  ---|_|
          //  |_|
          //=================== 
          (((r.y + r.height - vertical_offset) >= player.get_top_edge()) && 
           ((r.y) <= player.get_top_edge())
          ) || 
          // Second case=======
          //  ___
          //  | |___
          //  ---|_|
          //===================
          ( ((r.y + vertical_offset) <= player.get_bottom_edge()) &&
            ((r.y + r.height) >= player.get_bottom_edge())
          ) ||
          
          // with the first and second case, a third case is treated:
          //     ___  
          //  ___| |
          //  |_|  |
          //     |_|
          //==================
          // Fourth case
          //     ___  
          //     | |____
          //     |  |__|
          //     |_|
          //==================  
          (((r.y) >= player.get_top_edge()) && 
           (r.y + r.height) <= player.get_bottom_edge()
          )
        )
      ){
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
      return true;
    }
  }
  return false;
}


//=============================================================================
bool jumping_collision(Player player, Arena arena, double timeDiff)
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
  double horizontal_offset = timeDiff * player.get_velocity() + 0.1;

  if(player.get_jump_phase() == JumpPhase::Up) {
    for(const svg_tools::Rect& r: arena.get_obstacles()) {
      if(
        ((player.get_top_edge() <= (r.y + r.height)) && (player.get_top_edge() >= r.y))  && 
        (
          ((player.get_right_edge() >= r.x + horizontal_offset) && (player.get_left_edge() <= r.x)) ||
          ((player.get_left_edge() <= (r.x + r.width - horizontal_offset)) && (player.get_right_edge() >= (r.x + r.width))) ||
          ((player.get_left_edge() >= r.x) && (player.get_right_edge() <= (r.x + r.width)))
        )
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
      return true;
    } 
  }
  return false;
}


//=================================================================
bool falling_collision(Player player, Arena arena, double timeDiff)
{
  double horizontal_offset = timeDiff * player.get_velocity() + 0.1;

  for(const svg_tools::Rect& r: arena.get_obstacles()) {
    if(
      ((player.get_bottom_edge() >= (r.y)) && (player.get_bottom_edge() <= (r.y + r.height))) && 
      (
        ((player.get_right_edge() >= r.x + horizontal_offset) && (player.get_left_edge() <= r.x)) ||
        ((player.get_left_edge() <= (r.x + r.width - horizontal_offset)) && (player.get_right_edge() >= (r.x + r.width))) ||
        ((player.get_left_edge() >= r.x) && (player.get_right_edge() <= (r.x + r.width)))
      )
    ){
      return true;
    } 
  }
  return false;
}
