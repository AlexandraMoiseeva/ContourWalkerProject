#pragma once
#ifndef DRAWER_H
#define DRAWER_H

#include <iomanip>
#include <list>
#include <string>

#include "Geometry.h"
#include <SFML/Graphics.hpp>

struct Drawer
{
public:

    std::pair<double, double> drawScale(Node node, double scale = 5)
    {
        return std::make_pair(node.x * 1920 * scale + 50, (0.4 - node.z) * 1080 * scale);
    };


    void drawLine(Node n1, Node n2, sf::RenderWindow& window, unsigned alpha = 255)
    {
        auto vectorpoint1result = drawScale(n1);
        auto vectorpoint2result = drawScale(n2);

        auto vectorpoint1 = sf::Vector2f(vectorpoint1result.first, vectorpoint1result.second);
        auto vectorpoint2 = sf::Vector2f(vectorpoint2result.first, vectorpoint2result.second);

        sf::Vertex line[] =
        {
            sf::Vertex(vectorpoint1),
            sf::Vertex(vectorpoint2)
        };

        line[0].color = sf::Color(255, 255, 255, alpha);
        line[1].color = sf::Color(255, 255, 255, alpha);

        window.draw(line, 2, sf::Lines);

        return;
    }


    void drawContour(sf::RenderWindow& window, ContourWalkerTool& cwt)
    {
        for (auto it = cwt.contour.begin(); it != cwt.contour.end(); ++it)
        {
            Node nodepoint1, nodepoint2;

            if (it == cwt.contour.begin())
            {
                nodepoint1 = **std::next(cwt.contour.end(), -1);
                nodepoint2 = **cwt.contour.begin();
            }
            else
            {
                nodepoint1 = **it;
                nodepoint2 = **std::next(it, -1);
            }

            drawLine(nodepoint1, nodepoint2, window, 100);

            if (cwt.lineSym.a == std::numeric_limits<unsigned>::max())
                continue;

            drawLine(cwt.lineSym.getSymNode(nodepoint1), cwt.lineSym.getSymNode(nodepoint2), window, 100);
        }
        return;
    };


    void drawContour(sf::RenderWindow& window, ContourWalker& cwt)
    {
        for (auto it = cwt.contour.begin(); it != cwt.contour.end(); ++it)
        {
            Node nodepoint1, nodepoint2;

            if (it == cwt.contour.begin())
            {
                nodepoint1 = **std::next(cwt.contour.end(), -1);
                nodepoint2 = **cwt.contour.begin();
            }
            else
            {
                nodepoint1 = **it;
                nodepoint2 = **std::next(it, -1);
            }

            drawLine(nodepoint1, nodepoint2, window, 100);

            if (cwt.lineSym.a == std::numeric_limits<unsigned>::max())
                continue;

            drawLine(cwt.lineSym.getSymNode(nodepoint1), cwt.lineSym.getSymNode(nodepoint2), window, 100);
        }
        return;
    };


    void drawSpace(sf::RenderWindow& window, std::list<ContourWalkerTool>& toolFigures, std::list<ContourWalker>& wpFigures, std::list< std::list<SpaceArea>>& spaceAreas)
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
            for (auto elem : elemList)
            {

                std::vector<Node*> cntrWP = std::next(wpFigures.begin(), elem.detailWPId - wp)->contour;
                std::vector<Node*> cntrTool = std::next(toolFigures.begin(), elem.detailToolId - tool - 1)->contour;

                auto point0 = **std::next(cntrWP.begin(), *elem.contourWP.begin());

                for (auto point = std::next(elem.contourWP.begin()); point != std::next(elem.contourWP.end()); ++point)
                {
                    Drawer().drawLine(point0, **std::next(cntrWP.begin(), *point), window);

                    point0 = **std::next(cntrWP.begin(), *point);
                }

                Drawer().drawLine(point0, **std::next(cntrTool.begin(), *elem.contourTool.begin()), window);

                point0 = **std::next(cntrTool.begin(), *elem.contourTool.begin());

                for (auto point = std::next(elem.contourTool.begin()); point != std::next(elem.contourTool.end()); ++point)
                {
                    Drawer().drawLine(point0, **std::next(cntrTool.begin(), *point), window);

                    point0 = **std::next(cntrTool.begin(), *point);
                }

                Drawer().drawLine(point0, **std::next(cntrWP.begin(), *elem.contourWP.begin()), window);

                auto textPoint = Drawer().drawScale(point0);
                text.setPosition(textPoint.first + 50, textPoint.second + 50);
                text.setString(std::to_string(elem.spaceSquare));
                window.draw(text);

                text.setPosition(textPoint.first + 50, textPoint.second);
                text.setString(std::to_string(elem.detailWPId) + "." + std::to_string(elem.detailToolId) + '.' + std::to_string(elem.spaceAreaId));
                window.draw(text);
            }

    }


    void drawAll(sf::RenderWindow& window, std::list<ContourWalkerTool>& toolFigures, std::list<ContourWalker>& wpFigures, std::list< std::list<SpaceArea>>& spaceAreas)
    {
        for (auto elem : toolFigures)
            drawContour(window, elem);
        for (auto elem : wpFigures)
            drawContour(window, elem);

        drawSpace(window, toolFigures, wpFigures, spaceAreas);
    }
};


#endif