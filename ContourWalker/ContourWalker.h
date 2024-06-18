#pragma once

#include "Geometry.h"

class CWM
{
protected:
    CWM();
public:
    std::vector<SpaceArea>* spaceAreas = nullptr;

    std::vector<Tool> toolFigures;
    std::vector<Workpiece> wpFigures;


    void findSpace();


    void trackSpaceArea(std::vector<SpaceArea>*& lastSpaceAreas);
};