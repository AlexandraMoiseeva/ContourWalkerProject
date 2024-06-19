#include "ContourWalkerDRW.h"
#include "Reader.h"

CWMDrawerReader::CWMDrawerReader(std::string folder, std::stringstream& ss, int toolNumber, int wpNumber)
{
    toolFigures.reserve(toolNumber);
    wpFigures.reserve(wpNumber);

    spaceAreas = new std::vector<SpaceArea>[wpNumber * toolNumber];

    if (toolNumber > 1)
        for (int i = 1; i <= toolNumber; ++i)
        {
            Reader rw("../" + folder + "/" + ss.str() + "-t" + std::to_string(i) + ".csv2d", i);
            toolFigures.emplace_back(rw.nodes, rw.contour, rw.symAxisPoints, rw.connect, detailType::tool, i);
        }
    else
    {
        Reader rw("../" + folder + "/" + ss.str() + "-t" + ".csv2d", 0);
        toolFigures.emplace_back(rw.nodes, rw.contour, rw.symAxisPoints, rw.connect, detailType::tool, 0);
    }


    if (wpNumber > 1)
        for (int i = 1; i <= wpNumber; ++i)
        {
            Reader rw("../" + folder + "/" + ss.str() + "-wp" + std::to_string(i) + ".csv2d", i);
            wpFigures.emplace_back(rw.nodes, rw.contour, rw.symAxisPoints, rw.connect, detailType::workpiece, i);
        }
    else
    {
        Reader rw("../" + folder + "/" + ss.str() + "-wp" + ".csv2d", 0);
        wpFigures.emplace_back(rw.nodes, rw.contour, rw.symAxisPoints, rw.connect, detailType::workpiece, 0);
    }
};