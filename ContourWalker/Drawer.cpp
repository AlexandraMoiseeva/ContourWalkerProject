#include <list>
#include <string>
#include <iostream>

#include "Drawer.h"


std::pair<double, double> Drawer::drawScale(Node node, double scale)
{
    return std::make_pair(node.x * 1920 * scale + 50, (0.4 - node.z) * 1080 * scale);
};


void Drawer::drawLine(Node n1, Node n2, sf::RenderWindow& window, unsigned alpha)
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


void Drawer::drawContour(sf::RenderWindow& window, ContourWalkerTool& cwt)
{
    Node* nodepoint1 = cwt.contour.back();

    for (auto& it : cwt.contour)
    {
        Node* nodepoint2 = it;

        drawLine(*nodepoint1, *nodepoint2, window, 100);

        if (cwt.lineSym.a == std::numeric_limits<unsigned>::max())
            continue;

        drawLine(cwt.lineSym.getSymNode(*nodepoint1), cwt.lineSym.getSymNode(*nodepoint2), window, 100);

        nodepoint1 = nodepoint2;
    }
};


void Drawer::drawContour(sf::RenderWindow& window, ContourWalker& cwt)
{
    Node* nodepoint1 = cwt.contour.back();

    for (auto& it : cwt.contour)
    {
        Node* nodepoint2 = it;

        drawLine(*nodepoint1, *nodepoint2, window, 100);

        if (cwt.lineSym.a == std::numeric_limits<unsigned>::max())
            continue;

        drawLine(cwt.lineSym.getSymNode(*nodepoint1), cwt.lineSym.getSymNode(*nodepoint2), window, 100);

        nodepoint1 = nodepoint2;
    }
};


void Drawer::drawSpace(sf::RenderWindow& window, CWMDrawerReader& cwm)
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

    for (int i = 0; i < 2 * 1; ++i)
        for (auto& elem : cwm.spaceAreas[i])
        {

            std::vector<Node*> cntrWP = std::next(cwm.wpFigures.begin(), elem.detailWPId + (elem.detailWPId == 0 ? 0 : -1))->contour;
            std::vector<Node*> cntrTool = std::next(cwm.toolFigures.begin(), elem.detailToolId + (elem.detailToolId == 0 ? 0 : -1))->contour;

            auto point0 = **std::next(cntrWP.begin(), *elem.contourWP.begin());

            for (auto&& point = std::next(elem.contourWP.begin()); point != elem.contourWP.end(); ++point)
            {
                Drawer().drawLine(point0, **std::next(cntrWP.begin(), *point), window);

                point0 = **std::next(cntrWP.begin(), *point);
            }

            Drawer().drawLine(point0, **std::next(cntrTool.begin(), *elem.contourTool.begin()), window);

            point0 = **std::next(cntrTool.begin(), *elem.contourTool.begin());

            for (auto&& point = std::next(elem.contourTool.begin()); point != elem.contourTool.end(); ++point)
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


void Drawer::drawAll(sf::RenderWindow& window, CWMDrawerReader& cwm)
{
    for (auto& elem : cwm.toolFigures)
        drawContour(window, elem);
    for (auto& elem : cwm.wpFigures)
        drawContour(window, elem);

    drawSpace(window, cwm);
}