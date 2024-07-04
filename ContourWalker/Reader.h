#pragma once

#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <map>

#include "Geometry.h"

const enum class ReaderBikesEnum { object, nodeTool, nodeWP, edge, bound, contact };


const enum class ReaderSettingEnum { skip, readNodes, readEdge, readContacts };


class Reader
{
private:
    std::map<std::string, ReaderBikesEnum> readerBikes
    {
        {"node_id;x;z;T;volume", ReaderBikesEnum::nodeTool},
        {"node_id;x;z;v_x;v_z;force_x;force_z;T;stress_mean;strain_plast;stress_eff;stress_flow;strain_rate;contact_dist;volume", ReaderBikesEnum::nodeWP},
        {"#edge;0", ReaderBikesEnum::edge},
        {"#bound;0", ReaderBikesEnum::bound},
        {"#contacts", ReaderBikesEnum::contact}
    };
public:
    int detailTypeNum = 0;
    std::vector<Node> nodes = {};
    std::vector<int> contour = {};

    std::vector<int> symAxisPoints = {};

    std::vector<std::pair<int, Edge>> contactInit = {};
    

    Reader();


    Reader(std::string filePathValue, int detailTypeValue = 0);

};
