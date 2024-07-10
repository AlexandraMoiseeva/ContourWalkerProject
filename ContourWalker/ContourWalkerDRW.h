#pragma once

#include "ContourWalker.h"


class CM_CavityModel2D_FromFile : public CM_CavityModel2D // имя файлов привести в соответствие с классами
{
public:

    CM_CavityModel2D_FromFile(std::string folder, int toolNumber, int wpNumber);
};
