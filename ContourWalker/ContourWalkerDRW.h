#pragma once

#include "ContourWalker.h"


class CWMDrawerReader : public CWM
{
public:

    CWMDrawerReader(std::string folder, std::stringstream& ss, int toolNumber, int wpNumber);
};
