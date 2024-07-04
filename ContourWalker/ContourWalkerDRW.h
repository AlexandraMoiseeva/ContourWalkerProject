#pragma once

#include "ContourWalker.h"


class CM_CavityModel2D_FromFile : public CM_CavityModel2D
{
public:

    CM_CavityModel2D_FromFile(std::string folder, int toolNumber, int wpNumber);
};
