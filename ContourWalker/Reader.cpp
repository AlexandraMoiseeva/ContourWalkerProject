#include "Reader.h"

Reader::Reader() = default;


Reader::Reader(std::string const filePathValue, int detailTypeValue)
{

    detailTypeNum = detailTypeValue;
    std::ifstream file;
    file.open(filePathValue);

    ReaderSettingEnum readerSetting = ReaderSettingEnum::skip;

    if (file.is_open())
    {
        std::string s;
        std::string str;

        while (std::getline(file, s))
        {
            if (s.find("#") != 0 && s.find("node") != 0)
            {
                switch (readerSetting)
                {
                case ReaderSettingEnum::skip:
                    break;
                case ReaderSettingEnum::readNodes:
                {
                    std::stringstream ss(s);
                    int id;
                    int indexnum = 0;

                    double x;
                    double z;

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

                    Node node(id, detailTypeValue, x, z);

                    contactInit.push_back(std::make_pair(std::numeric_limits<int>::max(), Edge()));

                    nodes.push_back(node);
                    break;
                }
                case ReaderSettingEnum::readEdge:
                {
                    int numContour = std::stoi(s);

                    contour.push_back(numContour);
                    break;
                }
                case ReaderSettingEnum::readContacts:
                {
                    std::stringstream ss(s);
                    int id;
                    int toolNumber;
                    int n1;
                    int n2;
                    int indexnum = 0;
                    std::string fieldType;

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
                            contactInit[id] = std::make_pair(toolNumber, Edge(n1, n2));
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
            case ReaderBikesEnum::nodeTool:
                readerSetting = ReaderSettingEnum::readNodes;
                break;
            case ReaderBikesEnum::nodeWP:
                readerSetting = ReaderSettingEnum::readNodes;
                break;
            case ReaderBikesEnum::edge:
                readerSetting = ReaderSettingEnum::readEdge;
                break;
            case ReaderBikesEnum::bound:
                readerSetting = ReaderSettingEnum::skip;
                break;
            case ReaderBikesEnum::contact:
                readerSetting = ReaderSettingEnum::readContacts;
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
