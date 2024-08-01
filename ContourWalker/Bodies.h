#pragma once

#include <vector>
#include <string>

#include "BodiesSupport.h"

namespace cavity2d
{
    enum class lineDirection { none, vertical, other };


    enum class detailType { tool, workpiece, cavity };


    struct Edge
    {
        int n1 = 0;
        int n2 = 0;
    };


    struct EdgeNode
    {
        int source_body_id = -1;

        Node** first_point = nullptr;
        Node** second_point = nullptr;

        EdgeNode() = default;


        EdgeNode(Node** first_point, Node** second_point)
            : source_body_id((*first_point)->sourceObjInfo.source_body_id),
            first_point(first_point),
            second_point(second_point) {};

    };


    struct LineSymStruct
    {
        double a = std::numeric_limits<int>::max();
        double b = std::numeric_limits<int>::max();

    public:
        lineDirection linetype = lineDirection::none;

        LineSymStruct() = default;

        LineSymStruct(double xzSum, double xSum, double zSum, double x2Sum, double n);


        Node getSymNode(Node n) const;
    };


    class Body
    {
    public:
        std::vector<Node> nodes;
        Contour contour;

    public:

        detailType type = detailType::tool;
        int id = -1;

        Body() = default;


        Body(int node_amount, detailType type, int id);


        Contour& getContour();


        void addNode(Node node);


        Node& getNode(int node_id);


        void addContour(int id_node);
    };


    class Tool : public Body
    {
    public:
        Tool(int nodeAmount, detailType type, int id);

        std::vector<int> symAxisPoints;

        std::vector<std::pair<int, Edge>> contactInit;

        std::vector<EdgeNode> contact;

    public:
        LineSymStruct lineSym;


        Tool(int nodeAmount, int id);


        void addSymAxisPoint(int point);


        void addContact(std::pair<int, Edge> contact_init);


        void initialisation();

    protected:
        void symAxisInitialisation(std::vector<int>& symAxisPoints);
    };


    class CM_Cavity2D : public Body
    {
    private:
        Contour contourTool;
        Contour contourWP;

    public:
        double cavitySquare = 0;


        CM_Cavity2D(Contour& contourWP, Contour& contourTool);


        void squareCalculating();


        bool colocationCavity(CM_Cavity2D& lastCavity);

    private:
        bool isContourIntersection(Contour& tool) const;
    };


    class Workpiece : public Tool
    {
    public:
        Workpiece(int nodeAmount, int id);


        void contactInitialisation(std::vector<Tool>& details);


        EdgeNode getContactEdge(Node* node);
    };
}