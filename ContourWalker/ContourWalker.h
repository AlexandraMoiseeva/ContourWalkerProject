#pragma once

#include "Geometry.h"

class CM_CavityModel2D
{
protected:
    CM_CavityModel2D();

    std::vector<Tool> toolFigures;
    std::vector<Workpiece> wpFigures;

public:

    std::vector<SpaceArea>* spaceAreas = nullptr;


    const std::vector<Tool>& getToolFigures() const;


    const std::vector<Workpiece>& getWpFigures() const;


    void findSpace();


    void trackSpaceArea(std::vector<SpaceArea>*& lastSpaceAreas);


    void contactInizialisation();
};