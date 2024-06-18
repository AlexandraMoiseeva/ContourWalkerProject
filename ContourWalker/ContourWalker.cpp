#include "ContourWalker.h"

CWM::CWM() = default;


void CWM::findSpace()
{
    for (int wp_it = 0; wp_it < wpFigures.size(); ++wp_it)
    {
        for (int tool_it = 0; tool_it < toolFigures.size(); ++tool_it)
        {
            spaceAreas[wp_it * wpFigures.size() + tool_it] = wpFigures[wp_it].intersectionSpace(toolFigures[tool_it]);
        }
    }
};


void CWM::trackSpaceArea(std::vector<SpaceArea>*& lastSpaceAreas)
{
    int maxId = 0;

    for (int i = 0; i < wpFigures.size(); ++i)
    {
        for (int j = 0; j < toolFigures.size(); ++j)
        {
            for (auto& elem1 : spaceAreas[i * 1 + j])
            {
                for (auto& elem2 : lastSpaceAreas[i * wpFigures.size() + j])
                {
                    if (elem2.spaceAreaId > maxId)
                        maxId = elem2.spaceAreaId;

                    elem1.colocationSpaceArea(elem2);
                }

                if (elem1.spaceAreaId == std::numeric_limits<int>::max())
                    elem1.spaceAreaId = ++maxId;
            }
        }
    }
};