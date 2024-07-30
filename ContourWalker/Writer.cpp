#include "Writer.h"


void Writer::write(std::string folder, CM_CavityModel2D& cm)
{
    std::ofstream fileOut("../return_" + folder + ".txt",
        std::ofstream::out | std::ofstream::trunc);

    for (CM_Cavity2D elem : cm.getCavities())
    {
        writeInOut(fileOut, elem);
    }
};


void Writer::writeInOut(std::ofstream& fileOut, CM_Cavity2D& elem, bool isSym)
{
    fileOut << "Tool" + std::to_string(0) + "; " +
        "Workpiece" + std::to_string(0) + "; " +
        "Square" + std::to_string(0) +
        (isSym ? ".Sym" : "") + ";\n";

    fileOut << "Square: " << std::setprecision(15) << elem.cavitySquare << ";\n";

    fileOut << "Object type; node id;\n";

    for (auto const& point : elem.getContour())
    {
        fileOut <<
            std::to_string(point->sourceObjInfo.source_body_id) +
            "; " + std::to_string(point->sourceObjInfo.mesh_obj_id) +
            (isSym ? ".Sym" : "") + ";\n";
    }
}