#pragma once

#include <vector>
#include <string>

#include "GeometryInit.h"

class DetailInit
{
protected:
    std::vector<int> symAxisPoints;
    std::vector<std::pair<int, Edge>> contactInit;
    std::vector<Node> nodes;
    std::vector<Node*> contour;

public:

    detailTypeValue detail_type;

  
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


    std::vector<Node*>::iterator begin();


    std::vector<Node*>::iterator end();


    void addNode(Node nodeValue);


    Node& getNode(int nodeId);


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


    std::vector<CM_Cavity2D> intersectionSpace(Tool& otherDetail);
};