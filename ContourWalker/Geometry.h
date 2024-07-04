#pragma once

#include <vector>
#include <string>

#include "GeometryInit.h"

class DetailInit
{
protected:

public:

    detailTypeValue detail_type;

    std::vector<Node> nodes;
    std::vector<Node*> contour;

    std::vector<int> symAxisPoints;
    std::vector<std::pair<int, Edge>> contactInit;

    struct EdgeNode
    {
        Node** first_point = nullptr;
        Node** second_point = nullptr;

        EdgeNode() = default;


        EdgeNode(Node** first_point_value, Node** second_point_value)
            : first_point(first_point_value), second_point(second_point_value) {};

    };


    std::vector<EdgeNode> contact;


    LineSymStruct lineSym;


    DetailInit(int nodeAmount, detailType detail_type_value, int detail_id_value);


    void addNode(Node nodeValue);


    void addContour(int idNode);


    void addSymAxisPoint(int point);


    void addContact(std::pair<int, Edge> contactInit);


    void inizialisation();

protected:

    void symAxisInizialisation(std::vector<int>& symAxisPoints);
};
class Tool : public DetailInit
{
public:
    Tool(int nodeAmount, detailType detail_type_value, int detail_id_value);
};


class Workpiece : public DetailInit
{
public:

    Workpiece(int nodeAmount, detailType detail_type_value, int detail_id_value);


    void contactInizialisation(std::vector<Tool>& details);


    std::vector<SpaceArea> intersectionSpace(Tool& otherDetail);
};