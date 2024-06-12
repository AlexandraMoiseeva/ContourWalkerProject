#include "Geometry.h"

ContourWalkerTool::ContourWalkerTool() {};


ContourWalkerTool::ContourWalkerTool(std::vector<Node>& nodesValue, std::vector<unsigned>& contourValue, std::vector<unsigned>& symAxisPointsValue,
    std::vector<std::pair<unsigned, Segment>>& connectValue, unsigned detailTypeValue)
{
    detailTypeNum = detailTypeValue;

    nodes.assign(nodesValue.begin(), nodesValue.end());

    for (auto& elem : contourValue)
        contour.push_back(&nodes[elem]);

    symAxisInizialisation(symAxisPointsValue);

    connect = connectValue;
};


void ContourWalkerTool::symAxisInizialisation(std::vector<unsigned>& symAxisPoints)
{
    if (symAxisPoints.size() == 0)
        return;

    double xzSum = 0, xSum = 0, zSum = 0, x2Sum = 0, n = 0;

    for (int idElem : symAxisPoints)
    {
        Node elem = *std::next(nodes.begin(), idElem);
        xzSum += elem.x * elem.z;
        xSum += elem.x;
        zSum += elem.z;
        x2Sum += elem.x * elem.x;
        ++n;
    }

    lineSym = LineSymStruct(xzSum, xSum, zSum, x2Sum, n);
};



ContourWalker::ContourWalker() : ContourWalkerTool() {};


ContourWalker::ContourWalker(std::vector<Node>& nodesValue, std::vector<unsigned>& contourValue, std::vector<unsigned>& symAxisPointsValue,
    std::vector<std::pair<unsigned, Segment>>& connectValue, unsigned detailTypeValue)
{
    detailTypeNum = detailTypeValue;

    nodes.assign(nodesValue.begin(), nodesValue.end());

    for (auto& elem : contourValue)
        contour.push_back(&nodes[elem]);

    symAxisInizialisation(symAxisPointsValue);

    connect = connectValue;
};


std::vector<SpaceArea> ContourWalker::intersectionSpace(ContourWalkerTool& otherDetail)
{
    std::vector<SpaceArea> spaceAreas = {};
    unsigned detailTypeValue = otherDetail.detailTypeNum;
    bool StartSpaceContour = false;
    int spaceAreaCount = 0;
    int dopusk = 0;

    for (auto it = contour.begin() + 1; it != contour.end(); ++it)
    {
        if (std::next(connect.begin(), (*it)->id)->first != detailTypeValue and
            !(*it)->isSym)
        {
            if (StartSpaceContour == false)
            {
                if (std::next(connect.begin(), (*std::prev(it))->id)->first == detailTypeValue)
                {
                    spaceAreas.push_back(
                        SpaceArea(
                            detailTypeNum,
                            detailTypeValue,
                            Contour((*(it - 1))->placeInContour),
                            Contour(otherDetail.nodes[std::next(connect.begin(),
                                (*(it - 1))->id)->second.n2].placeInContour)
                        ));
                }
                else
                    if ((*(it - 1))->isSym)
                    {
                        spaceAreas.push_back(
                            SpaceArea(
                                detailTypeNum,
                                detailTypeValue,
                                Contour((*(it - 1))->placeInContour),
                                Contour()
                            ));
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
        if (std::next(connect.begin(), (*it)->id)->first == detailTypeValue or (*it)->isSym)
        {
            if (StartSpaceContour == true)
            {
                StartSpaceContour = false;

                spaceAreas.back().contourWP.push_back((*std::prev(it))->placeInContour);
                spaceAreas.back().contourWP.push_back((*it)->placeInContour);

                if (std::next(connect.begin(), (*it)->id)->first == detailTypeValue)
                {
                    if (std::prev(spaceAreas.end())->contourTool.beginNode != std::numeric_limits<unsigned>::max())
                    {
                        int it2 = -- * (spaceAreas.back()).contourTool.begin();
                        int it3 = --otherDetail.nodes[std::next(connect.begin(), (*it)->id)->second.n1].placeInContour;

                        for (int it1 = it2; it1 != it3 and it1 != it3 - otherDetail.contour.size(); --it1)
                        {
                            spaceAreas.back().contourTool.push_front(it1 < 0 ? otherDetail.contour.size() + it1 : it1);
                        }
                    }
                    else
                    {
                        auto it1 = std::next(connect.begin(), (*it)->id)->second.n1;

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
                    if (std::prev(spaceAreas.end())->contourTool.beginNode != std::numeric_limits<unsigned>::max())
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
        int it3 = otherDetail.nodes[std::next(connect.begin(), (*(contour.begin() + dopusk))->id)->second.n1].placeInContour;

        for (int it1 = it2; it1 != it3 and it1 != it3 - otherDetail.contour.size(); --it1)
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