#pragma once

#include "Geometry.h"

class CWM
{
protected:
    std::vector<SpaceArea>* spaceAreas = nullptr;

    std::vector<ContourWalkerTool> toolFigures;
    std::vector<ContourWalker> wpFigures;

    CWM() {};


    void findSpace();


    void trackSpaceArea();

};