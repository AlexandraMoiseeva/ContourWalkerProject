#include "CM_CavityModel2D_FromFile.h"
#include "Reader.h"


void add(Reader& rw, std::vector<Tool>& tools)
{
    for (auto& node : rw.nodes)
        tools.back().addNode(node);

    for (auto id : rw.contour)
        tools.back().addContour(id);

    for (auto point : rw.symAxisPoints)
        tools.back().addSymAxisPoint(point);

    for (auto contactInit : rw.contactInit)
        tools.back().addContact(contactInit);
}


void add(Reader& rw, std::vector<Workpiece>& workpieces)
{
    for (auto& node : rw.nodes)
        workpieces.back().addNode(node);

    for (auto id : rw.contour)
        workpieces.back().addContour(id);

    for (auto point : rw.symAxisPoints)
        workpieces.back().addSymAxisPoint(point);

    for (auto const& contactInit : rw.contactInit)
        workpieces.back().addContact(contactInit);
}


CM_CavityModel2D_FromFile::CM_CavityModel2D_FromFile(std::string folder, int toolNumber, int wpNumber)
    : CM_CavityModel2D()
{
    if (toolNumber > 1)
        for (int i = 1; i <= toolNumber; ++i)
        {
            Reader rw("../" + folder + "-t" +
                std::to_string(i) + ".csv2d", i);
            tools.emplace_back(rw.nodes.size(), i);

            add(rw, tools);
        }
    else
    {
        Reader rw("../" + folder + "-t" + ".csv2d", 0);
        tools.emplace_back(rw.nodes.size(), 0);

        add(rw, tools);
    }


    if (wpNumber > 1)
        for (int i = 1; i <= wpNumber; ++i)
        {
            Reader rw("../" + folder + "-wp" +
                std::to_string(i) + ".csv2d", i);
            workpieces.emplace_back(rw.nodes.size(), i);

            add(rw, workpieces);
        }
    else
    {
        Reader rw("../" + folder + "-wp" + ".csv2d", 0);
        workpieces.emplace_back(rw.nodes.size(), 0);

        add(rw, workpieces);
    }
};