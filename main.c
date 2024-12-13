#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// Window dimensions
const GLint Width = 700;
const GLint Height = 700;

// Viewing dimensions
const GLint ViewingWidth = 500;
const GLint ViewingHeight = 500;

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
  // Initialize openGL with Double buffer and RGB color without transparency.
  // Its interesting to try GLUT_SINGLE instead of GLUT_DOUBLE.
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // Create the window.
  glutInitWindowSize(Width, Height);
  glutInitWindowPosition(150,50);
  glutCreateWindow("2D Game");

  // Define callbacks.
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