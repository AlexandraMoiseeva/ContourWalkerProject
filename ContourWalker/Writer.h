#pragma once

#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "ContourWalkerDRW.h"

class Writer
{
public:

    Writer();


    void write(std::string folder, CM_CavityModel2D& cm);

private:

    void writeInOut(std::ofstream& fileOutValue, CM_Cavity2D elem, bool isSym = false);
};