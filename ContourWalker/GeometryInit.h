#pragma once
#ifndef GEOMETRY_INIT_H
#define GEOMETRY_INIT_H


#include <iomanip>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <iostream>


const enum DetailEnum { symAxis, tool, wp };


const enum lineDirectionn{ vertical, other };


class Node
{
public:
    unsigned toolType = NULL;
    bool isSym = false;

    unsigned placeInContour = std::numeric_limits<unsigned>::max();
    unsigned id = NULL;

    double x = NULL;
    double z = NULL;

    Node() {};


    Node(double xValue, double zValue)
    {
        x = xValue;
        z = zValue;
    };


    Node(int idValue, double xValue, double zValue)
    {
        id = idValue;
        x = xValue;
        z = zValue;
    };
};


class Segment
{
public:
    unsigned n1 = NULL;
    unsigned n2 = NULL;

    Segment() {};


    Segment(unsigned n1Value, unsigned n2Value)
    {
        n1 = n1Value;
        n2 = n2Value;
    };
};


class Contour
{
private:
    std::list<unsigned> contour = {};

public:
    unsigned beginNode = std::numeric_limits<unsigned>::max();
    unsigned endNode = std::numeric_limits<unsigned>::max();

    Contour() {};


    Contour(unsigned nodeValue) : beginNode(nodeValue)
    {
        contour.push_back(nodeValue);
    };


    void push_back(unsigned nodeValue)
    {
        contour.push_back(nodeValue);
        endNode = nodeValue;
    };


    void push_front(unsigned nodeValue)
    {
        contour.push_front(nodeValue);
        if (endNode == std::numeric_limits<unsigned>::max())
            endNode = beginNode;
        beginNode = nodeValue;
    };

    std::list<unsigned>::iterator begin()
    {
        return contour.begin();
    };


    std::list<unsigned>::iterator end()
    {
        return std::prev(contour.end());
    };
};


class SpaceArea
{
public:
    unsigned detailToolId = NULL;
    unsigned detailWPId = NULL;

    unsigned spaceAreaId = std::numeric_limits<unsigned>::max();

    double spaceSquare = NULL;

    Contour contourTool;
    Contour contourWP;

    SpaceArea(unsigned detailWPIdValue, unsigned detailToolIdValue, Contour contourWPValue, Contour contourToolValue)
    {
        detailToolId = detailToolIdValue;
        detailWPId = detailWPIdValue;

        contourTool = contourToolValue;
        contourWP = contourWPValue;
    };

    void intersection(std::vector<Node*>& cntrWP, std::vector<Node*>& cntrTool)
    {
        double sumSquare = 0.0f;
        Node point1, point2;

        point1 = **std::next(cntrWP.begin(), *contourWP.begin());

        for (auto elem = contourWP.begin(); elem != std::next(contourWP.end()); ++elem)
        {
            point2 = **std::next(cntrWP.begin(), *elem);

            sumSquare += 0.5 * (point1.x * point2.z - point2.x * point1.z);

            point1 = point2;
        }

        for (auto elem1 = contourTool.begin(); elem1 != std::next(contourTool.end()); ++elem1)
        {
            point2 = **std::next(cntrTool.begin(), *elem1);

            sumSquare += 0.5 * (point1.x * point2.z - point2.x * point1.z);

            point1 = point2;
        }

        point2 = **std::next(cntrWP.begin(), *contourWP.begin());

        sumSquare += 0.5 * (point1.x * point2.z - point2.x * point1.z);

        spaceSquare = abs(sumSquare);
    }

    bool colocationSpaceArea(SpaceArea &lastSpaceArea)
    {
        if (lastSpaceArea.detailToolId != detailToolId or lastSpaceArea.detailWPId != detailWPId)
            return false;

        if (isContourIntersection(lastSpaceArea.contourTool))
        {
            spaceAreaId = lastSpaceArea.spaceAreaId;
            return true;
        }

        return false;
    }

private:
    bool isContourIntersection(Contour& otherDetail) const
    {

        if (contourTool.beginNode > otherDetail.endNode and contourTool.endNode > contourTool.beginNode)
            return false;
        if (otherDetail.beginNode > contourTool.endNode and otherDetail.endNode > otherDetail.beginNode)
            return false;
        return true;
    };
};


struct LineSymStruct
{
private:
    unsigned linetype = other;
public:

    double a = std::numeric_limits<unsigned>::max();
    double b = std::numeric_limits<unsigned>::max();


    LineSymStruct() {};

    //Определение идет с помощью МНК по полученым значения
    LineSymStruct(double xzSum, double xSum, double zSum, double x2Sum, double n)
    {
        if (abs(n * x2Sum - xSum * xSum) < 1e-3)
        {
            a = xSum / n;
            linetype = vertical;
        }
        else
        {
            a = (n * xzSum - xSum * zSum) / (n * x2Sum - xSum * xSum);
            b = (zSum - a * xSum) / n;
        }
    };

    //Возвращает узел симметричный n
    Node getSymNode(Node n)
    {
        if (linetype == vertical)
            return Node(-n.x, n.z);
        
        if (abs(a) < 1e-5)
            return Node(n.x, 2 * b - n.z);

        double x0 = (n.z + (1.0f / a) * n.x - b) / (a + (1.0f / a));
        double z0 = a * x0 + b;
        return (Node(2 * x0 - n.x, 2 * z0 - n.z));
    }

};

#endif

