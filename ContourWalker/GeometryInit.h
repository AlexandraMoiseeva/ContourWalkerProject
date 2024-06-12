#pragma once

#include <vector>
#include <string>

const enum class lineDirection { vertical, other };


class Node
{
public:
public:
    unsigned toolType = 0;
    bool isSym = false;

    unsigned placeInContour = std::numeric_limits<unsigned>::max();
    unsigned id = 0;

    double x = 0;
    double z = 0;

    Node();


    Node(double xValue, double zValue);


    Node(int idValue, double xValue, double zValue);
};


class Segment
{
public:
    unsigned n1 = 0;
    unsigned n2 = 0;

    Segment();


    Segment(unsigned n1Value, unsigned n2Value);
};


class Contour
{
private:
    std::list<unsigned> contour = {};

public:
    unsigned beginNode = std::numeric_limits<unsigned>::max();
    unsigned endNode = std::numeric_limits<unsigned>::max();

    Contour();


    Contour(unsigned nodeValue);


    void push_back(unsigned nodeValue);


    void push_front(unsigned nodeValue);


    std::list<unsigned>::iterator begin();


    std::list<unsigned>::iterator end();
};


class SpaceArea
{
public:
    unsigned detailToolId = 0;
    unsigned detailWPId = 0;

    unsigned spaceAreaId = std::numeric_limits<unsigned>::max();

    double spaceSquare = 0;

    Contour contourTool;
    Contour contourWP;

    SpaceArea();

    SpaceArea(unsigned detailWPIdValue, unsigned detailToolIdValue, Contour contourWPValue, Contour contourToolValue);

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

    double a = std::numeric_limits<unsigned>::max();
    double b = std::numeric_limits<unsigned>::max();


    LineSymStruct();


    LineSymStruct(double xzSum, double xSum, double zSum, double x2Sum, double n);


    Node getSymNode(Node n);
};

