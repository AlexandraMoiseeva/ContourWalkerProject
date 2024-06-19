#pragma once

#include <vector>
#include <string>

#include "GeometryInit.h"

class Tool
{
public:

    detailTypeValue detail_type;

    std::vector<Node> nodes;
    std::vector<Node*> contour;

    std::vector<std::pair<int, Segment>> connect;

    LineSymStruct lineSym;

    Tool();


    Tool(std::vector<Node>& nodesValue, std::vector<int>& contourValue, std::vector<int>& symAxisPointsValue,
        std::vector<std::pair<int, Segment>>& connectValue, detailType detail_type_value, int detail_id_value);

protected:

    void symAxisInizialisation(std::vector<int>& symAxisPoints);

};


class Workpiece : public Tool
{
public:

    Workpiece();


    Workpiece(std::vector<Node>& nodesValue, std::vector<int>& contourValue, std::vector<int>& symAxisPointsValue,
        std::vector<std::pair<int, Segment>>& connectValue, detailType detail_type_value, int detail_id_value);


    std::vector<SpaceArea> intersectionSpace(Tool& otherDetail);
};