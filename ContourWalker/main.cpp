#include <iostream>
#include <filesystem>

#include <SFML/Graphics.hpp>

#include "CM_CavityModel2D.h" 
#include "Drawer.h"
#include "Writer.h"
#include "some_tests.h"

int main()
{
    tests::SomeTests();

    sf::RenderWindow window(sf::VideoMode(1440, 810), "SFML works!");
    sf::View view(sf::FloatRect(0, 0, 960 * 1.5, 540 * 1.5));

    window.setView(view);
    
    int time = 1;

    std::string folder = "data_example/data(6)";

    if (std::filesystem::exists("../return_" + folder))
    {
        std::filesystem::remove_all("../return_" + folder);
    }

    std::filesystem::create_directory("../return_" + folder);

    std::vector<CM_Cavity2D> lastCavity;

    int toolNumber = 2;
    int wpNumber = 1;

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseWheelScrolled &&
                event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
            {

                if (event.mouseWheelScroll.delta > 0)
                    view.zoom(0.93f);
                else
                    view.zoom(1.03f);
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

        std::stringstream ss;

        ss << std::setw(3) << std::setfill('0') << time;

        CM_CavityModel2D_FromFile cmObject(folder + "/" +
            ss.str(), toolNumber, wpNumber);
         
        cmObject.initialisation(toolNumber, wpNumber);

        cmObject.findCavity();
        
        cmObject.trackCavity(lastCavity);
        
        Writer().write(folder + "/" + ss.str(), cmObject);

        Drawer().drawAll(window, cmObject);

        lastCavity = std::move(cmObject.getCavities());

        sf::sleep(sf::milliseconds(300*1));

        window.display();

        if (time++ == 90)
            time = 1;
    }
    return 0;
}