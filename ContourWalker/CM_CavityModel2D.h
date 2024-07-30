#pragma once

#include "Bodies.h"

using namespace cavity2d;


class CM_CavityModel2D
{
protected:
    CM_CavityModel2D() = default;


    std::vector<Tool> tools;
    std::vector<Workpiece> workpieces;

    std::vector<CM_Cavity2D> cavities;

    std::vector<CM_Cavity2D> intersectionCavity(Workpiece& workpiece, Tool& tool);
    
public:
    std::vector<Tool>& getTools();


    std::vector<Workpiece>& getWorkpieces();


    std::vector<CM_Cavity2D>& getCavities();


    void findCavity();


    void trackCavity(std::vector<CM_Cavity2D>& lastCavity);


    void initialisation(int toolNumber, int wpNumber);
};