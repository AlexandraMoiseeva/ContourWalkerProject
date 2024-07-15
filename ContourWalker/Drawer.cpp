#include <string>
#include <iostream>

#include "Drawer.h"


std::pair<double, double> Drawer::drawScale(Node node, double scale)
{
    return std::make_pair(node.coordinate.x * 1920 * scale + 50, (0.4 - node.coordinate.z) * 1080 * scale);
};


void Drawer::drawLine(Node n1, Node n2, sf::RenderWindow& window, int alpha)
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


void Drawer::drawContour(sf::RenderWindow& window, Tool& cwt)
{
    auto nodepoint1 = Node();

    for (const auto& it : cwt)
    {
        if (nodepoint1 == Node())
            nodepoint1 = *it;
        auto nodepoint2 = *it;

        drawLine(nodepoint1, nodepoint2, window, 100);

        if (cwt.lineSym.linetype != lineDirection::none)
            drawLine(cwt.lineSym.getSymNode(nodepoint1), cwt.lineSym.getSymNode(nodepoint2), window, 100);

        nodepoint1 = nodepoint2;
    }
};


void Drawer::drawSpace(sf::RenderWindow& window, CM_CavityModel2D& cm) const
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

    for (auto const& elem : cm.cavitys)
    {
        auto point0 = Node();
        auto point1 = Node();

        for (auto const& point : elem)
        {
            if (point0 == Node())
            {
                point0 = *point;
                point1 = *point;

                continue;
            }
            Drawer().drawLine(point1, *point, window);

            point1 = *point;
        }

        Drawer().drawLine(point1, point0, window);

        auto textPoint = Drawer().drawScale(point1);
        text.setPosition(textPoint.first + 50, textPoint.second + 50);
        text.setString(std::to_string(elem.spaceSquare));
        window.draw(text);

        text.setPosition(textPoint.first + 50, textPoint.second);
        text.setString(std::to_string(point0.sourceObjInfo.source_body_id) + "."
            + std::to_string(elem.id));
        window.draw(text);
    }
};


void Drawer::drawAll(sf::RenderWindow& window, CM_CavityModel2D& cm)
{
    for (auto& elem : cm.gettools())
        drawContour(window, elem);
    for (auto& elem : cm.getworkpieces())
        drawContour(window, elem);

    drawSpace(window, cm);
}