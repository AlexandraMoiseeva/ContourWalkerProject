#include "ContourWalker.h"

CM_CavityModel2D::CM_CavityModel2D() = default; // определение конструктора по умолчанию, который ничего не делает, лучше оставлять в хидере


CM_CavityModel2D::CM_CavityModel2D(int toolNumber, int wpNumber)
{
    toolFigures.reserve(toolNumber);
    wpFigures.reserve(wpNumber);
};


std::vector<Tool>& CM_CavityModel2D::getToolFigures()
{
    return toolFigures;
};


std::vector<Workpiece>& CM_CavityModel2D::getWpFigures()
{
    return wpFigures;
};


void CM_CavityModel2D::findSpace()
{
    for (int wp_it = 0; wp_it < wpFigures.size(); ++wp_it)
    {
        for (int tool_it = 0; tool_it < toolFigures.size(); ++tool_it)
        {
            cavitys[wp_it * wpFigures.size() + tool_it] = wpFigures[wp_it].intersectionSpace(toolFigures[tool_it]);
        }
    }
};


void CM_CavityModel2D::trackSpaceArea(std::vector<CM_Cavity2D>*& lastSpaceAreas)
{
    int maxId = 0;

    for (int i = 0; i < wpFigures.size(); ++i)
    {
        for (int j = 0; j < toolFigures.size(); ++j)
        {
            for (auto& elem1 : cavitys[i * 1 + j])
            {
                for (auto& elem2 : lastSpaceAreas[i * wpFigures.size() + j])
                {
                    if (elem2.cavityId > maxId)
                        maxId = elem2.cavityId;

                    elem1.colocationSpaceArea(elem2);
                }

                if (elem1.cavityId == std::numeric_limits<int>::max())
                    elem1.cavityId = ++maxId;
            }
        }
    }
};


void CM_CavityModel2D::contactInizialisation()
{
    for (auto& wp : wpFigures)
    {
        wp.contactInizialisation(toolFigures);
        wp.inizialisation();
    }

    for (auto& tool : toolFigures)
    {
        tool.inizialisation();
    }
};