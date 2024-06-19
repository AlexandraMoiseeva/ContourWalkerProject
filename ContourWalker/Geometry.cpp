#include "Geometry.h"

Tool::Tool() = default;


Tool::Tool(std::vector<Node>& nodesValue, std::vector<int>& contourValue, std::vector<int>& symAxisPointsValue,
    std::vector<std::pair<int, Segment>>& connectValue, detailType detail_type_value, int detail_id_value) : detail_type(detail_type_value, detail_id_value)
{
    nodes.assign(nodesValue.begin(), nodesValue.end());

    for (int const& elem : contourValue)
        contour.push_back(&nodes[elem]);

    symAxisInizialisation(symAxisPointsValue);

    connect = connectValue;
};


void Tool::symAxisInizialisation(std::vector<int>& symAxisPoints)
{
    if (symAxisPoints.empty())
        return;

    double xzSum = 0;
    double xSum = 0;
    double zSum = 0;
    double x2Sum = 0;
    double n = 0;

    for (int idElem : symAxisPoints)
    {
        Node elem = *std::next(nodes.begin(), idElem);
        xzSum += elem.coordinate.x * elem.coordinate.z;
        xSum += elem.coordinate.x;
        zSum += elem.coordinate.z;
        x2Sum += elem.coordinate.x * elem.coordinate.x;
        ++n;
    }

    lineSym = LineSymStruct(xzSum, xSum, zSum, x2Sum, n);
};



Workpiece::Workpiece() : Tool() {};


Workpiece::Workpiece(std::vector<Node>& nodesValue, std::vector<int>& contourValue, std::vector<int>& symAxisPointsValue,
    std::vector<std::pair<int, Segment>>& connectValue, detailType detail_type_value, int detail_id_value)
{
    detail_type = detailTypeValue(detail_type_value, detail_id_value);

    nodes.assign(nodesValue.begin(), nodesValue.end());

    for (int const& elem : contourValue)
        contour.push_back(&nodes[elem]);

    symAxisInizialisation(symAxisPointsValue);

    connect = connectValue;
};


std::vector<SpaceArea> Workpiece::intersectionSpace(Tool& otherDetail)
{
    std::vector<SpaceArea> spaceAreas = {};
    int detailTypeValue = otherDetail.detail_type.detail_id;
    bool StartSpaceContour = false;
    int spaceAreaCount = 0;
    int dopusk = 0;

    for (auto it = contour.begin() + 1; it != contour.end(); ++it)
    {
        if (std::next(connect.begin(), (*it)->sourceObjInfo.mesh_obj_id)->first != detailTypeValue && !(*it)->isSym)
        {
            if (StartSpaceContour == false)
            {
                if (std::next(connect.begin(), (*std::prev(it))->sourceObjInfo.mesh_obj_id)->first == detailTypeValue)
                {
                    spaceAreas.emplace_back(
                        detail_type.detail_id,
                            detailTypeValue,
                            Contour((*(it - 1))->placeInContour),
                            Contour(otherDetail.nodes[std::next(connect.begin(),
                                (*(it - 1))->sourceObjInfo.mesh_obj_id)->second.n2].placeInContour));
                }
                else
                    if ((*(it - 1))->isSym)
                    {
                        spaceAreas.emplace_back(
                            detail_type.detail_id,
                                detailTypeValue,
                                Contour((*(it - 1))->placeInContour),
                                Contour());
                    }
                    else
                    {
                        ++dopusk;
                        continue;
                    }

                StartSpaceContour = true;
                ++spaceAreaCount;
            }
            else
            {
                spaceAreas.back().contourWP.push_back((*(it - 1))->placeInContour);
            }
        }
        if (connect[(*it)->sourceObjInfo.mesh_obj_id].first == detailTypeValue || (*it)->isSym)
        {
            if (StartSpaceContour == true)
            {
                StartSpaceContour = false;

                spaceAreas.back().contourWP.push_back((*std::prev(it))->placeInContour);
                spaceAreas.back().contourWP.push_back((*it)->placeInContour);

                if (std::next(connect.begin(), (*it)->sourceObjInfo.mesh_obj_id)->first == detailTypeValue)
                {
                    if (spaceAreas.back().contourTool.beginNode != std::numeric_limits<int>::max())
                    {
                        int it2 = -- * (spaceAreas.back()).contourTool.begin();
                        int it3 = -- otherDetail.nodes[std::next(connect.begin(), (*it)->sourceObjInfo.mesh_obj_id)->second.n1].placeInContour;

                        for (int it1 = it2; it1 != it3 && it1 != it3 - otherDetail.contour.size(); --it1)
                        {
                            spaceAreas.back().contourTool.push_front(it1 < 0 ? otherDetail.contour.size() + it1 : it1);
                        }
                    }
                    else
                    {
                        auto it1 = std::next(connect.begin(), (*it)->sourceObjInfo.mesh_obj_id)->second.n1;

                        spaceAreas.back().contourTool.push_front(it1);

                        while (it1 != otherDetail.contour.size())
                        {
                            ++it1;
                            spaceAreas.back().contourTool.push_back(it1);
                            if ((otherDetail.contour[it1])->isSym)
                                break;
                        }
                    }
                }
                else
                {
                    if (spaceAreas.back().contourTool.beginNode != std::numeric_limits<int>::max())
                    {
                        auto it1 = *(std::prev(spaceAreas.end()))->contourTool.begin();

                        spaceAreas.back().contourTool.push_back(it1);

                        while (it1 != 0)
                        {
                            --it1;
                            spaceAreas.back().contourTool.push_front(it1);
                            if ((otherDetail.contour[it1])->isSym)
                                break;
                        }
                    }
                    else
                    {
                        spaceAreas.pop_back();
                        --spaceAreaCount;
                    }
                }
            }
        }
    }

    if (spaceAreaCount == 0)
        return {};

    if (dopusk != 0)
    {
        for (auto it = std::next(contour.begin()); it != std::next(contour.begin()) + dopusk; ++it)
        {
            spaceAreas.back().contourWP.push_back((*it)->placeInContour);
        }

        int it2 = *(spaceAreas.back()).contourTool.begin();
        int it3 = otherDetail.nodes[std::next(connect.begin(), (*(contour.begin() + dopusk))->sourceObjInfo.mesh_obj_id)->second.n1].placeInContour;

        for (int it1 = it2; it1 != it3 && it1 != it3 - otherDetail.contour.size(); --it1)
        {
            spaceAreas.back().contourTool.push_front(it1 < 0 ? otherDetail.contour.size() + it1 : it1);
        }

    }

    std::vector<SpaceArea>::iterator maxSquareIterator = std::prev(spaceAreas.end());
    double maxSquare = 0;

    for (std::vector<SpaceArea>::iterator elem = std::next(spaceAreas.end(), -spaceAreaCount); elem != spaceAreas.end(); ++elem)
    {
        elem->intersection(contour, otherDetail.contour);

        if (elem->spaceSquare > maxSquare)
        {
            maxSquare = elem->spaceSquare;
            maxSquareIterator = elem;
        }
    }

    spaceAreas.erase(maxSquareIterator);

    return spaceAreas;
}