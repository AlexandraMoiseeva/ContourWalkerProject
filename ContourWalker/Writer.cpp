#include "Writer.h"


Writer::Writer() {};


void Writer::write(std::string folder, std::stringstream& ss, CWMDrawerReader& cwm)
{
    std::ofstream fileOut("../return" + folder + "/" + ss.str() + ".txt", std::ofstream::out | std::ofstream::trunc);

    for (int i = 0; i < 2 * 1; ++i)
        for (SpaceArea elem : cwm.spaceAreas[i])
        {
            std::vector<Node*> cntrWP = std::next(cwm.wpFigures.begin(), elem.detailWPId + (elem.detailWPId == 0 ? 0 : -1))->contour;
            std::vector<Node*> cntrTool = std::next(cwm.toolFigures.begin(), elem.detailToolId + (elem.detailToolId == 0 ? 0 : -1))->contour;

            writeInOut(fileOut, elem, cntrWP, cntrTool);
        }

}


void Writer::writeInOut(std::ofstream& fileOutValue, SpaceArea elem, std::vector<Node*>& cntrWP, std::vector<Node*>& cntrTool, bool isSym)
{
    fileOutValue << "Tool" + std::to_string((int)elem.detailToolId) + "; "
        "Workpiece" + std::to_string((int)elem.detailWPId) + "; "
        + "Square" + std::to_string(elem.spaceAreaId) + (isSym ? ".Sym" : "") + ";\n";

    fileOutValue << "Square: " << std::setprecision(15) << elem.spaceSquare << ";\n";

    fileOutValue << "Object type; node id;\n";

    for (auto point = elem.contourWP.begin(); point != elem.contourWP.end(); ++point)
    {
        fileOutValue << "Workpiece" + std::to_string((int)elem.detailWPId) + "; "
            + std::to_string((*std::next(cntrWP.begin(), *point))->id) + (isSym ? ".Sym" : "") + ";\n";
    }
    for (auto point = elem.contourTool.begin(); point != elem.contourTool.end(); ++point)
    {
        fileOutValue << "Tool" + std::to_string((int)elem.detailToolId) + "; "
            + std::to_string((*std::next(cntrTool.begin(), *point))->id) + (isSym ? ".Sym" : "") + ";\n";
    }
}