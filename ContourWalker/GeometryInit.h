#pragma once
#ifndef GEOMETRY_INIT_H
#define GEOMETRY_INIT_H


#include <iomanip>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <iostream>


//Перечисление типов объектов на основе тех, что в #contacts
const enum DetailEnum { symAxis, tool, wp };

//Типы прямых для оси симметрии
const enum lineDirectionn{ vertical, other };


/*
* Класс для определения узла
* Хранит:
*   placeInContour - место в векторе, определяющем контур
*   id - место в векторе, хранящем все вершины
*   x, z - соответсвующие координаты
*/
class Node
{
public:
    unsigned toolType = symAxis;
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

/*
* Класс для определения отрезка
* Хранит:
*   *n1, *n2 - указатели на узлы, характеризующие отрезок
*/
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
public:
    std::list<Node*>::iterator beginNode;
    std::list<Node*>::iterator endNode;

    Contour() {};


    Contour(std::list<Node*>::iterator beginNodeValue, std::list<Node*>::iterator endNodeValue) : 
        beginNode(beginNodeValue), endNode(endNodeValue) {};

    Contour(std::list<Node*>::iterator nodeValue) : beginNode(nodeValue), endNode(nodeValue) {};

};


class SpaceArea
{
public:
    unsigned detailToolId = NULL;
    unsigned detailWPId = NULL;

    unsigned spaceAreaId = std::numeric_limits<unsigned>::max();

    double spaceSquare = 0;

    Contour contourTool;
    Contour contourWP;

    SpaceArea(unsigned detailWPIdValue, unsigned detailToolIdValue, Contour contourWPValue, Contour contourToolValue)
    {
        detailToolId = detailToolIdValue;
        detailWPId = detailWPIdValue;

        contourTool = contourToolValue;
        contourWP = contourWPValue;
    };

    //Построение контуров пустых областей и нахождение их площадей - формула площади Гаусса
    void intersection()
    {
        double sumSquare = 0.0;
        Node point1, point2;

        point1 = **contourWP.beginNode;

        for (auto elem = contourWP.beginNode; elem != std::next(contourWP.endNode); ++elem)
        {
            point2 = **elem;

            sumSquare += 0.5 * (point1.x * point2.z - point2.x * point1.z);

            point1 = point2;
        }

        for (auto elem1 = contourTool.endNode; elem1 != std::next(contourTool.beginNode); ++elem1)
        {
            point2 = **elem1;

            sumSquare += 0.5 * (point1.x * point2.z - point2.x * point1.z);

            point1 = point2;
        }

        point2 = **contourWP.beginNode;

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
    bool isContourIntersection(Contour &otherDetail) const
    {
        if ((*contourTool.beginNode)->placeInContour < (*otherDetail.endNode)->placeInContour)
            return false;
        if ((*otherDetail.beginNode)->placeInContour < (*contourTool.endNode)->placeInContour)
            return false;
        return true;
    };
};

/*
* Структура для определения оси симметрии
* Хранит:
*   a, b - переменные характеризующие прямую
*   тип прямой - вертикальная прямая - x = a
*              - любая другая прямая - y = a * x + b
*/
struct LineSymStruct
{
private:
public:
    unsigned linetype = other;

    double a = NULL;
    double b = NULL;


    LineSymStruct() {};

    //Определение идет с помощью МНК по полученым значения
    LineSymStruct(double xzSum, double xSum, double zSum, double x2Sum, double n)
    {
        if (abs(n * x2Sum - xSum * xSum) < 1e-5)
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
            return Node(NULL, -n.x, n.z);
        
        if (abs(a) < 1e-5)
            return Node(NULL, n.x, 2 * b - n.z);

        double x0 = (n.z + (1.0f / a) * n.x - b) / (a + (1.0f / a));
        double z0 = a * x0 + b;
        return (Node(NULL, 2 * x0 - n.x, 2 * z0 - n.z));
    }

};

#endif

