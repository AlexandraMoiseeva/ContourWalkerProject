#pragma once

#include "Geometry.h"

class CM_CavityModel2D
{
protected:
    CM_CavityModel2D();


    CM_CavityModel2D(int toolNumber, int wpNumber);

    std::vector<Tool> toolFigures;
    std::vector<Workpiece> wpFigures;

public:

    std::vector<CM_Cavity2D>* cavitys = nullptr;


    const std::vector<Tool>& getToolFigures() const;


    const std::vector<Workpiece>& getWpFigures() const;


    void findSpace();


    void trackSpaceArea(std::vector<CM_Cavity2D>*& lastSpaceAreas);


    void contactInizialisation();
};