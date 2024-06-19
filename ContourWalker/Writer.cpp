#include "Writer.h"


Writer::Writer() = default;


void Writer::write(std::string folder, std::stringstream& ss, CWM& cwm)
{
    std::ofstream fileOut("../return_" + folder + "/" + ss.str() + ".txt", std::ofstream::out | std::ofstream::trunc);

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
    fileOutValue << "Tool" + std::to_string(elem.detailToolId) + "; "
        "Workpiece" + std::to_string(elem.detailWPId) + "; "
        + "Square" + std::to_string(elem.spaceAreaId) + (isSym ? ".Sym" : "") + ";\n";

    fileOutValue << "Square: " << std::setprecision(15) << elem.spaceSquare << ";\n";

    fileOutValue << "Object type; node id;\n";

    for (auto const& point : elem.contourWP)
    {
        fileOutValue << "Workpiece" + std::to_string(elem.detailWPId) + "; "
            + std::to_string(cntrWP[point]->sourceObjInfo.mesh_obj_id) + (isSym ? ".Sym" : "") + ";\n";
    }
    for (auto const& point : elem.contourTool)
    {
        fileOutValue << "Tool" + std::to_string(elem.detailToolId) + "; "
            + std::to_string(cntrTool[point]->sourceObjInfo.mesh_obj_id) + (isSym ? ".Sym" : "") + ";\n";
    }
}