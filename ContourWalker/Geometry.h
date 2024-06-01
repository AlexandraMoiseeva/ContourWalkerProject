#pragma once
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <string>

#include "GeometryInit.h"

const enum symAxisStateValue { noneStateAxis, beginNodeAxis, bothNodeebetweenFigures };


class ContourWalkerTool
{
public:
    unsigned detailTypeNum = tool;
    std::vector<Node> nodes = {};
    std::vector<Node*> contour = {};

    std::list<std::pair<unsigned, Segment>> connect = {};

    LineSymStruct lineSym;

    ContourWalkerTool() {};


    ContourWalkerTool(std::vector<Node>& nodesValue, std::list<unsigned>& contourValue, std::list<unsigned>& symAxisPointsValue, 
        std::list<std::pair<unsigned, Segment>>& connectValue, unsigned detailTypeValue = tool)
    {
        detailTypeNum = detailTypeValue;

        nodes = nodesValue;
        for (auto elem : contourValue)
            contour.push_back(&*std::next(nodes.begin(), elem));

        symAxisInizialisation(symAxisPointsValue);

        connect = connectValue;
    };


    void symAxisInizialisation(std::list<unsigned>& symAxisPoints)
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

};


class ContourWalker : public ContourWalkerTool
{
public:

    std::list<SpaceArea> spaceAreas = {};

    ContourWalker() : ContourWalkerTool() {}


    ContourWalker(std::vector<Node>& nodesValue, std::list<unsigned>& contourValue, std::list<unsigned>& symAxisPointsValue,
        std::list<std::pair<unsigned, Segment>>& connectValue, unsigned detailTypeValue = tool) :
        ContourWalkerTool(nodesValue, contourValue, symAxisPointsValue, connectValue, detailTypeValue) {};


    void intersectionSpace(ContourWalkerTool& otherDetail)
    {
        unsigned detailTypeValue = otherDetail.detailTypeNum;
        bool StartSpaceContour = false;
        int spaceAreaCount = 0;
        int dopusk = 0;

        for (auto it = std::next(contour.begin()); it != contour.end(); ++it)
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
                                Contour((*std::next(it, -1))->placeInContour),
                                Contour(otherDetail.nodes[std::next(connect.begin(), 
                                    (*std::prev(it))->id)->second.n2].placeInContour)
                                    ));
                    }
                    else
                        if ((*std::prev(it))->isSym)
                        {
                            spaceAreas.push_back(
                                SpaceArea(
                                    detailTypeNum,
                                    detailTypeValue,
                                    Contour((*std::prev(it))->placeInContour),
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
                    std::prev(spaceAreas.end())->contourWP.push_back((*std::prev(it))->placeInContour);
                }
            }
            if (std::next(connect.begin(), (*it)->id)->first == detailTypeValue or (*it)->isSym)
            {
                if (StartSpaceContour == true)
                {
                    StartSpaceContour = false;

                    (std::prev(spaceAreas.end()))->contourWP.push_back((*std::prev(it))->placeInContour);
                    (std::prev(spaceAreas.end()))->contourWP.push_back((*it)->placeInContour);

                    if (std::next(connect.begin(), (*it)->id)->first == detailTypeValue)
                    {
                        if (std::prev(spaceAreas.end())->contourTool.beginNode != std::numeric_limits<unsigned>::max())
                        {                            
                            int it2 = --*(std::prev(spaceAreas.end()))->contourTool.begin();
                            int it3 = --otherDetail.nodes[std::next(connect.begin(), (*it)->id)->second.n1].placeInContour;

                            for (int it1 = it2; it1 != it3 and it1 != it3 - otherDetail.contour.size(); --it1)
                            {
                                (std::prev(spaceAreas.end()))->contourTool.push_front(it1 < 0 ? otherDetail.contour.size() + it1 : it1);
                            }
                        }
                        else
                        {
                            auto it1 = std::next(connect.begin(), (*it)->id)->second.n1;

                            (std::prev(spaceAreas.end()))->contourTool.push_front(it1);

                            while (it1 != otherDetail.contour.size())
                            {
                                ++it1;
                                (std::prev(spaceAreas.end()))->contourTool.push_back(it1);
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

                            (std::prev(spaceAreas.end()))->contourTool.push_back(it1);

                            while (it1 != 0)
                            {
                                --it1;
                                (std::prev(spaceAreas.end()))->contourTool.push_front(it1);
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
            return;

        if (dopusk != 0)
        {
            for (auto it = std::next(contour.begin()); it != std::next(contour.begin()) + dopusk; ++it)
            {
                std::prev(spaceAreas.end())->contourWP.push_back((*it)->placeInContour);
            }

            int it2 = *(std::prev(spaceAreas.end()))->contourTool.begin();
            int it3 = otherDetail.nodes[std::next(connect.begin(), (*(std::next(contour.begin()) + dopusk))->id)->second.n1].placeInContour;

            for (int it1 = it2; it1 != it3 and it1 != it3 - otherDetail.contour.size(); --it1)
            {
                (std::prev(spaceAreas.end()))->contourTool.push_front(it1 < 0 ? otherDetail.contour.size() + it1 : it1);
            }

        }

        std::list<SpaceArea>::iterator maxSquareIterator = std::prev(spaceAreas.end());
        double maxSquare = 0;

        for (std::list<SpaceArea>::iterator elem = std::next(spaceAreas.end(), -spaceAreaCount); elem != spaceAreas.end(); ++elem)
        {
            elem->intersection(contour, otherDetail.contour);
            
            if (elem->spaceSquare > maxSquare)
            {
                maxSquare = elem->spaceSquare;
                maxSquareIterator = elem;
            }
        }

        spaceAreas.erase(maxSquareIterator); 
    }
};

#endif