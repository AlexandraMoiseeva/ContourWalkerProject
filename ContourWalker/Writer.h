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


    void write(std::string folder, std::stringstream& ss, CWMDrawerReader& cwm);

private:

    void writeInOut(std::ofstream& fileOutValue, SpaceArea elem, std::vector<Node*>& cntrWP, std::vector<Node*>& cntrTool, bool isSym = false);
};