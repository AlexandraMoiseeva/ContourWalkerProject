#pragma once
#ifndef CONTOUR_WALKER_H
#define CONTOUR_WALKER_H

#include <list>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <string>

#include "Geometry.h"
#include "Reader.h"


class CWM
{
public:
    std::list< std::list<SpaceArea>> spaceAreas = {};
private:
    std::stringstream ss;
    std::string folder;

    std::list<ContourWalkerTool> toolFigures;
    std::list<ContourWalker> wpFigures;

    
    void writeInOut(std::ofstream& fileOutValue, SpaceArea elem, std::list<Node*>& cntrWP, std::list<Node*>& cntrTool, bool isSym = false)
    {
        fileOutValue << "Tool" + std::to_string((int)elem.detailToolId - (int)tool) + "; " 
            "Workpiece" + std::to_string((int)elem.detailWPId - (int)wp) + "; " 
            + "Square" + std::to_string((int)elem.spaceAreaId) + (isSym ? ".Sym" : "") + ";\n";

        fileOutValue << "Square: " << std::setprecision(15) << elem.spaceSquare << ";\n";

        fileOutValue << "Object type; node id;\n";

        for (auto point = elem.contourWP.begin(); point != elem.contourWP.end(); ++point)
        {
            fileOutValue << "Workpiece" + std::to_string((int)elem.detailWPId - (int)wp) + "; "
                + std::to_string((*std::next(cntrWP.begin(), *point))->id) + (isSym ? ".Sym" : "") + ";\n";
        }
        for (auto point = elem.contourTool.begin(); point != elem.contourTool.end(); ++point)
        {
            fileOutValue << "Tool" + std::to_string((int)elem.detailToolId - (int)tool) + "; "
                + std::to_string((*std::next(cntrTool.begin(), *point))->id) + (isSym ? ".Sym" : "") + ";\n";
        }
    }

public:

    CWM() {};


    CWM(std::string folderValue, unsigned time, unsigned toolNumber, unsigned wpNumber)
    {
        ss << std::setw(3) << std::setfill('0') << time;
        folder = folderValue;

        if(toolNumber > 1)
            for (unsigned i = 1; i <= toolNumber; ++i)
                toolFigures.push_back(ContourWalkerTool("../" + folder + "/" + ss.str() + "-t" + std::to_string(i) + ".csv2d", tool + i));
        else
            toolFigures.push_back(ContourWalkerTool("../" + folder + "/" + ss.str() + "-t" + ".csv2d", tool));

        if (wpNumber > 1)
            for (unsigned i = 1; i <= toolNumber; ++i)
                wpFigures.push_back(ContourWalker("../" + folder + "/" + ss.str() + "-wp" + std::to_string(i) + ".csv2d", wp + i));
        else
            wpFigures.push_back(ContourWalker("../" + folder + "/" + ss.str() + "-wp" + ".csv2d", wp));

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

        std::ofstream fileOut("../return" + folder + "/" + ss.str() + ".txt", std::ofstream::out | std::ofstream::trunc);

        for (std::list<SpaceArea> elemList : spaceAreas)
            for (SpaceArea elem : elemList)
            {
                std::list<Node*> cntrWP = std::next(wpFigures.begin(), elem.detailWPId - wp)->contour;
                std::list<Node*> cntrTool = std::next(toolFigures.begin(), elem.detailToolId - tool - 1)->contour;

                writeInOut(fileOut, elem, cntrWP, cntrTool);
            }
    }


    void trackSpaceArea(std::list< std::list<SpaceArea>>& lastSpaceAreas)
    {
        int maxId = 0;
        
        for (int i = 0; i < spaceAreas.size(); ++i)
        {
            for (std::list<SpaceArea>::iterator elem = std::next(spaceAreas.begin(), i)->begin();
                elem != std::next(spaceAreas.begin(), i)->end(); ++elem)
            {
                

                if (spaceAreas.size() == lastSpaceAreas.size())
                {
                    for (std::list<SpaceArea>::iterator elem1 = std::next(lastSpaceAreas.begin(), i)->begin();
                        elem1 != std::next(lastSpaceAreas.begin(), i)->end(); ++elem1)
                    {
                        if (elem1->spaceAreaId > maxId)
                            maxId = elem1->spaceAreaId;

                        elem->colocationSpaceArea(*elem1);
                    }
                }


                if (elem->spaceAreaId == std::numeric_limits<unsigned>::max())
                    elem->spaceAreaId = ++maxId;

            }
        }
    }


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
                std::list<Node*> cntrWP = std::next(wpFigures.begin(), elem.detailWPId - wp)->contour;
                std::list<Node*> cntrTool = std::next(toolFigures.begin(), elem.detailToolId - tool - 1)->contour;
                
                auto point0 = **elem.contourWP.begin(cntrWP);

                for (auto point = std::next(elem.contourWP.begin(cntrWP)); point != std::next(elem.contourWP.end(cntrWP)); ++point)
                {
                    Drawer().drawLine(point0, **point, window);

                    point0 = **point;
                }

                if (((*elem.contourTool.begin(cntrTool))->placeInContour - (*elem.contourTool.end(cntrTool))->placeInContour) > 0)
                {
                    Drawer().drawLine(point0, **elem.contourTool.begin(cntrTool), window);

                    point0 = **elem.contourTool.begin(cntrTool);

                    for (auto point = std::next(elem.contourTool.begin(cntrTool)); point != std::next(elem.contourTool.end(cntrTool)); ++point)
                    {
                        Drawer().drawLine(point0, **point, window);

                        point0 = **point;
                    }
                }
                Drawer().drawLine(point0, **elem.contourWP.begin(cntrWP), window);

                auto textPoint = Drawer().drawScale(point0);
                text.setPosition(textPoint.first + 50, textPoint.second + 50);
                text.setString(std::to_string(elem.spaceSquare));
                window.draw(text);

                text.setPosition(textPoint.first + 50, textPoint.second);
                text.setString(std::to_string(elem.detailWPId) + "." + std::to_string(elem.detailToolId) + '.' + std::to_string(elem.spaceAreaId));
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