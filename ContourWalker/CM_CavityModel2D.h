#pragma once

#include "Geometry.h"

using namespace cavity2d;


class CM_CavityModel2D
{
protected:
    CM_CavityModel2D() = default;


    std::vector<Tool> tools;
    std::vector<Workpiece> workpieces;

    std::vector<CM_Cavity2D> intersectionSpace(Workpiece& oneDetail, Tool& otherDetail);

public:

    std::vector<CM_Cavity2D> cavitys;


    std::vector<Tool>& gettools();


    std::vector<Workpiece>& getworkpieces();


    void findSpace();


    void trackSpaceArea(std::vector<CM_Cavity2D>& lastSpaceAreas);


    void inizialisation(int toolNumber, int wpNumber);
};