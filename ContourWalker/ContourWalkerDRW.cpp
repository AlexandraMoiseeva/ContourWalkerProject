#include "ContourWalkerDRW.h"
#include "Reader.h"

CWMDrawerReader::CWMDrawerReader() {};


CWMDrawerReader::CWMDrawerReader(std::string folder, std::stringstream& ss, unsigned toolNumber, unsigned wpNumber)
{
    toolFigures.reserve(toolNumber);
    wpFigures.reserve(wpNumber);

    spaceAreas = new std::vector<SpaceArea>[wpNumber * toolNumber];

    if (toolNumber > 1)
        for (unsigned i = 1; i <= toolNumber; ++i)
        {
            Reader rw("../" + folder + "/" + ss.str() + "-t" + std::to_string(i) + ".csv2d", i);
            toolFigures.push_back(ContourWalkerTool(rw.nodes, rw.contour, rw.symAxisPoints, rw.connect, i));
        }
    else
    {
        Reader rw("../" + folder + "/" + ss.str() + "-t" + ".csv2d", 0);
        toolFigures.push_back(ContourWalkerTool(rw.nodes, rw.contour, rw.symAxisPoints, rw.connect, 0));
    }


    if (wpNumber > 1)
        for (unsigned i = 1; i <= wpNumber; ++i)
        {
            Reader rw("../" + folder + "/" + ss.str() + "-wp" + std::to_string(i) + ".csv2d", i);
            wpFigures.push_back(ContourWalker(rw.nodes, rw.contour, rw.symAxisPoints, rw.connect, i));
        }
    else
    {
        Reader rw("../" + folder + "/" + ss.str() + "-wp" + ".csv2d", 0);
        wpFigures.push_back(ContourWalker(rw.nodes, rw.contour, rw.symAxisPoints, rw.connect, 0));
    }
};


void CWMDrawerReader::findSpace()
{
    for (int wp_it = 0; wp_it < wpFigures.size(); ++wp_it)
    {
        for (int tool_it = 0; tool_it < toolFigures.size(); ++tool_it)
        {
            spaceAreas[wp_it * wpFigures.size() + tool_it] = wpFigures[wp_it].intersectionSpace(toolFigures[tool_it]);
        }
    }
};


void CWMDrawerReader::trackSpaceArea(std::vector<SpaceArea>*& lastSpaceAreas)
{
    int maxId = 0;

    for (int i = 0; i < wpFigures.size(); ++i)
    {
        for (int j = 0; j < toolFigures.size(); ++j)
        {
            for (auto& elem1 : spaceAreas[i * 1 + j])
            {
                if (true)
                {
                    for (auto& elem2 : lastSpaceAreas[i * wpFigures.size() + j])
                    {
                        if (elem2.spaceAreaId > maxId)
                            maxId = elem2.spaceAreaId;

                        elem1.colocationSpaceArea(elem2);
                    }
                }

                if (elem1.spaceAreaId == std::numeric_limits<unsigned>::max())
                    elem1.spaceAreaId = ++maxId;
            }
        }
    }
};