#include "utils.h"
#include <iostream>

namespace svg_tools {
  
  /// @brief Reads a .svg file and populates circles and rectangles vectors
  /// @param file 
  /// @param r 
  /// @param c 
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
    tinyxml2::XMLElement* circ = doc.FirstChildElement( "svg" )->FirstChildElement( "circle" );

    while(circ != NULL){
      double cx = std::stod(circ->Attribute("cx"));
      double cy = std::stod(circ->Attribute("cy"));
      double r = std::stod(circ->Attribute("r"));
      std::string color = circ->Attribute("fill");

      c.push_back({ cx, cy, r, color });

      // debug
      // std::cout << cx << cy << r << color << std::endl;

      circ = circ->NextSiblingElement("circle");
    }
  }
}
