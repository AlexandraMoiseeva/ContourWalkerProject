#include "stdafx.h"
#include "CM_CavityModel2D.h"


Tool& CM_CavityModel2D::getToolById(int i)
{
    return tools[ placeInVectorByIdTool[i] ];
};


Workpiece& CM_CavityModel2D::getWorkpieceById(int i)
{
    return workpieces[ placeInVectorByIdWorkpiece[i] ];
};


std::vector<Tool>& CM_CavityModel2D::getTools()
{
    return tools;
};


std::vector<Workpiece>& CM_CavityModel2D::getWorkpieces()
{
    return workpieces;
};


std::vector<CM_Cavity2D>& CM_CavityModel2D::getCavities()
{
    return cavities;
};


std::vector<CM_Cavity2D> CM_CavityModel2D::intersectionCavity
(Workpiece& workpiece, Tool& tool)
{
    std::vector<CM_Cavity2D> cavities;

    Contour contourWP;
    Contour contourTool;

    bool StartCavityContour = false;
    bool isSym = false;

    int shift = 0;

    auto isContact = [&] (std::vector<Node*>::iterator n) 
        { return workpiece.getContactEdge(*n).source_body_id == tool.id; };

    for (auto it = workpiece.getContour().begin() + 1;
        it != workpiece.getContour().end(); ++it)
    {
        if (!isContact(it) && !(*it)->isSym)
        {
            if (StartCavityContour == false)
            {
                if (isContact(it - 1))
                {
                    contourWP = Contour(*(it - 1));
                    contourTool = Contour();
                    isSym = false;
                }
                else
                    if ((*(it - 1))->isSym)
                    {
                        contourWP = Contour(*(it - 1));
                        contourTool = Contour();
                        isSym = true;
                    }
                    else
                    {
                        ++shift;
                        continue;
                    }

                StartCavityContour = true;
            }
            else
                contourWP.push_back(*(it - 1));
        }
        else
        {
            if (StartCavityContour == true)
            {
                StartCavityContour = false;

                contourWP.push_back(*(it - 1));
                contourWP.push_back(*it);

                if (isContact(it))
                {
                    if (!isSym)
                    {
                        auto it2 = *workpiece.getContactEdge(*contourWP.begin()).second_point;
                        auto it3 = workpiece.getContactEdge(*it).first_point;

                        for (int it1 = 1; it1 != tool.getContour().size(); ++it1)
                        {
                            contourTool.push_back(
                                (&*(tool.getContour().end() - 1) - it3)
                                >= it1 ? *(it3 + it1)
                                : *(tool.getContour().begin() 
                                    + (*it3 - *(tool.getContour().end() - it1))));
                            if (*(contourTool.end() - 1) == it2)
                                break;
                        }
                    }
                    else
                    {
                        auto it1 = workpiece.getContactEdge(*it).second_point - 1;

                        while (it1 - &*tool.getContour().begin() !=
                            tool.getContour().size())
                        {
                            ++it1;
                            contourTool.push_back(*it1);
                            if ((*it1)->isSym)
                                break;
                        }
                    }

                    cavities.emplace_back(contourWP, contourTool);
                }
                else
                {
                    if (!isSym)
                    {
                        auto it1 = workpiece.getContactEdge(
                            *contourWP.begin()).second_point + 1;

                        while (it1 != &*tool.getContour().begin())
                        {
                            --it1;
                            contourTool.push_front(*it1);
                            if ((*it1)->isSym)
                                break;
                        }

                        cavities.emplace_back(contourWP, contourTool);
                    }
                }
            }
        }
    }

    if (cavities.empty())
        return {};

    if (shift != 0)
    {
        for (auto it = workpiece.getContour().begin();
            it != workpiece.getContour().begin() + shift + 1; ++it)
        {
            contourWP.push_back(*it);
        }

        auto it2 = 
            *workpiece.getContactEdge(*contourWP.begin()).second_point;
        auto it3 = workpiece.getContactEdge(
            *(workpiece.getContour().begin() + shift + 1)).first_point;

        for (int it1 = 1; it1 != tool.getContour().size(); ++it1)
        {
            contourTool.push_back(
                (&*(tool.getContour().end() - 1) - it3) >= it1 ?
                *(it3 + it1) : *(tool.getContour().begin() +
                    (*it3 - *(tool.getContour().end() - it1))));
            if (*(contourTool.end() - 1) == it2)
                break;
        }

        cavities.emplace_back(contourWP, contourTool);
    }

    auto maxSquareIterator = cavities.begin();
    double maxSquare = 0;

    for (auto elem = cavities.begin(); elem != cavities.end(); ++elem)
    {
        elem->squareCalculating();

        if (elem->cavitySquare > maxSquare)
        {
            maxSquare = elem->cavitySquare;
            maxSquareIterator = elem;
        }
    }

    cavities.erase(maxSquareIterator);

    return cavities;
};


void CM_CavityModel2D::findCavity()
{
    for (auto& wp_it : workpieces)
    {
        for (auto& tool_it : tools)
        {
            auto elem = intersectionCavity(wp_it, tool_it);
            cavities.insert(cavities.end(), 
                std::make_move_iterator(elem.begin()),
                std::make_move_iterator(elem.end()));
        }
    }
};


void CM_CavityModel2D::trackCavity(std::vector<CM_Cavity2D>& lastCavity)
{
    int maxId = 0;

    for (auto& elem1 : cavities)
    {
        for (auto& elem2 : lastCavity)
        {
            if (elem2.id > maxId)
                maxId = elem2.id;

            elem1.colocationCavity(elem2);
        }

        if (elem1.id == -1)
            elem1.id = ++maxId;
    }
};


void CM_CavityModel2D::initialisation(int toolNumber, int wpNumber)
{
    for (int i = 0; i < tools.size(); ++i)
    {
        auto& tool = tools[i];

        tool.initialisation();

        placeInVectorByIdTool[tool.id] = i;
    }
    
    for (int i = 0; i < workpieces.size(); ++i)
    {
        auto& wp = workpieces[i];

        wp.contactInitialisation(tools, placeInVectorByIdTool);
        wp.initialisation();
        
        placeInVectorByIdWorkpiece[wp.id] = i;
    }
};