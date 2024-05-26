#include <iostream>

#include <SFML/Graphics.hpp>

#include "ContourWalker.h" 

int main()
{
    sf::RenderWindow window(sf::VideoMode(960 * 1.5, 540 * 1.5), "SFML works!");
    sf::View view(sf::FloatRect(0, 0, 960 * 1.5, 540 * 1.5));

    window.setView(view);
    
    unsigned time = 1;

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
            {

                if (event.mouseWheelScroll.delta > 0)
                    view.zoom(0.93);
                else
                    view.zoom(1.03);
                window.setView(view);
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                {
                    view.move(-50.0f, 0.0f);
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                {
                    view.move(50.0f, 0.0f);
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                {
                    view.move(0.0f, -50.0f);
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                {
                    view.move(0.0f, 50.0f);
                }

                window.setView(view);
            }
        }

        window.clear();

        CWM cwmObject(time, 2, 1);

        cwmObject.findSpace();

        cwmObject.drawAll(window);

        sf::sleep(sf::milliseconds(300*1));

        window.display();

        if (time++ == 113)
            time = 1;
    }

    return 0;
}