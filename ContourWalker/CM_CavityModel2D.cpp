#include "CM_CavityModel2D.h"


std::vector<Tool>& CM_CavityModel2D::gettools()
{
    return tools;
};


std::vector<Workpiece>& CM_CavityModel2D::getworkpieces()
{
    return workpieces;
};


std::vector<CM_Cavity2D> CM_CavityModel2D::intersectionSpace(Workpiece& oneDetail, Tool& otherDetail)
{
    std::vector<CM_Cavity2D> cavitys;
    Contour contourWP;
    Contour contourTool;
    int detailTypeValue = otherDetail.id;
    bool StartSpaceContour = false;
    bool isSym = false;
    int cavityCount = 0;
    int dopusk = 0;

    for (auto it = oneDetail.begin() + 1; it != oneDetail.end(); ++it)
    {
        if (oneDetail.contactInit[(*it)->sourceObjInfo.mesh_obj_id].first != detailTypeValue && !(*it)->isSym)
        {
            if (StartSpaceContour == false)
            {
                if (oneDetail.contactInit[(*std::prev(it))->sourceObjInfo.mesh_obj_id].first == detailTypeValue)
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
                        ++dopusk;
                        continue;
                    }

                StartSpaceContour = true;
                ++cavityCount;
            }
            else
            {
                contourWP.push_back(*(it - 1));
            }
        }
        if (oneDetail.contactInit[(*it)->sourceObjInfo.mesh_obj_id].first == detailTypeValue || (*it)->isSym)
        {
            if (StartSpaceContour == true)
            {
                StartSpaceContour = false;

                contourWP.push_back(*(it - 1));
                contourWP.push_back(*it);

                if (oneDetail.contactInit[(*it)->sourceObjInfo.mesh_obj_id].first == detailTypeValue)
                {
                    if (!isSym)
                    {
                        auto it2 = *oneDetail.contact[(*contourWP.begin())->sourceObjInfo.mesh_obj_id].second_point;
                        auto it3 = oneDetail.contact[(*it)->sourceObjInfo.mesh_obj_id].first_point;

                        for (int it1 = 1; it1 != otherDetail.end() - otherDetail.begin(); ++it1)
                        {
                            contourTool.push_back((&*(otherDetail.end() - 1) - it3) >= it1 ? *(it3 + it1)
                                : *(otherDetail.begin() + (*it3 - *(otherDetail.end() - it1))));
                            if (*(contourTool.end() - 1) == it2)
                                break;
                        }
                    }
                    else
                    {
                        auto it1 = oneDetail.contact[(*it)->sourceObjInfo.mesh_obj_id].second_point - 1;

                        while (it1 - &*otherDetail.begin() != otherDetail.end() - otherDetail.begin())
                        {
                            ++it1;
                            contourTool.push_back(*it1);
                            if ((*it1)->isSym)
                                break;
                        }
                    }

                    cavitys.emplace_back(
                        contourWP,
                        contourTool);
                }
                else
                {
                    if (!isSym)
                    {
                        auto it1 = oneDetail.contact[(*contourWP.begin())->sourceObjInfo.mesh_obj_id].second_point + 1;

                        while (it1 - &*otherDetail.begin() != 0)
                        {
                            --it1;
                            contourTool.push_front(*it1);
                            if ((*it1)->isSym)
                                break;
                        }

                        cavitys.emplace_back(
                            contourWP,
                            contourTool);
                    }
                    else
                    {
                        --cavityCount;
                    }
                }
            }
        }
    }

    if (cavityCount == 0)
        return {};

    if (dopusk != 0)
    {
        for (auto it = oneDetail.begin(); it != oneDetail.begin() + dopusk + 1; ++it)
        {
            contourWP.push_back(*it);
        }

        auto it2 = *oneDetail.contact[(*contourWP.begin())->sourceObjInfo.mesh_obj_id].second_point;
        auto it3 = oneDetail.contact[(*(oneDetail.begin() + dopusk + 1))->sourceObjInfo.mesh_obj_id].first_point;

        for (int it1 = 1; it1 != otherDetail.end() - otherDetail.begin(); ++it1)
        {
            contourTool.push_back((&*(otherDetail.end() - 1) - it3) >= it1 ? *(it3 + it1)
                : *(otherDetail.begin() + (*it3 - *(otherDetail.end() - it1))));
            if (*(contourTool.end() - 1) == it2)
                break;
        }

        cavitys.emplace_back(
            contourWP,
            contourTool);
    }

    std::vector<CM_Cavity2D>::iterator maxSquareIterator = std::prev(cavitys.end());
    double maxSquare = 0;

    for (std::vector<CM_Cavity2D>::iterator elem = cavitys.begin(); elem != cavitys.end(); ++elem)
    {
        elem->intersection();

        if (elem->spaceSquare > maxSquare)
        {
            maxSquare = elem->spaceSquare;
            maxSquareIterator = elem;
        }
    }

    cavitys.erase(maxSquareIterator);

    return cavitys;
};


void CM_CavityModel2D::findSpace()
{
    for (auto& wp_it : workpieces)
    {
        for (auto& tool_it : tools)
        {
            auto elem = intersectionSpace(wp_it, tool_it);
            cavitys.insert(cavitys.end(), elem.begin(), elem.end());
        }
    }
};


void CM_CavityModel2D::trackSpaceArea(std::vector<CM_Cavity2D>& lastSpaceAreas)
{
    int maxId = 0;

    for (auto& elem1 : cavitys)
    {
        for (auto& elem2 : lastSpaceAreas)
        {
            if (elem2.id > maxId)
                maxId = elem2.id;

            elem1.colocationSpaceArea(elem2);
        }

        if (elem1.id == -1)
            elem1.id = ++maxId;
    }
};


void CM_CavityModel2D::inizialisation(int toolNumber, int wpNumber)
{
    tools.reserve(toolNumber);
    workpieces.reserve(wpNumber);

    for (auto& wp : workpieces)
    {
        wp.contactInizialisation(tools);
        wp.inizialisation();
    }

    for (auto& tool : tools)
    {
        tool.inizialisation();
    }
};