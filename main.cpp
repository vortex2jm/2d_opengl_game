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

// Window dimensions
const GLint Width = 700;
const GLint Height = 700;

// Callback declarations
void init(void);
void idle(void);
void ResetKeyStatus();
void renderScene(void);
void keyup(unsigned char key, int x, int y);
void keyPress(unsigned char key, int x, int y);

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
  glutDisplayFunc(renderScene);
  glutKeyboardFunc(keyPress);
  glutIdleFunc(idle);
  glutKeyboardUpFunc(keyup);

  // Setup
  init();

  glutMainLoop();
  return 0;
}

//=============
void init(void)
{
  // Erasing frames and keys
  ResetKeyStatus();
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

//============================================
void keyPress(unsigned char key, int x, int y){}

//========================================
void keyup(unsigned char key, int x, int y){}

//=============
void idle(void){}

//==================
void ResetKeyStatus(){}
