#pragma once
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <sstream>
#include <fstream>
#include <string>

#include "Reader.h"
#include "GeometryInit.h"
#include "Drawer.h"

#include <SFML/Graphics.hpp>

const enum symAxisStateValue { noneStateAxis, beginNodeAxis, bothNodeebetweenFigures };

/*
* Класс, отвечающий за всю работу с одной из фигур Tool
* Хранит:
*   detailTypeNum - тип фигуры
*   nodes - вектор, хранящий узлы в порядке считыввания
*   contour - вектор, хранящий указатели на узлы в порядке, образующем контур
*   symAxisPoints - хранит номера узлом, лежащих на оси симметрии
*   LineSym - ось симметрии, структуры LineSymStruct
*   connect - хранит словарь, размера количества узлов, где в соответсвие номеру узла
              ставится пара - номера узлов отрезка, которого он касается.
              В случае Tool1 и Tool2 - касание с wp, в случае с wp - касание с Tool1.
              Если узел не касается, то в соответсвие ставится пара (-1, -1)
*   connect1 - хранит словарь, размера количества узлов, где в соответсвие номеру узла
              ставится пара - номера узлов отрезка, которого он касается.
              В случае Tool1 и Tool2 - этот вектор пуст, в случае с wp - касание с Tool2.
              Если узел не касается, то в соответсвие ставится пара (-1, -1)
*/
class ContourWalkerTool
{
public:
    unsigned detailTypeNum = tool;
    std::list< Node > nodes = {};
    std::list< Node* > contour = {};

    std::list< unsigned > symAxisPoints = {};

    std::list< std::pair<unsigned, Segment> > connect = {};

    LineSymStruct lineSym;

    ContourWalkerTool() {};


    ContourWalkerTool(std::string filePathValue, unsigned detailTypeValue = tool)
    {
        Reader reader(filePathValue, detailTypeValue);

        detailTypeNum = detailTypeValue;

        nodes = reader.nodes;
        for (auto elem : reader.contour)
            contour.push_back(&*std::next(nodes.begin(), elem));

        symAxisPoints = reader.symAxisPoints;

        connect = reader.connect;
    };

    //Отрисовка контура
    void draw(sf::RenderWindow& window)
    {
        for (auto it = contour.begin(); it != contour.end(); ++it)
        {
            Node nodepoint1, nodepoint2;

            if (it == contour.begin())
            {
                nodepoint1 = **std::next(contour.end(), -1);
                nodepoint2 = **contour.begin();
            }
            else
            {
                nodepoint1 = **it;
                nodepoint2 = **std::next(it, -1);
            }

            Drawer().drawLine(nodepoint1, nodepoint2, window, 100);

            if (symAxisPoints.size() == 0)
                continue;

            Drawer().drawLine(lineSym.getSymNode(nodepoint1), lineSym.getSymNode(nodepoint2), window, 100);
        }
        return;
    };

    //Определение оси симметрии
    void symAxisInizialisation()
    {
        if (symAxisPoints.size() == 0)
            return;

        double xzSum = 0, xSum = 0, zSum = 0, x2Sum = 0, n = 0;

        for (int idElem : symAxisPoints)
        {
            Node elem = *std::next(nodes.begin(), idElem);
            xzSum += elem.x * elem.z;
            xSum += elem.x;
            zSum += elem.z;
            x2Sum += elem.x * elem.x;
            ++n;
        }

        lineSym = LineSymStruct(xzSum, xSum, zSum, x2Sum, n);
    };

};

/*
* Класс, отвечающий за всю работу с одной из фигур Tool
* Хранит:
*   detailTypeNum - тип фигуры
*   nodes - вектор, хранящий узлы в порядке считыввания
*   contour - вектор, хранящий указатели на узлы в порядке, образующем контур
*   symAxisPoints - хранит номера узлом, лежащих на оси симметрии
*   LineSym - ось симметрии, структуры LineSymStruct
*   connect - хранит словарь, размера количества узлов, где в соответсвие номеру узла
              ставится пара - номера узлов отрезка, которого он касается.
              Если узел не касается, то в соответсвие ставится пара (-1, -1)
*   connect1 - хранит словарь, размера количества узлов, где в соответсвие номеру узла
              ставится пара - номера узлов отрезка, которого он касается.
              Если узел не касается, то в соответсвие ставится пара (-1, -1)
*   connectSpace - хранит вектор пустых областей
                   - элемент - пара:
                                  1) Список вершин, рассматриваемой фигуры
                                  2) Пара - отрезок, которого касается первый и последний узел
*   connectSpace - хранит вектор пустых областей
                   - элемент - пара:
                                  1) Список вершин, рассматриваемой фигуры
                                  2) Пара - отрезок, которого касается первый и последний узел
*/
class ContourWalker : public ContourWalkerTool
{
public:

    std::list<SpaceArea> spaceAreas = {};

    ContourWalker() : ContourWalkerTool() {}


    ContourWalker(std::string filePathValue, int detailTypeValue = tool) : ContourWalkerTool(filePathValue, detailTypeValue) {};

    //Определние соответсвующих connectSpace - нахождение контуров из некасающихся узлов и соответсвующих отрезков
    void intersectionSpace(ContourWalkerTool& otherDetail)
    {
        unsigned detailTypeValue = otherDetail.detailTypeNum;
        bool StartSpaceContour = false;
        int spaceAreaCount = 0;
        unsigned symAxisState = noneStateAxis;

        for (auto it = contour.begin(); it != contour.end(); ++it)
        {
            if (std::next(connect.begin(), (*it)->id)->first != detailTypeValue and
                std::find(symAxisPoints.begin(), symAxisPoints.end(), (*it)->id) == symAxisPoints.end())
            {
                if (StartSpaceContour == false)
                {
                    ++spaceAreaCount;
                    if (std::next(connect.begin(), (*std::next(it, -1))->id)->first == detailTypeValue)
                    {
                        spaceAreas.push_back(
                            SpaceArea(
                                detailTypeNum,
                                detailTypeValue,
                                Contour(std::next(it, -1)),
                                Contour(std::next(otherDetail.contour.begin(),
                                    std::next(otherDetail.nodes.begin(),
                                        (std::next(connect.begin(), (*std::prev(it))->id))->second.n2
                                    )->placeInContour
                                )
                                )
                            ));

                        symAxisState = bothNodeebetweenFigures;
                    }
                    else
                    {
                        spaceAreas.push_back(
                            SpaceArea(
                                detailTypeNum,
                                detailTypeValue,
                                Contour(std::next(it, -1)),
                                Contour(std::next(otherDetail.contour.begin(),
                                    std::next(otherDetail.nodes.begin(),
                                        *otherDetail.symAxisPoints.begin()
                                    )->placeInContour
                                )
                                )
                            ));

                        symAxisState = beginNodeAxis;
                    }

                    StartSpaceContour = true;
                }
                else
                {
                    std::prev(spaceAreas.end())->contourWP.endNode = std::next(it, -1);
                }
            }
            if (std::next(connect.begin(), (*it)->id)->first == detailTypeValue or
                std::find(symAxisPoints.begin(), symAxisPoints.end(), (*it)->id) != symAxisPoints.end())
            {
                if (StartSpaceContour == true)
                {
                    StartSpaceContour = false;

                    (std::prev(spaceAreas.end()))->contourWP.endNode = it;

                    if (std::next(connect.begin(), (*it)->id)->first == detailTypeValue)
                    {
                        (std::prev(spaceAreas.end()))->contourTool.endNode = std::next(otherDetail.contour.begin(),
                            std::next(otherDetail.nodes.begin(),
                                (std::next(connect.begin(), (*it)->id))->second.n1
                            )->placeInContour
                        );

                        if (symAxisState == beginNodeAxis)
                        {
                            std::list<Node*>::iterator it1 = (std::prev(spaceAreas.end()))->contourTool.endNode;

                            while (true)
                            {
                                if (std::find(otherDetail.symAxisPoints.begin(), otherDetail.symAxisPoints.end(),
                                    (*it1)->id) == otherDetail.symAxisPoints.end())
                                    ++it1;
                                else
                                {
                                    (std::prev(spaceAreas.end()))->contourTool.beginNode = it1;
                                    break;
                                }
                            }
                        }
                    }
                    else
                    {
                        
                        if (symAxisState == bothNodeebetweenFigures)
                        {
                            std::list<Node*>::iterator it1 = (std::prev(spaceAreas.end()))->contourTool.beginNode;

                            while (true)
                            {
                                if (std::find(otherDetail.symAxisPoints.begin(), otherDetail.symAxisPoints.end(),
                                    (*it1)->id) == otherDetail.symAxisPoints.end())
                                    --it1;
                                else
                                {
                                    (std::prev(spaceAreas.end()))->contourTool.endNode = it1;
                                    break;
                                }
                            }
                        }

                        if (symAxisState == beginNodeAxis)
                        {
                            spaceAreas.pop_back();
                        }
                    }
                    symAxisState = noneStateAxis;

                }
            }
        }     
        
        std::list<SpaceArea>::iterator maxSquareIterator = std::prev(spaceAreas.end());
        double maxSquare = 0;
        for (std::list<SpaceArea>::iterator elem = std::next(spaceAreas.end(), -spaceAreaCount); elem != spaceAreas.end(); ++elem)
        {
            elem->intersection();

            if (elem->spaceSquare > maxSquare)
            {
                maxSquare = elem->spaceSquare;
                maxSquareIterator = elem;
            }
        }

        spaceAreas.erase(maxSquareIterator);

    }

};

#endif