#pragma once
#ifndef CONTOUR_WALKER_H
#define CONTOUR_WALKER_H

#include <list>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>

#include "Geometry.h"
#include "Reader.h"

/*
* Класс, отвечающий за решение задачи в общем
* Хранит:
*   ss - номер итерации в нужном формате
*   firstFigure - Tool1, в классе ContourWalker
*   secondFigure - Tool2, в классе ContourWalker
*   wpFigure - WP, в классе ContourWalker
*   spaceAreaTool1 - хранит пустые области Wp с Tool1, их площадь, их номер
*   spaceAreaTool2 - хранит пустые области Wp с Tool2, их площадь, их номер
*/
class CWM
{
public:
    std::list< std::list<SpaceArea>> spaceAreas;
    std::list< std::list<SpaceArea>> lastSpaceAreas;
private:
    std::stringstream ss;
    std::list<ContourWalkerTool> toolFigures;
    std::list<ContourWalker> wpFigures;

    
    void writeInOut(std::ofstream& fileOutValue, SpaceArea elem, bool isSym = false)
    {
        fileOutValue << "Tool" + std::to_string(elem.detailToolId) + "; " + "Square" + std::to_string(elem.spaceAreaId) + (isSym ? ".Sym" : "") + "\n";
        fileOutValue << "Square: " << std::setprecision(15) << elem.spaceSquare << "\n";
        fileOutValue << "Object type; node id;\n";
        for (auto point = elem.contourWP.beginNode; point != std::next(elem.contourWP.endNode); ++point)
        {
            fileOutValue << "; " + std::to_string(point->id) + (isSym ? ".Sym" : "") + "\n";
        }
        for (auto point = elem.contourTool.beginNode; point != std::next(elem.contourTool.endNode); ++point)
        {
            fileOutValue << "; " + std::to_string(point->id) + (isSym ? ".Sym" : "") + "\n";
        }
    }

public:

    CWM(unsigned time, unsigned toolNumber, unsigned wpNumber)
    {
        ss << std::setw(3) << std::setfill('0') << time;

        if(toolNumber > 1)
            for (unsigned i = 1; i <= toolNumber; ++i)
                toolFigures.push_back(ContourWalkerTool("../data(1)/" + ss.str() + "-t" + std::to_string(i) + ".csv2d", tool + i));
        else
            toolFigures.push_back(ContourWalkerTool("../data(1)/" + ss.str() + "-t" + ".csv2d", tool));

        if (wpNumber > 1)
            for (unsigned i = 1; i <= toolNumber; ++i)
                wpFigures.push_back(ContourWalker("../data(1)/" + ss.str() + "-wp" + std::to_string(i) + ".csv2d", wp + i));
        else
            wpFigures.push_back(ContourWalker("../data(1)/" + ss.str() + "-wp" + ".csv2d", wp));

    };


    CWM(unsigned time, unsigned toolNumber, unsigned wpNumber, std::list< std::list<SpaceArea>> spaceAreasValue) : CWM(time, toolNumber, wpNumber)
    {
        lastSpaceAreas = spaceAreasValue;
    };
    
    //Нахождние областей и запись в папку returnData
    void findSpace()
    {
        for(auto elem = toolFigures.begin(); elem != toolFigures.end(); ++elem)
            elem->symAxisInizialisation();
        for (auto elem = wpFigures.begin(); elem != wpFigures.end(); ++elem)
            elem->symAxisInizialisation();

        for (auto wpElem = wpFigures.begin();  wpElem != wpFigures.end(); ++wpElem)
        {
            for (auto toolElem = toolFigures.begin();  toolElem != toolFigures.end(); ++toolElem)
            {
                wpElem->intersectionSpace(*toolElem);
            }

            spaceAreas.push_back(wpElem->spaceAreas);
        }

        std::ofstream fileOut("../returnData/" + ss.str() + ".txt", std::ofstream::out | std::ofstream::trunc);
        
    }

    //Отрисовка пустых областей

    void drawSpace(sf::RenderWindow& window)
    {
        sf::Text text;
        sf::Font font;

        if (!font.loadFromFile("ArialRegular.ttf"))
        {
            std::cerr << "Не удалось найти шрифт\n";
        }

        text.setFont(font);
        text.setCharacterSize(16);
        text.setFillColor(sf::Color::Red);

        for (auto elemList : spaceAreas)
            for (auto elem: elemList)
            {
                auto point0 = *elem.contourWP.beginNode;

                for (auto point = std::next(elem.contourWP.beginNode); point != elem.contourWP.endNode; point++)
                {
                    Drawer().drawLine(point0, *point, window);

                    point0 = *point;
                }

                if (elem.contourTool.endNode - elem.contourTool.beginNode > 0)
                {
                    Drawer().drawLine(point0, *elem.contourTool.beginNode, window);

                    point0 = *elem.contourTool.beginNode;

                    for (auto point = std::next(elem.contourTool.beginNode); point != elem.contourTool.endNode; point++)
                    {
                        Drawer().drawLine(point0, *point, window);

                        point0 = *point;
                    }
                }

                Drawer().drawLine(point0, *elem.contourWP.endNode, window);

                auto textPoint = Drawer().drawScale(point0);
                text.setPosition(textPoint.first + 50, textPoint.second + 50);
                text.setString(std::to_string(elem.spaceSquare));
                window.draw(text);

                text.setPosition(textPoint.first + 50, textPoint.second);
                text.setString(std::to_string(elem.detailToolId) + '.' + std::to_string(elem.spaceAreaId));
                window.draw(text);
            }

    }

    //Отрисовка областей и контуров фигур
    void drawAll(sf::RenderWindow& window)
    {
        for (auto elem : toolFigures)
            elem.draw(window);
        for (auto elem : wpFigures)
            elem.draw(window);
        
        drawSpace(window);
    }
};

#endif