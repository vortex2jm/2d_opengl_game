#include "tinyxml2.h"
#include <GL/glut.h>
#include <GL/glu.h>
#include <iostream>
#include <GL/gl.h>
#include <string>
#include <vector>

// MOVE TO UTILS
// Intermediate structures to read the svg
struct Rect {
  double x;
  double y;
  double width;
  double height;
  std::string color;
};

struct Circ {
  double cx;
  double cy;
  double r;
  std::string color;
};

// Window dimensions
const GLint Width = 700;
const GLint Height = 700;

// Viewing dimensions
const GLint ViewingWidth = 500;
const GLint ViewingHeight = 500;

//auxiliar functions

// MOVE TO UTILS
void readSvg(char * file, std::vector<Rect> &r, std::vector<Circ> &c);


// callbacks
void renderScene(void);
void keyPress(unsigned char key, int x, int y);
void keyup(unsigned char key, int x, int y);
void ResetKeyStatus();
void init(void);
void idle(void);

//=============================//
// Main                        //
//=============================//
int main(int argc, char *argv[])
{
  if(argc < 2){
    perror("Missing command line argument!\n");
    exit(1);
  }

  // reading elements buffer
  std::vector<Rect> rectangles = {};
  std::vector<Circ> circles = {};
  readSvg(argv[1], rectangles, circles);


  // Setting up
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // Creating the window.
  glutInitWindowSize(Width, Height);
  glutInitWindowPosition(150,50);
  glutCreateWindow("2D Game");

  // Defining callbacks.
  glutDisplayFunc(renderScene);
  glutKeyboardFunc(keyPress);
  glutIdleFunc(idle);
  glutKeyboardUpFunc(keyup);

  init();

  glutMainLoop();
  return 0;
}

//=============================//
// Callbacks                   //
//=============================//
void renderScene(void)
{
  // Clear the screen.
  glClear(GL_COLOR_BUFFER_BIT);
  
  glColor3f (1.0, 0, 0);  

   glBegin(GL_POLYGON);
      glVertex3f (0, 0, 0);
      glVertex3f (0, 0, 0);
      glVertex3f (0, 0, 0);
      glVertex3f (0, 0, 0);
   glEnd();
  
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
void init(void)
{
    ResetKeyStatus();
    // The color the windows will redraw. Its done to erase the previous frame.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black, no opacity(alpha).

    glMatrixMode(GL_PROJECTION); // Select the projection matrix
    glOrtho(-(ViewingWidth/2),     // X coordinate of left edge
            (ViewingWidth/2),     // X coordinate of right edge
            -(ViewingHeight/2),     // Y coordinate of bottom edge
            (ViewingHeight/2),     // Y coordinate of top edge
            -100,     // Z coordinate of the “near” plane
            100);    // Z coordinate of the “far” plane
    glMatrixMode(GL_MODELVIEW); // Select the projection matrix
    glLoadIdentity();

}

//=============
void idle(void)
{
  glutPostRedisplay();
}

void ResetKeyStatus()
{

}

//=====================================================================
void readSvg(char * file, std::vector<Rect> &r, std::vector<Circ> &c){
  tinyxml2::XMLDocument doc;
	doc.LoadFile(file);
  
  // Rectangles reading
  tinyxml2::XMLElement* rect = doc.FirstChildElement( "svg" )->FirstChildElement("rect");

  while(rect != NULL){
    double x = std::stod(rect->Attribute("x"));
    double y = std::stod(rect->Attribute("y"));
    double width = std::stod(rect->Attribute("width"));
    double height = std::stod(rect->Attribute("height"));
    std::string color = rect->Attribute("fill");

    r.push_back({ x, y, width, height, color });

    // debug
    // std::cout << x << y << width << height << color << std::endl;

    rect = rect->NextSiblingElement("rect");
  }

  // Circles reading  
  tinyxml2::XMLElement* circ = doc.FirstChildElement( "svg" )->FirstChildElement("circ");

  while(circ != NULL){
    double cx = std::stod(rect->Attribute("cx"));
    double cy = std::stod(rect->Attribute("cy"));
    double r = std::stod(rect->Attribute("r"));
    std::string color = rect->Attribute("fill");

    c.push_back({ cx, cy, r, color });

    // debug
    // std::cout << x << y << width << height << color << std::endl;

    circ = circ->NextSiblingElement("circ");
  }
}
