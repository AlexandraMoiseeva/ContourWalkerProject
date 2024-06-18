#pragma once

#include <vector>
#include <string>

#include "GeometryInit.h"


class Tool
{
public:

    int detailTypeNum = 0;
    std::vector<Node> nodes;
    std::vector<Node*> contour;

    std::vector<std::pair<int, Segment>> connect;

    LineSymStruct lineSym;

    Tool();


    Tool(std::vector<Node>& nodesValue, std::vector<int>& contourValue, std::vector<int>& symAxisPointsValue,
        std::vector<std::pair<int, Segment>>& connectValue, int detailTypeValue = 0);

protected:

    void symAxisInizialisation(std::vector<int>& symAxisPoints);

};


class Workpiece : public Tool
{
public:

    Workpiece();


    Workpiece(std::vector<Node>& nodesValue, std::vector<int>& contourValue, std::vector<int>& symAxisPointsValue,
        std::vector<std::pair<int, Segment>>& connectValue, int detailTypeValue = 0);


    std::vector<SpaceArea> intersectionSpace(Tool& otherDetail);
};