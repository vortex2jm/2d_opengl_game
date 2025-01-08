#ifndef utils_h
#define utils_h

#include "tinyxml2.h"
#include <vector>
#include <string>

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

#endif
