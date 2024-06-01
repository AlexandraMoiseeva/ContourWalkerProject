#pragma once
#ifndef READER_H
#define READER_H

#include <iomanip>
#include <list>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "Geometry.h"


const enum ReaderBikesEnum { object, nodeTool, nodeWP, edge, bound, contacts };


const enum ReaderSettingEnum { skip, readNodes, readEdge, readContacts };

class ReaderWriter
{
public:
    unsigned detailTypeNum = tool;
    std::vector< Node > nodes = {};
    std::list< unsigned > contour = {};

    std::list< unsigned > symAxisPoints = {};

    std::list< std::pair<unsigned, Segment> > connect = {};

    std::map<std::string, unsigned> readerBikes
    {
        {"node_id;x;z;T;volume", nodeTool},
        {"node_id;x;z;v_x;v_z;force_x;force_z;T;stress_mean;strain_plast;stress_eff;stress_flow;strain_rate;contact_dist;volume", nodeWP},
        {"#edge;0", edge},
        {"#bound;0", bound},
        {"#contacts", contacts}
    };


    ReaderWriter() {};


    ReaderWriter(std::string filePathValue, int detailTypeValue = tool)
    {

        detailTypeNum = detailTypeValue;
        std::ifstream file;
        file.open(filePathValue);

        unsigned readerSetting = skip;

        if (file.is_open())
        {
            std::string s, str;
            while (std::getline(file, s))
            {
                if (s.find("#") != 0 and s.find("node") != 0)
                {
                    switch (readerSetting)
                    {
                    case skip:
                        break;
                    case readNodes:
                    {
                        std::stringstream ss(s);
                        int id, indexnum = 0;
                        double x, z;

                        while (std::getline(ss, str, ';'))
                        {
                            if (indexnum == 0)
                                id = std::stoi(str);
                            if (indexnum == 1)
                                x = std::stof(str);
                            if (indexnum == 2)
                                z = std::stof(str);

                            ++indexnum;
                        }

                        Node node(id, x, z);
                        node.toolType = detailTypeNum;

                        connect.push_back(std::make_pair(std::numeric_limits<unsigned>::max(), Segment()));

                        nodes.push_back(node);
                        break;
                    }
                    case readEdge:
                    {
                        int numContour = std::stoi(s);
                        if (std::next(nodes.begin(), numContour)->placeInContour == std::numeric_limits<unsigned>::max())
                            std::next(nodes.begin(), numContour)->placeInContour = contour.size();

                        contour.push_back(numContour);
                        break;
                    }
                    case readContacts:
                    {
                        std::stringstream ss(s);
                        int id, toolNumber, n1, n2, indexnum = 0;
                        std::string fieldType;
                        bool isNotBreak = true;

                        while (std::getline(ss, str, ';'))
                        {
                            if (indexnum == 0)
                                id = std::stoi(str);
                            if (indexnum == 1)
                            {
                                fieldType = str;
                                if (fieldType == "SymAxis")
                                {
                                    symAxisPoints.push_back(id);
                                    std::next(nodes.begin(), id)->isSym = true;
                                }
                            }
                            if (indexnum == 2)
                            {
                                toolNumber = std::stoi(str);
                            }
                            if (indexnum == 3)
                                n1 = std::stoi(str);
                            if (indexnum == 4)
                            {
                                n2 = std::stoi(str);
                                *std::next(connect.begin(), id) = std::make_pair(toolNumber + tool, Segment(n1, n2));
                            }

                            ++indexnum;
                        }

                        break;
                    }
                    default:
                        break;
                    }
                    continue;
                }
                switch (readerBikes[s])
                {
                case nodeTool:
                    readerSetting = readNodes;
                    break;
                case nodeWP:
                    readerSetting = readNodes;
                    break;
                case edge:
                    readerSetting = readEdge;
                    break;
                case bound:
                    readerSetting = skip;
                    break;
                case contacts:
                    readerSetting = readContacts;
                    break;
                default:
                    break;
                }
            }
            //”дал€ем последний элемент в контуре, т.к. они повтор€ютс€
            contour.pop_back();
        }
        file.close();
    };


    void writeInOut(std::ofstream& fileOutValue, SpaceArea elem, std::vector<Node*>& cntrWP, std::vector<Node*>& cntrTool, bool isSym = false)
    {
        fileOutValue << "Tool" + std::to_string((int)elem.detailToolId - (int)tool) + "; "
            "Workpiece" + std::to_string((int)elem.detailWPId - (int)wp) + "; "
            + "Square" + std::to_string(elem.spaceAreaId) + (isSym ? ".Sym" : "") + ";\n";

        fileOutValue << "Square: " << std::setprecision(15) << elem.spaceSquare << ";\n";

        fileOutValue << "Object type; node id;\n";

        for (auto point = elem.contourWP.begin(); point != elem.contourWP.end(); ++point)
        {
            fileOutValue << "Workpiece" + std::to_string((int)elem.detailWPId - (int)wp) + "; "
                + std::to_string((*std::next(cntrWP.begin(), *point))->id) + (isSym ? ".Sym" : "") + ";\n";
        }
        for (auto point = elem.contourTool.begin(); point != elem.contourTool.end(); ++point)
        {
            fileOutValue << "Tool" + std::to_string((int)elem.detailToolId - (int)tool) + "; "
                + std::to_string((*std::next(cntrTool.begin(), *point))->id) + (isSym ? ".Sym" : "") + ";\n";
        }
    }


    void write(std::string folder, std::stringstream& ss, std::list<ContourWalkerTool>& toolFigures, std::list<ContourWalker>& wpFigures, std::list< std::list<SpaceArea>>& spaceAreas)
    {
        std::ofstream fileOut("../return" + folder + "/" + ss.str() + ".txt", std::ofstream::out | std::ofstream::trunc);

        for (std::list<SpaceArea> elemList : spaceAreas)
            for (SpaceArea elem : elemList)
            {
                std::vector<Node*> cntrWP = std::next(wpFigures.begin(), elem.detailWPId - wp)->contour;
                std::vector<Node*> cntrTool = std::next(toolFigures.begin(), elem.detailToolId - tool - 1)->contour;

                writeInOut(fileOut, elem, cntrWP, cntrTool);
            }
    }
};

#endif