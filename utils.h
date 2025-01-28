#ifndef utils_h
#define utils_h

#include "tinyxml2.h"
#include <vector>
#include <string>

// colors
#define BLACK { 0.0, 0.0, 0.0 }
#define RED { 1.0, 0.0, 0.0 }
#define GREEN { 0.0, 1.0, 0.0 }
#define BLUE { 0.0, 0.0, 1.0 }
#define YELLOW { 1.0, 1.0, 0 }

enum JumpState {
  NotJumping,
  Jumping
};

enum JumpPhase {
  Up,
  Down
};

enum FallState {
  NotFalling,
  Falling
};

enum HorizontalMoveDirection {
  Left,
  Right
};

/// @brief Tools and features to manage .svg files
namespace svg_tools {
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

  void readSvg(char * file, std::vector<Rect> &r, std::vector<Circ> &c);
}

// Transformer matrix operations
namespace matrix_tools {
  void translatePoint2d(double point[2], double offSetX, double offSetY);
  void rotatePoint2d(double point[2], double angle);
}

#endif
