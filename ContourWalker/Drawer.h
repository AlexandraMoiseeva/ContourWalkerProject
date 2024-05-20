#pragma once
#ifndef DRAWER_H
#define DRAWER_H

#include <iomanip>
#include <list>
#include <string>

#include "GeometryInit.h"
#include <SFML/Graphics.hpp>

//Структура сопровождающая всю отрисовку
struct Drawer
{
public:
    /*
    * Возвращает координаты для экрана SFML.Задан в подобранном разрешене,
    * меняется только масштаб для детального рассмотрения
    */
    std::pair<double, double> drawScale(Node node, double scale = 5)
    {
        return std::make_pair(node.x * 1920 * scale + 50, (0.4 - node.z) * 1080 * scale);
    };


    std::pair<double, double> drawScale(std::vector<double> node, double scale = 5)
    {
        return std::make_pair(node[0] * 1920 * scale + 50, (0.4 - node[1]) * 1080 * scale);
    };

    /*
    * Отрисовывает отрезок на данных узлах
    * меняется только прозрачность. Все в белом цвете
    */
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


    void drawLine(std::vector<double> n1, std::vector<double> n2, sf::RenderWindow& window, unsigned alpha = 255)
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
};


#endif