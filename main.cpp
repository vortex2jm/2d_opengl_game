#include "tinyxml2.h"
#include <GL/glut.h>
#include <GL/glu.h>
#include <iostream>
#include <GL/gl.h>
#include "utils.h"
#include "arena.h"
#include <string>
#include <vector>

// Window dimensions
const GLint Width = 700;
const GLint Height = 700;

// callbacks=========
void renderScene(void);
void keyPress(unsigned char key, int x, int y);
void keyup(unsigned char key, int x, int y);
void ResetKeyStatus();
void init(void);
void idle(void);


//svg data===================================
std::vector<svg_tools::Rect> rectangles = {};
std::vector<svg_tools::Circ> circles = {};
Arena ring;

// MAIN========================
int main(int argc, char *argv[])
{
  // File reading and initialization========================
  if(argc < 2){
    std::cerr << "Missing command line argument" << std::endl;
    exit(1);
  }

  // vectors passed by referece====================
  svg_tools::readSvg(argv[1], rectangles, circles);   
  ring.setup(rectangles);
  
  // Setting up graphic lib===================
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

  // reset matrix stack
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

//=============================//
// Callbacks                   //
//=============================//
void renderScene(void)
{
  // Clear the screen.
  glClear(GL_COLOR_BUFFER_BIT);
  
  ring.draw();

  // debug
  // glColor3f (1.0, 0, 0);  

  //  glBegin(GL_POLYGON);
  //     glVertex3f (0, 100, 0);
  //     glVertex3f (0, 0, 0);
  //     glVertex3f (100, 0, 0);
  //     glVertex3f (100, 100, 0);
  //  glEnd();

  // Draw the new frame of the game.
  glutSwapBuffers(); 
}

//============================================
void keyPress(unsigned char key, int x, int y)
{
  glutPostRedisplay();
}

//========================================
void keyup(unsigned char key, int x, int y)
{
  glutPostRedisplay();
}

//=============
void idle(void)
{
  glutPostRedisplay();
}

void ResetKeyStatus()
{

}
