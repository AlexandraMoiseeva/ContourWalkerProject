#pragma once
#ifndef CONTOUR_WALKER_H
#define CONTOUR_WALKER_H

#include <list>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <string>

#include "Geometry.h"


class CWM
{
public:
    std::list< std::list<SpaceArea>> spaceAreas = {};

    std::list<ContourWalkerTool> toolFigures;
    std::list<ContourWalker> wpFigures;


public:

    CWM() {};


    CWM(std::list<ContourWalkerTool>& toolFiguresValue, std::list<ContourWalker>& wpFiguresValue) 
        : toolFigures(toolFiguresValue), wpFigures(wpFiguresValue) {};


    void findSpace()
    {
        for (auto wpElem = wpFigures.begin();  wpElem != wpFigures.end(); ++wpElem)
        {
            for (auto toolElem = toolFigures.begin();  toolElem != toolFigures.end(); ++toolElem)
            {
                wpElem->intersectionSpace(*toolElem);
            }

            spaceAreas.push_back(wpElem->spaceAreas);
        }
    }


    void trackSpaceArea(std::list< std::list<SpaceArea>>& lastSpaceAreas)
    {
        int maxId = 0;
        
        for (int i = 0; i < spaceAreas.size(); ++i)
        {
            for (std::list<SpaceArea>::iterator elem = std::next(spaceAreas.begin(), i)->begin();
                elem != std::next(spaceAreas.begin(), i)->end(); ++elem)
            {
                
                if (spaceAreas.size() == lastSpaceAreas.size())
                {
                    for (std::list<SpaceArea>::iterator elem1 = std::next(lastSpaceAreas.begin(), i)->begin();
                        elem1 != std::next(lastSpaceAreas.begin(), i)->end(); ++elem1)
                    {
                        if (elem1->spaceAreaId > maxId)
                            maxId = elem1->spaceAreaId;

                        elem->colocationSpaceArea(*elem1);
                    }
                }

                if (elem->spaceAreaId == std::numeric_limits<unsigned>::max())
                    elem->spaceAreaId = ++maxId;
            }
        }
    }

};

#endif