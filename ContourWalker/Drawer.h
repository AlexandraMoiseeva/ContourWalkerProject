#pragma once

#include "Geometry.h"
#include "ContourWalkerDRW.h" 

#include <SFML/Graphics.hpp>

struct Drawer
{
private:

    std::pair<double, double> drawScale(Node node, double scale = 5);


    void drawLine(Node n1, Node n2, sf::RenderWindow& window, unsigned alpha = 255);


    void drawContour(sf::RenderWindow& window, ContourWalkerTool& cwt);


    void drawContour(sf::RenderWindow& window, ContourWalker& cwt);


    void drawSpace(sf::RenderWindow& window, CWMDrawerReader& cwm);

 public:

     void drawAll(sf::RenderWindow& window, CWMDrawerReader& cwm);
};