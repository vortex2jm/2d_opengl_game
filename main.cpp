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


  // Horizontal left motion
  if(key_status['a']) {
    self.walk(-timeDifference);
    glMatrixMode(GL_PROJECTION);             
      glTranslated((timeDifference* 0.05), 0, 0);
    glMatrixMode(GL_MODELVIEW);
  }


  // Horizontal right motion
  if(key_status['d']) {
    self.walk(timeDifference);
    glMatrixMode(GL_PROJECTION);             
      glTranslated(-(timeDifference*0.05), 0, 0);
    glMatrixMode(GL_MODELVIEW);
  }

  // debug
  // std::cout << timeDifference << std::endl;

  // Jump
  if(jump_state == JumpState::Jumping){
    if(!self.jump(timeDifference, key_status[MOUSE_RIGHT])){
      jump_state = JumpState::NotJumping;
    }
    self.reset_legs_position();
  }


  // Criar um atributo para armazenar a posição Y do player
  // Transladar para cima enquanto o botao tiver pressionado e o tempo nao tiver excedido
  // Seguir a formula fisica para fazer a distancia da translação variar de acordo com a gravidade 
  // Se o botão parar de ser pressionado ou exceder o tempo maximo de pulo, 
  // Transladar o boneco para baixo até que ele volte para a posição Y inicial,
  // Seguindo a formula fisica para aumentar a distancia da translação gradativamente
  // OBS: talvez a formula fisica nao funcione. Transladar com velocidade constante

  glutPostRedisplay();
}


//===================================================
void mouseClick(int button, int state, int x, int y) {
  if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {

    // The jump key can be activated only when the player is not jumping
    if(jump_state == JumpState::NotJumping){
      jump_state = JumpState::Jumping;
      key_status[MOUSE_RIGHT] = 1;
      //self.jump(); // Supondo que você tenha um método `jump` para o jogador
    }
    return;
  }
  if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
    key_status[MOUSE_RIGHT] = 0;
  }
}
