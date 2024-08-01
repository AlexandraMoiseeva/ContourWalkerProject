#include "CM_CavityModel2D_FromFile.h"
#include "Reader.h"


void add(Reader& rw, Tool& tool)
{
    for (auto& node : rw.nodes)
        tool.addNode(node);

    for (auto id : rw.contour)
        tool.addContour(id);

    for (auto point : rw.symAxisPoints)
        tool.addSymAxisPoint(point);

    for (auto nodeId = 0; nodeId < rw.contactInit.size(); ++nodeId)
        if (rw.contactInit[nodeId].source_body_id == -1)
            continue;
        else
            tool.addContact(nodeId, rw.contactInit[nodeId]);
}


CM_CavityModel2D_FromFile::CM_CavityModel2D_FromFile(std::string folder, int toolNumber, int wpNumber)
    : CM_CavityModel2D()
{
    tools.reserve(toolNumber);
    workpieces.reserve(wpNumber);

    if (toolNumber > 1)
        for (int i = 1; i <= toolNumber; ++i)
        {
            Reader rw("../" + folder + "-t" +
                std::to_string(i) + ".csv2d", i);
            tools.emplace_back(rw.nodes.size(), i);

            add(rw, tools.back());
        }
    else
    {
        Reader rw("../" + folder + "-t" + ".csv2d", 0);
        tools.emplace_back(rw.nodes.size(), 0);

        add(rw, tools.back());
    }


    if (wpNumber > 1)
        for (int i = 1; i <= wpNumber; ++i)
        {
            Reader rw("../" + folder + "-wp" +
                std::to_string(i) + ".csv2d", i);
            workpieces.emplace_back(rw.nodes.size(), i);

            add(rw, workpieces.back());
        }
    else
    {
        Reader rw("../" + folder + "-wp" + ".csv2d", 0);
        workpieces.emplace_back(rw.nodes.size(), 0);

        add(rw, workpieces.back());
    }
};