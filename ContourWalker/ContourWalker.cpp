#include <iostream>

#include <SFML/Graphics.hpp>

#include "ContourWalker.h" 
#include "Drawer.h"
#include "Reader.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(960 * 1.5, 540 * 1.5), "SFML works!");
    sf::View view(sf::FloatRect(0, 0, 960 * 1.5, 540 * 1.5));

    window.setView(view);
    
    unsigned time = 1;

    std::string folder = "data(9)";

    if (std::filesystem::exists("../return" + folder))
    {
        std::filesystem::remove_all("../return" + folder);
    }

    std::filesystem::create_directory("../return" + folder);

    std::list< std::list<SpaceArea>> lastSpaceAreas = {};

    unsigned toolNumber = 2;
    unsigned wpNumber = 1;

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

        std::stringstream ss;

        std::list<ContourWalkerTool> toolFigures = {};
        std::list<ContourWalker> wpFigures = {};

        ss << std::setw(3) << std::setfill('0') << time;

        if (toolNumber > 1)
            for (unsigned i = 1; i <= toolNumber; ++i)
            {
                ReaderWriter rw("../" + folder + "/" + ss.str() + "-t" + std::to_string(i) + ".csv2d", tool + i);
                toolFigures.push_back(ContourWalkerTool(rw.nodes, rw.contour, rw.symAxisPoints, rw.connect, tool + i));
            }
        else
        {
            ReaderWriter rw("../" + folder + "/" + ss.str() + "-t" + ".csv2d", tool);
            toolFigures.push_back(ContourWalkerTool(rw.nodes, rw.contour, rw.symAxisPoints, rw.connect, tool));
        }

        if (wpNumber > 1)
            for (unsigned i = 1; i <= toolNumber; ++i)
            {
                ReaderWriter rw("../" + folder + "/" + ss.str() + "-wp" + std::to_string(i) + ".csv2d", wp +  i);
                wpFigures.push_back(ContourWalker(rw.nodes, rw.contour, rw.symAxisPoints, rw.connect, wp + i));
            }
        else
        {
            ReaderWriter rw("../" + folder + "/" + ss.str() + "-wp" + ".csv2d", wp);
            wpFigures.push_back(ContourWalker(rw.nodes, rw.contour, rw.symAxisPoints, rw.connect, wp));
        }

        CWM cwmObject(toolFigures, wpFigures);

        cwmObject.findSpace();
        
        cwmObject.trackSpaceArea(lastSpaceAreas);
        lastSpaceAreas = cwmObject.spaceAreas;

        ReaderWriter().write(folder, ss, cwmObject.toolFigures, cwmObject.wpFigures, cwmObject.spaceAreas);

        Drawer().drawAll(window, cwmObject.toolFigures, cwmObject.wpFigures, cwmObject.spaceAreas);

        sf::sleep(sf::milliseconds(300*1));

        window.display();

        if (time++ == 113)
            time = 1;
    }

    return 0;
}