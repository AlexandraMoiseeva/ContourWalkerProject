#pragma once

#include <vector>
#include <deque>
#include <string>

const enum class lineDirection { vertical, other };


const enum class detailType { tool, workpiece };

struct detailTypeValue
{
public:
    detailType detail_type = detailType::tool;
    int detail_id = -1;

    detailTypeValue() = default;

    detailTypeValue(detailType detail_type_value, int detail_id_value)
        : detail_type(detail_type_value),
        detail_id(detail_id_value) {};
};


class Node
{
public:
    struct SourceObjInfo {
        SourceObjInfo() = default;

        SourceObjInfo(int source_body_id, int mesh_obj_id) noexcept
            : source_body_id{ source_body_id }
            , mesh_obj_id{ mesh_obj_id } {}

        SourceObjInfo(int source_body_id, int mesh_obj_id, void* src_ptr) noexcept
            : source_body_id{ source_body_id }
            , mesh_obj_id{ mesh_obj_id }
            , ptr{ src_ptr } {}

        int source_body_id = -1;
        int mesh_obj_id = -1;
        void* ptr = nullptr;
    } sourceObjInfo;

    bool isSym = false;

    struct Coordinate
    {
        double x = 0;
        double z = 0;

        Coordinate() = default;


        Coordinate(double xValue, double zValue) : x(xValue), z(zValue) {};
    } coordinate;
    

    Node();


    Node(double xValue, double zValue);


    Node(int idValue, int detailIdValue, double xValue, double zValue);


    bool operator == (Node n);


    bool operator != (Node n);
};


class Edge
{
public:
    int n1 = 0;
    int n2 = 0;

    Edge();


    Edge(int n1Value, int n2Value);
};


class Contour
{
private:
    std::deque<Node**> contour = {};

public:
    int beginNode = std::numeric_limits<int>::max();
    int endNode = std::numeric_limits<int>::max();

    Contour();


    Contour(Node** nodeValue);


    void push_back(Node** nodeValue);


    void push_front(Node** nodeValue);


    std::deque<Node**>::iterator begin();


    std::deque<Node**>::iterator end();
};


class SpaceArea
{
public:
    int detailToolId = 0;
    int detailWPId = 0;

    int spaceAreaId = std::numeric_limits<int>::max();

    double spaceSquare = 0;

    Contour contourTool;
    Contour contourWP;

    SpaceArea();

    SpaceArea(int detailWPIdValue, int detailToolIdValue, Contour contourWPValue, Contour contourToolValue);

    void intersection(std::vector<Node*>& cntrWP, std::vector<Node*>& cntrTool);

    bool colocationSpaceArea(SpaceArea& lastSpaceArea);

private:
    bool isContourIntersection(Contour& otherDetail) const;
};


struct LineSymStruct
{
private:
    lineDirection linetype = lineDirection::other;
public:

    double a = std::numeric_limits<int>::max();
    double b = std::numeric_limits<int>::max();


    LineSymStruct();


    LineSymStruct(double xzSum, double xSum, double zSum, double x2Sum, double n);


    Node getSymNode(Node n) const;
};

