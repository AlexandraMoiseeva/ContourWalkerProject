#include "Writer.h"


Writer::Writer() = default;


void Writer::write(std::string folder, CM_CavityModel2D& cm)
{
    std::ofstream fileOut("../return_" + folder + ".txt", std::ofstream::out | std::ofstream::trunc);

    for (int i = 0; i < 2 * 1; ++i)
        for (CM_Cavity2D elem : cm.cavitys[i])
        {
            writeInOut(fileOut, elem);
        }

}


void Writer::writeInOut(std::ofstream& fileOutValue, CM_Cavity2D elem, bool isSym)
{
    fileOutValue << "Tool" + std::to_string(elem.detailToolId) + "; "
        "Workpiece" + std::to_string(elem.detailWPId) + "; "
        + "Square" + std::to_string(elem.cavityId) + (isSym ? ".Sym" : "") + ";\n";

    fileOutValue << "Square: " << std::setprecision(15) << elem.spaceSquare << ";\n";

    fileOutValue << "Object type; node id;\n";

    for (auto const& point : elem)
    {
        fileOutValue << std::to_string((*point)->sourceObjInfo.source_body_id) + std::to_string(elem.detailWPId) + "; "
            + std::to_string((*point)->sourceObjInfo.mesh_obj_id) + (isSym ? ".Sym" : "") + ";\n";
    }
}