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


void Drawer::drawContour(sf::RenderWindow& window, const DetailInit& cwt)
{
    Node const* nodepoint1 = cwt.contour.back();

    for (auto& it : cwt.contour)
    {
        Node const* nodepoint2 = it;

        drawLine(*nodepoint1, *nodepoint2, window, 100);

        if (cwt.lineSym.a != std::numeric_limits<int>::max())
            drawLine(cwt.lineSym.getSymNode(*nodepoint1), cwt.lineSym.getSymNode(*nodepoint2), window, 100);

        nodepoint1 = nodepoint2;

    }
};


void Drawer::drawSpace(sf::RenderWindow& window, CM_CavityModel2D& cm)
{
    sf::Text text;
    sf::Font font;

    if (!font.loadFromFile("ArialRegular.ttf"))
    {
        std::cerr << "�� ������� ����� �����\n";
    }

    text.setFont(font);
    text.setCharacterSize(16);
    text.setFillColor(sf::Color::Red);

    for (int i = 0; i < 2 * 1; ++i)
        for (auto& elem : cm.cavitys[i])
        {
            Node point0 = Node();
            Node point1 = Node();

            for (auto const& point : elem)
            {
                if (point0 == Node())
                {
                    point0 = **point;
                    point1 = **point;

                    continue;
                }
                Drawer().drawLine(point1, **point, window);

                point1 = **point;
            }

            Drawer().drawLine(point1, point0, window);

            auto textPoint = Drawer().drawScale(point1);
            text.setPosition(textPoint.first + 50, textPoint.second + 50);
            text.setString(std::to_string(elem.spaceSquare));
            window.draw(text);

            text.setPosition(textPoint.first + 50, textPoint.second);
            text.setString(std::to_string(elem.detailWPId) + "." + std::to_string(elem.detailToolId) + '.' + std::to_string(elem.cavityId));
            window.draw(text);
        }
};


void Drawer::drawAll(sf::RenderWindow& window, CM_CavityModel2D& cm)
{
    for (const auto& elem : cm.getToolFigures())
        drawContour(window, elem);
    for (const auto& elem : cm.getWpFigures())
        drawContour(window, elem);

    drawSpace(window, cm);
}