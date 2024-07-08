#include "ContourWalkerDRW.h"
#include "Reader.h"

void add(Reader& rw, std::vector<Tool>& toolFigures)
{
    for (auto node : rw.nodes)
        toolFigures.back().addNode(node);

    for (auto id : rw.contour)
        toolFigures.back().addContour(id);

    for (auto point : rw.symAxisPoints)
        toolFigures.back().addSymAxisPoint(point);

    for (auto contactInit : rw.contactInit)
        toolFigures.back().addContact(contactInit);
}


void add(Reader& rw, std::vector<Workpiece>& wpFigures)
{
    for (auto const& node : rw.nodes)
        wpFigures.back().addNode(node);

    for (auto id : rw.contour)
        wpFigures.back().addContour(id);

    for (auto point : rw.symAxisPoints)
        wpFigures.back().addSymAxisPoint(point);

    for (auto const& contactInit : rw.contactInit)
        wpFigures.back().addContact(contactInit);
}


CM_CavityModel2D_FromFile::CM_CavityModel2D_FromFile(std::string folder, int toolNumber, int wpNumber) : CM_CavityModel2D(toolNumber, wpNumber)
{
    cavitys = new std::vector<CM_Cavity2D>[wpNumber * toolNumber];

    if (toolNumber > 1)
        for (int i = 1; i <= toolNumber; ++i)
        {
            Reader rw("../" + folder + "-t" + std::to_string(i) + ".csv2d", i);
            toolFigures.emplace_back(rw.nodes.size(), detailType::tool, i);

            add(rw, toolFigures);
        }
    else
    {
        Reader rw("../" + folder + "-t" + ".csv2d", 0);
        toolFigures.emplace_back(rw.nodes.size(), detailType::tool, 0);

        add(rw, toolFigures);
    }


    if (wpNumber > 1)
        for (int i = 1; i <= wpNumber; ++i)
        {
            Reader rw("../" + folder + "-wp" + std::to_string(i) + ".csv2d", i);
            wpFigures.emplace_back(rw.nodes.size(), detailType::workpiece, i);

            add(rw, wpFigures);
        }
    else
    {
        Reader rw("../" + folder + "-wp" + ".csv2d", 0);
        wpFigures.emplace_back(rw.nodes.size(), detailType::workpiece, 0);

        add(rw, wpFigures);
    }
};