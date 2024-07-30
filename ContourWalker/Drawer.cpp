#include <iostream>
#include <string>

#include "Drawer.h"


std::pair<double, double> Drawer::drawScale(Node node, double scale)
{
    return std::make_pair(node.coordinate.x * 1920 * scale + 50,
        (0.4 - node.coordinate.z) * 1080 * scale);
};


void Drawer::drawLine(Node n1, Node n2, sf::RenderWindow& window, int alpha)
{
    auto vectorpoint1result = drawScale(n1);
    auto vectorpoint2result = drawScale(n2);

    auto vectorpoint1 = sf::Vector2f(vectorpoint1result.first,
        vectorpoint1result.second);
    auto vectorpoint2 = sf::Vector2f(vectorpoint2result.first,
        vectorpoint2result.second);

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
    for (auto point = cwt.getContour().begin() + 1; 
        point != cwt.getContour().end(); ++point)
    {
        drawLine(**point, **(point - 1), window, 100);

        if (cwt.lineSym.linetype != lineDirection::none)
            drawLine(cwt.lineSym.getSymNode(**point),
                cwt.lineSym.getSymNode(**(point - 1)), window, 100);
    }
};


void Drawer::drawCavity(sf::RenderWindow& window, CM_CavityModel2D& cm)
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

    for (auto& elem : cm.getCavities())
    {
        for (auto point = elem.getContour().begin() + 1;
            point != elem.getContour().end(); ++point)
        {
            drawLine(**point, **(point - 1), window);
        }

        auto textPoint = Drawer().drawScale(*elem.getContour().front());
        text.setPosition(textPoint.first + 50, textPoint.second + 50);
        text.setString(std::to_string(elem.cavitySquare));
        window.draw(text);

        text.setPosition(textPoint.first + 50, textPoint.second);
        text.setString(std::to_string(elem.getContour().front()->sourceObjInfo.source_body_id) + "."
            + std::to_string(elem.id));
        window.draw(text);
    }
};


void Drawer::drawAll(sf::RenderWindow& window, CM_CavityModel2D& cm)
{
    for (auto& elem : cm.getTools())
        drawContour(window, elem);
    for (auto& elem : cm.getWorkpieces())
        drawContour(window, elem);

    drawCavity(window, cm);
}