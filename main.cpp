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
bool is_player_colliding_horizontally(Player player, Arena arena, HorizontalMoveDirection direction, double timeDiff);
bool is_player_into_arena_vertically(Player player, Arena arena);
bool is_player_colliding_vertically(Player player, Arena arena, double timeDiff);
bool falling_collision(Player player, Arena arena, double timeDiff);


//svg data===================================
std::vector<svg_tools::Rect> rectangles = {};
std::vector<svg_tools::Circ> circles = {};

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

  
  // The horizontal colision check will happen here, not into player class
  // the player class will provider getters


  // Horizontal left motion
  if(key_status['a']) {
    // Checking arena limits
    if(is_player_into_arena_horizontally(self, ring, HorizontalMoveDirection::Left)) {
      // Checking collision against obstacles
      if(!is_player_colliding_horizontally(self, ring, HorizontalMoveDirection::Left, timeDifference)) {
        
        self.walk(-timeDifference);
        
        glMatrixMode(GL_PROJECTION);             
          glTranslated((timeDifference* 0.05), 0, 0);
        glMatrixMode(GL_MODELVIEW);
      }
    }
  }

  // Horizontal right motion
  if(key_status['d']) {
    // Checking arena limits
    if(is_player_into_arena_horizontally(self, ring, HorizontalMoveDirection::Right)) {
      // Checking collision against obstacles
      if(!is_player_colliding_horizontally(self, ring, HorizontalMoveDirection::Right, timeDifference)) {
        
        self.walk(timeDifference);
        
        glMatrixMode(GL_PROJECTION);             
          glTranslated(-(timeDifference*0.05), 0, 0);
        glMatrixMode(GL_MODELVIEW);


        //debug
        // std::cout << (timeDifference * 0.05) << std::endl;
      }
    }
  }


  //Gravity physics
  if(jump_state == JumpState::NotJumping) {
    
    // self.set_jump_phase_to_down();

    self.fall(
      timeDifference, 
      falling_collision(self, ring, timeDifference)  
    );

    // self.reset_legs_position();
    
    // if(is_player_into_arena_vertically(self, ring)){

      // if(!is_player_colliding_vertically(self, ring, timeDifference)){
      //   self.fall(timeDifference);
      // } else {
      //   self.set_fall_time();
      // }

    // }
    // self.fall(
    //   ring.get_y() + ring.get_height(), 
    //   timeDifference,
    //   is_player_colliding_vertically(self, ring, timeDifference)   
    // );
  }


  // Jump
  if(jump_state == JumpState::Jumping){
    // Legs dont move when player is jumping
    self.reset_legs_position(); 
    
    // Checking if player is into arena
    if(is_player_into_arena_vertically(self, ring)) {

      // If jump() returns 0, jump stops
      if(!self.jump(
          timeDifference, 
          key_status[MOUSE_RIGHT],
          is_player_colliding_vertically(self, ring, timeDifference)) // Checking collision
        ) {
        jump_state = JumpState::NotJumping;
      }
    }
  }

  glutPostRedisplay();
}


//===================================================
void mouseClick(int button, int state, int x, int y) {
  if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {

    // The jump key can be activated only when the player is not jumping
    if(jump_state == JumpState::NotJumping){
      jump_state = JumpState::Jumping;

      //void set_jump_phase_to_up(); // VERIFY
      
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
// put direction into player attributes
bool is_player_into_arena_horizontally(Player player, Arena arena, HorizontalMoveDirection direction)
{
  if(direction == HorizontalMoveDirection::Left) {
    return ((player.get_cx() - (player.get_width()/2)) >= arena.get_x()); 
  }
  // Right
  return ((player.get_cx() + (player.get_width()/2)) <= (arena.get_x() + arena.get_width()));
}

//=============================================================
bool is_player_into_arena_vertically(Player player, Arena arena)
{
  // if(player.get_jump_phase() == JumpPhase::Up) {
  //   return ((player.get_cy() - (player.get_height()/2)) >= arena.get_y());
  // }
  return 
    ((player.get_cy() + (player.get_height()/2)) <= (arena.get_y() + arena.get_height())) &&
    ((player.get_cy() - (player.get_height()/2)) >= arena.get_y());
}


//===============================================================================
bool is_player_colliding_horizontally(Player player, Arena arena,  HorizontalMoveDirection direction, double timeDiff) {
  
  double vertical_offset = timeDiff * player.get_velocity() + 0.1;
  
  if(direction == HorizontalMoveDirection::Right) {
    for(const svg_tools::Rect& r: arena.get_obstacles()) {
      if(
        // by with 
        (player.get_cx() + (player.get_width()/2) >= (r.x)) &&  
        (player.get_cx() + (player.get_width()/2) <= (r.x + r.width)) &&
        // by height
        ( 
          // First case=======
          //     ___
          //  ---|_|
          //  |_|
          //=================== 
          (((r.y + r.height - vertical_offset) >= (player.get_cy() - (player.get_height()/2))) && 
           ((r.y) <= (player.get_cy() - (player.get_height()/2)))
          ) || 
          // Second case=======
          //  ___
          //  | |___
          //  ---|_|
          //===================
          ( ((r.y + vertical_offset) <= (player.get_cy() + (player.get_height()/2))) &&
            ((r.y + r.height) >= (player.get_cy() + (player.get_height()/2)))
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
          (((r.y) >= (player.get_cy() - (player.get_height()/2))) &&
           (r.y + r.height) <= (player.get_cy() + (player.get_height()/2))
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
      (player.get_cx() - (player.get_width()/2) <= (r.x + r.width)) &&  
      (player.get_cx() - (player.get_width()/2) >= (r.x)) &&
      (
        (((r.y + r.height - vertical_offset) >= (player.get_cy() - (player.get_height()/2))) && 
         ((r.y) <= (player.get_cy() - (player.get_height()/2)))
        ) ||
        
        ( ((r.y + vertical_offset) <= (player.get_cy() + (player.get_height()/2))) &&
          ((r.y + r.height) >= (player.get_cy() + (player.get_height()/2)))
        ) ||
        (((r.y) >= (player.get_cy() - (player.get_height()/2))) &&
         (r.y + r.height) <= (player.get_cy() + (player.get_height()/2))
        )
      )
    ){
      return true;
    }
  }
  return false;
}

//============================================================
bool is_player_colliding_vertically(Player player, Arena arena, double timeDiff)
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
