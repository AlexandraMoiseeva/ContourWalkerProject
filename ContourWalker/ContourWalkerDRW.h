#pragma once

#include "ContourWalker.h"


class CWMDrawerReader : protected CWM
{
public:
    std::vector<SpaceArea>* spaceAreas = nullptr;

    std::vector<ContourWalkerTool> toolFigures;
    std::vector<ContourWalker> wpFigures;

    CWMDrawerReader();


    CWMDrawerReader(std::string folder, std::stringstream& ss, unsigned toolNumber, unsigned wpNumber);


    void findSpace();


    void trackSpaceArea(std::vector<SpaceArea>*& lastSpaceAreas);
};
