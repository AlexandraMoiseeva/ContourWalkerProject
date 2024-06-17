#pragma once

#include <vector>
#include <string>

#include "GeometryInit.h"


class ContourWalkerTool
{
public:

    unsigned detailTypeNum = 0;
    std::vector<Node> nodes;
    std::vector<Node*> contour;

    std::vector<std::pair<unsigned, Segment>> connect;

    LineSymStruct lineSym;

    ContourWalkerTool();


    ContourWalkerTool(std::vector<Node>& nodesValue, std::vector<unsigned>& contourValue, std::vector<unsigned>& symAxisPointsValue,
        std::vector<std::pair<unsigned, Segment>>& connectValue, unsigned detailTypeValue = 0);

protected:

    void symAxisInizialisation(std::vector<unsigned>& symAxisPoints);

};


class ContourWalker : public ContourWalkerTool
{
public:

    ContourWalker();


    ContourWalker(std::vector<Node>& nodesValue, std::vector<unsigned>& contourValue, std::vector<unsigned>& symAxisPointsValue,
        std::vector<std::pair<unsigned, Segment>>& connectValue, unsigned detailTypeValue = 0);


    std::vector<SpaceArea> intersectionSpace(ContourWalkerTool& otherDetail);
};