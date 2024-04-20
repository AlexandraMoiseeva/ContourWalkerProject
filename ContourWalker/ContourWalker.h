#include "list"
#include "sstream"
#include "fstream"
#include "iomanip"

#include "DTW.hpp"

//������������ ����� �������� �� ������ ���, ��� � #contacts
static enum DetailEnum { symAxis, tool1, tool2, wp};

//������������ ����� ����� � �����, �������������� ������; ��.readerBikes
static enum ReaderBikesEnum { object, nodeTool, nodeWP, edge, bound, contacts };

//������������ �������� ��� ���������� �������������� ����� ������ � �����
static enum ReaderSettingEnum { skip, readNodes, readEdge, readContacts };

//���� ������ ��� ��� ���������
static enum lineDirectionn { vertical, other };

//������� �������������� string �������� � unsigned �� ReaderBikesEnum
static std::map<std::string, unsigned> readerBikes
{
    {"node_id;x;z;T;volume", nodeTool},
    {"node_id;x;z;v_x;v_z;force_x;force_z;T;stress_mean;strain_plast;stress_eff;stress_flow;strain_rate;contact_dist;volume", nodeWP},
    {"#edge;0", edge},
    {"#bound;0", bound},
    {"#contacts", contacts}
};

//������� �������������� string �������� � unsigned �� DetailEnum
static std::map<std::string, unsigned> detailType
{
    {"SymAxis", symAxis},
    {"Tool", tool1},
    {"Workpiece", wp}
};

/*
* ����� ��� ����������� ����
* ������:
*   placeInContour - ����� � �������, ������������ ������
*   id - ����� � �������, �������� ��� �������
*   x, z - �������������� ����������
*/
class Node
{
public:
    int placeInContour = -1;
    int id = NULL;
    float x = NULL;
    float z = NULL;

    Node() {};


    Node(float xValue, float zValue)
    {
        x = xValue;
        z = zValue;
    };


    Node(int idValue, float xValue, float zValue)
    {
        id = idValue;
        x = xValue;
        z = zValue;
    };
};

/*
* ����� ��� ����������� �������
* ������:
*   *n1, *n2 - ��������� �� ����, ��������������� �������
*   A, B, C - ���������� ��� ������ � ���������� ������, ����������� �� �������,
*   �� ���� - y = A/(-B) + C/(-B)
*/
class Segment
{
public:
    Node* n1;
    Node* n2;

    float A, B, C;

    Segment(Node& n1Value, Node& n2Value)
    {
        n1 = new Node{ n1Value };
        n2 = new Node{ n2Value };

        A = (n2->z - n1->z);
        B = (n1->x - n2->x);
        C = (n1->z * n2->x - n2->z * n1->x);
    };

    //���������� ���� - ��������� n �� ������ �������. ���� ������� ������� �� ���� ����������� �����, ���������� n
    Node proection(Node n)
    {
        if (A * A + B * B != 0)
            return Node((B * B * n.x - C * A - A * B * n.z) / (B * B + A * A),
                ((B * B * n.x - C * A - A * B * n.z) / (B * B + A * A)) * A / (-B) - C / B);
        return n;
    };

    //� ����������� �� ����� ������������� ����� ���������� �� ����� ������� �� ������� ����� ���� 
    float inArea(Node* n)
    {
        return -B * (n->z - n1->z) - A * (n->x - n1->x);
    }

    /*
    * ���������, ����� �� �������� ������� ������� � s1 �� ��� �����������.
    * true - ������������,
    * false - �� ������������
    */
    bool boudingBox(Segment s1)
    {
        if (std::min(n1->x, n2->x) > std::max(s1.n1->x, s1.n2->x))
            return false;
        if (std::max(n1->x, n2->x) < std::min(s1.n1->x, s1.n2->x))
            return false;
        if (std::min(n1->z, n2->z) > std::max(s1.n1->z, s1.n2->z))
            return false;
        if (std::max(n1->z, n2->z) < std::min(s1.n1->z, s1.n2->z))
            return false;
        return true;
    };

    /*
    * ���������, ������������ ������ ������� � s1 ��� ���
    * true - ������������
    * false - �� ������������
    */
    bool isSegmentCross(Segment s1)
    {
        return boudingBox(s1) && (inArea(s1.n1) * inArea(s1.n2) <= 0)
            && (s1.inArea(n1) * s1.inArea(n2) <= 0);
    };

    // ���������� ����� ����������� ������ �������� ������ �������� � s1, ���� �� ������������, ���������� ���� �� ����� �������
    Node SegmentCross(Segment s1)
    {
        float det = A * s1.B - s1.A * B;
        if (det == 0)
            return *n1;
        return Node(-(C * s1.B - s1.C * B) / det, -(A * s1.C - s1.A * C) / det);
    };
};

/*
* ��������� ��� ����������� ��� ���������
* ������:
*   a, b - ���������� ��������������� ������
*   ��� ������ - ������������ ������ - x = a
*              - ����� ������ ������ - y = a * x + b
*/
struct LineSymStruct
{
private:
    unsigned linetype = other;

    float a = NULL;
    float b = NULL;

public:
    LineSymStruct() {};

    //����������� ���� � ������� ��� �� ��������� ��������
    LineSymStruct(float xzSum, float xSum, float zSum, float x2Sum, float n)
    {
        if (n * x2Sum == xSum * xSum)
        {
            a = xSum / n;
            linetype = vertical;
        }
        else
        {
            a = (n * xzSum - xSum * zSum) / (n * x2Sum - xSum * xSum);
            b = (zSum - a * xSum) / n;
        }
    };

    //���������� ���� ������������ n
    Node getSymNode(Node n)
    {
        if (linetype == vertical)
            return Node(-1, -n.x, n.z);
        if (a == 0)
            return Node(-1, n.x, 2 * b - n.z);
        float x0 = (n.z + (1.0f / a) * n.x - b) / (a + (1.0f / a));
        float z0 = a * x0 + b;
        return (Node(-1, 2 * x0 - n.x, 2 * z0 - n.z));
    }

};

//��������� �������������� ��� ���������
struct Drawer
{
public:
    /*
    * ���������� ���������� ��� ������ SFML.����� � ����������� ���������,
    * �������� ������ ������� ��� ���������� ������������
    */
    std::pair<float, float> drawScale(Node node, float scale = 5)
    {
        return std::make_pair(node.x * 1920 * scale + 50, (0.4 - node.z) * 1080 * scale);
    };


    std::pair<float, float> drawScale(std::vector<double> node, float scale = 5)
    {
        return std::make_pair(node[0] * 1920 * scale + 50, (0.4 - node[1]) * 1080 * scale);
    };

    /*
    * ������������ ������� �� ������ �����
    * �������� ������ ������������. ��� � ����� �����
    */
    void drawLine(Node n1, Node n2, sf::RenderWindow& window, unsigned alpha = 255)
    {
        auto vectorpoint1result = drawScale(n1);
        auto vectorpoint2result = drawScale(n2);

        auto vectorpoint1 = sf::Vector2f(vectorpoint1result.first, vectorpoint1result.second);
        auto vectorpoint2 = sf::Vector2f(vectorpoint2result.first, vectorpoint2result.second);

        sf::Vertex line[] =
        {
            sf::Vertex(vectorpoint1),
            sf::Vertex(vectorpoint2)
        };

        line[0].color = sf::Color(255, 255, 255, alpha);
        line[1].color = sf::Color(255, 255, 255, alpha);

        window.draw(line, 2, sf::Lines);

        return;
    }


    void drawLine(std::vector<double> n1, std::vector<double> n2, sf::RenderWindow& window, unsigned alpha = 255)
    {
        auto vectorpoint1result = drawScale(n1);
        auto vectorpoint2result = drawScale(n2);

        auto vectorpoint1 = sf::Vector2f(vectorpoint1result.first, vectorpoint1result.second);
        auto vectorpoint2 = sf::Vector2f(vectorpoint2result.first, vectorpoint2result.second);

        sf::Vertex line[] =
        {
            sf::Vertex(vectorpoint1),
            sf::Vertex(vectorpoint2)
        };

        line[0].color = sf::Color(255, 255, 255, alpha);
        line[1].color = sf::Color(255, 255, 255, alpha);

        window.draw(line, 2, sf::Lines);

        return;
    }
};

/*
* �����, ���������� �� ��� ������ � ����� �� �����
* ������:
*   detailTypeNum - ��� ������
*   nodes - ������, �������� ���� � ������� �����������
*   contour - ������, �������� ��������� �� ���� � �������, ���������� ������ 
*   symAxisPoints - ������ ������ �����, ������� �� ��� ���������
*   LineSym - ��� ���������, ��������� LineSymStruct
*   connect - ������ �������, ������� ���������� �����, ��� � ����������� ������ ����
              �������� ���� - ������ ����� �������, �������� �� ��������. 
              � ������ Tool1 � Tool2 - ������� � wp, � ������ � wp - ������� � Tool1.
              ���� ���� �� ��������, �� � ����������� �������� ���� (-1, -1)
*   connect1 - ������ �������, ������� ���������� �����, ��� � ����������� ������ ����
              �������� ���� - ������ ����� �������, �������� �� ��������. 
              � ������ Tool1 � Tool2 - ���� ������ ����, � ������ � wp - ������� � Tool2.
              ���� ���� �� ��������, �� � ����������� �������� ���� (-1, -1)
*   connectSpace - ������ ������ ������ �������� (��� Tool1, Tool2 - � wp, � wp - c Tool1)
                   - ������� - ����:
                                  1) ������ ������, ��������������� ������
                                  2) ���� - �������, �������� �������� ������ � ��������� ����
*   connectSpace - ������ ������ ������ �������� (��� Tool1, Tool2 - ������, � wp - c Tool2)
                   - ������� - ����:
                                  1) ������ ������, ��������������� ������
                                  2) ���� - �������, �������� �������� ������ � ��������� ����
*/
class ContourWalker
{
public:
    int detailTypeNum = tool1;
    std::vector<Node> nodes = {};
    std::vector<Node*> contour = {};

    std::list<int> symAxisPoints = {};
    LineSymStruct LineSym;

    std::map<int, std::pair<int, int> > connect = {};
    std::map<int, std::pair<int, int> > connect1 = {};

    std::vector<std::pair< std::list<int>, std::pair<Segment, Segment> >> connectSpace = {};
    std::vector<std::pair< std::list<int>, std::pair<Segment, Segment> >> connectSpace1 = {};


    ContourWalker() {};


    ContourWalker(std::string filePathValue, int detailTypeValue = tool1)
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
                        float x, z;

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

                        connect[id] = std::make_pair(-1, -1);
                        connect1[id] = std::make_pair(-1, -1);

                        nodes.push_back(node);
                        break;
                    }
                    case readEdge:
                    {
                        int numContour = std::stoi(s);
                        if (nodes[numContour].placeInContour == -1)
                            nodes[numContour].placeInContour = contour.size();

                        contour.push_back(&nodes[numContour]);
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
                                if (detailType[fieldType] == symAxis)
                                    symAxisPoints.push_back(id);
                            }
                            if (indexnum == 2)
                            {
                                toolNumber = std::stof(str);
                            }
                            if (indexnum == 3)
                                n1 = std::stof(str);
                            if (indexnum == 4)
                            {
                                n2 = std::stof(str);
                                if (toolNumber == tool2)
                                    connect1[id] = std::make_pair(n1, n2);
                                else
                                    connect[id] = std::make_pair(n1, n2);
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
            //������� ��������� ������� � �������, �.�. ��� �����������
            contour.pop_back();
        }
        file.close();
    };

    //��������� �������
    void draw(sf::RenderWindow& window)
    {
        for (auto it = contour.cbegin(); it != contour.cend(); ++it)
        {
            Node nodepoint1, nodepoint2;

            if (it == contour.begin())
            {
                nodepoint1 = **std::next(contour.end(), -1);
                nodepoint2 = **contour.begin();
            }
            else
            {
                nodepoint1 = **it;
                nodepoint2 = **std::next(it, -1);
            }
            
            Drawer().drawLine(nodepoint1, nodepoint2, window, 100);

            if (symAxisPoints.size() == 0)
                continue;

            Drawer().drawLine(LineSym.getSymNode(nodepoint1), LineSym.getSymNode(nodepoint2), window, 100);
        }
        return;
    };

    //����������� ��� ���������
    void symAxisInizialisation()
    {
        if (symAxisPoints.size() == 0)
            return;

        float xzSum = 0, xSum = 0, zSum = 0, x2Sum = 0, n = 0;

        for (int idElem : symAxisPoints)
        {
            Node elem = nodes[idElem];

            xzSum += elem.x * elem.z;
            xSum += elem.x;
            zSum += elem.z;
            x2Sum += elem.x * elem.x;
            ++n;
        }

        LineSym = LineSymStruct(xzSum, xSum, zSum, x2Sum, n);
    };

    //���������� �������������� connectSpace - ���������� �������� �� ������������ ����� � �������������� ��������
    void intersectionSpace(ContourWalker& otherDetail, unsigned detailTypeValue = tool1)
    {
        bool StartSpaceContour = false;
        bool startRot = true;
        auto connectWish = detailTypeValue == tool2 ? connect1 : connect;

        std::vector<std::pair< std::list<int>, std::pair<Segment, Segment> >> connectSpaceWish;

        for (auto it = contour.begin(); it != contour.end(); ++it)
        {
            if (connectWish[(*it)->id].first != -1 or std::find(symAxisPoints.begin(), symAxisPoints.end(), (*it)->id) != symAxisPoints.end())
                startRot = false;
            if (startRot)
                continue;
            if (connectWish[(*it)->id].first == -1 and std::find(symAxisPoints.begin(), symAxisPoints.end(), (*it)->id) == symAxisPoints.end())
            {
                if (StartSpaceContour == false)
                {
                    if (connectWish[(*std::next(it, -1))->id].first != -1)
                        connectSpaceWish.push_back(std::make_pair(std::list<int>{(*std::next(it, -1))->id},
                            std::make_pair(
                                Segment(*otherDetail.contour[otherDetail.nodes[connectWish[(*std::prev(it))->id].first].placeInContour],
                                    *otherDetail.contour[otherDetail.nodes[connectWish[(*std::prev(it))->id].second].placeInContour]),
                                Segment(*otherDetail.contour[otherDetail.nodes[connectWish[(*std::prev(it))->id].first].placeInContour],
                                    *otherDetail.contour[otherDetail.nodes[connectWish[(*std::prev(it))->id].second].placeInContour]))));
                    else
                        connectSpaceWish.push_back(std::make_pair(std::list<int>{(*std::next(it, -1))->id},
                            std::make_pair(
                                Segment(*otherDetail.contour[otherDetail.nodes[*otherDetail.symAxisPoints.begin()].placeInContour],
                                    *otherDetail.contour[otherDetail.nodes[*otherDetail.symAxisPoints.begin()].placeInContour]),
                                Segment(*otherDetail.contour[otherDetail.nodes[*otherDetail.symAxisPoints.begin()].placeInContour],
                                    *otherDetail.contour[otherDetail.nodes[*otherDetail.symAxisPoints.begin()].placeInContour]))));
                    StartSpaceContour = true;
                }
                else
                    connectSpaceWish[connectSpaceWish.size() - 1].first.push_back((*std::next(it, -1))->id);
            }
            if (connectWish[(*it)->id].first != -1 or std::find(symAxisPoints.begin(), symAxisPoints.end(), (*it)->id) != symAxisPoints.end())
            {
                if (StartSpaceContour == true)
                {
                    StartSpaceContour = false;

                    connectSpaceWish[connectSpaceWish.size() - 1].first.push_back((*std::next(it, -1))->id);
                    connectSpaceWish[connectSpaceWish.size() - 1].first.push_back((*it)->id);
                    if (std::find(symAxisPoints.begin(), symAxisPoints.end(), (*it)->id) == symAxisPoints.end())
                        connectSpaceWish[connectSpaceWish.size() - 1].second.first = Segment(*otherDetail.contour[otherDetail.nodes[connectWish[(*it)->id].first].placeInContour],
                            *otherDetail.contour[otherDetail.nodes[connectWish[(*it)->id].second].placeInContour]);
                    else
                        connectSpaceWish[connectSpaceWish.size() - 1].second.first = Segment(*otherDetail.contour[otherDetail.nodes[*otherDetail.symAxisPoints.begin()].placeInContour],
                            *otherDetail.contour[otherDetail.nodes[*otherDetail.symAxisPoints.begin()].placeInContour]);
                }
            }
        }

        if (detailTypeValue != tool1)
            connectSpaceWish.pop_back();
        else
            connectSpaceWish.erase(connectSpaceWish.begin());

        if (detailTypeValue == tool2)
            std::copy(connectSpaceWish.begin(), connectSpaceWish.end(), std::back_inserter(connectSpace1));
        else
            std::copy(connectSpaceWish.begin(), connectSpaceWish.end(), std::back_inserter(connectSpace));
    }

    //���������� �������� ������ �������� � ���������� �� �������� - ������� ������� ������
    std::vector<std::pair<std::pair<unsigned, float>, std::vector<std::vector<double>>>> intersection(ContourWalker& otherDetail,
        unsigned detailTypeValue, std::vector<std::pair<std::pair<unsigned, float>, std::vector<std::vector<double>>>> lastSpaceArea)
    {
        std::vector<std::pair< std::list<int>, std::pair<Segment, Segment> >> connectSpaceWish = detailTypeValue == tool2 ? connectSpace1 : connectSpace;
        std::vector<std::pair<std::pair<unsigned, float>, std::vector<std::vector<double>>>> spaceArea = {};

        for (int i = 0; i < connectSpaceWish.size(); ++i)
        {
            float sumSquare = 0.0f;
            Node point1, point2;

            point1 = nodes[*connectSpaceWish[i].first.begin()];

            spaceArea.push_back(std::make_pair(std::make_pair(i, 0), std::vector<std::vector<double>> { { point1.x, point1.z } }));

            for (auto elem : connectSpaceWish[i].first)
            {
                point2 = nodes[elem];

                spaceArea[spaceArea.size() - 1].second.push_back({ point2.x, point2.z });

                sumSquare += 0.5 * (point1.x * point2.z - point2.x * point1.z);

                point1 = point2;
            }

            point2 = connectSpaceWish[i].second.first.proection(nodes[*std::prev(connectSpaceWish[i].first.end())]);

            spaceArea[spaceArea.size() - 1].second.push_back({ point2.x, point2.z });

            sumSquare += 0.5 * (point1.x * point2.z - point2.x * point1.z);

            point1 = point2;

            if (connectSpaceWish[i].second.first.n1->id != connectSpaceWish[i].second.first.n2->id and
                connectSpaceWish[i].second.second.n1->id != connectSpaceWish[i].second.second.n2->id)
                for (auto elem1 = std::next(&otherDetail.contour[connectSpaceWish[i].second.first.n1->placeInContour]);
                    elem1 != &otherDetail.contour[connectSpaceWish[i].second.second.n2->placeInContour]; ++elem1)
            {
                point2 = **elem1;

                spaceArea[spaceArea.size() - 1].second.push_back({ point2.x, point2.z });

                sumSquare += 0.5 * (point1.x * point2.z - point2.x * point1.z);

                point1 = point2;
            }
            else
                if (connectSpaceWish[i].second.first.n1->id != connectSpaceWish[i].second.first.n2->id)
                    for (auto elem1 = std::next(&otherDetail.contour[connectSpaceWish[i].second.first.n1->placeInContour]);
                        std::find(otherDetail.symAxisPoints.begin(), otherDetail.symAxisPoints.end(), (*std::prev(elem1))->id) == otherDetail.symAxisPoints.end(); ++elem1)
            {
                point2 = **elem1;

                spaceArea[spaceArea.size() - 1].second.push_back({ point2.x, point2.z });

                sumSquare += 0.5 * (point1.x * point2.z - point2.x * point1.z);

                point1 = point2;
            }

            point2 = connectSpaceWish[i].second.second.proection(point1);

            spaceArea[spaceArea.size() - 1].second.push_back({ point2.x, point2.z });

            sumSquare += 0.5 * (point1.x * point2.z - point2.x * point1.z);

            point1 = point2;

            point2 = nodes[*connectSpaceWish[i].first.begin()];

            sumSquare += 0.5 * (point1.x * point2.z - point2.x * point1.z);

            spaceArea[i].first.second = abs(sumSquare);
        }

        //������� ������������ ��������
        std::vector<std::pair<double, int>> distances(lastSpaceArea.size());
        std::fill(distances.begin(), distances.end(), std::make_pair(std::numeric_limits<double>::max(), -1));

        int idNumber = lastSpaceArea.size() - 1;

        for (int i = 0; i < spaceArea.size(); i++)
        {
            double distanceMin = std::numeric_limits<double>::max();
            int id = -1;

            for (int j = 0; j < lastSpaceArea.size(); j++)
            {
                double distance = DTW::dtw_distance_only(spaceArea[i].second, lastSpaceArea[j].second, 2);
                distanceMin, id = distance < distanceMin ? distance, lastSpaceArea[j].first.first : distanceMin, id;
            }

            if (id != -1)
            {
                if (distances[id].first < distanceMin)
                {
                    ++idNumber;
                    spaceArea[i].first.first = idNumber;
                }
                else
                {
                    if (distances[id].second == -1)
                        distances[id] = std::make_pair(distanceMin, i);
                    else
                    {
                        ++idNumber;
                        spaceArea[distances[id].second].first.first = idNumber;
                        distances[id] = std::make_pair(distanceMin, i);
                    }
                    spaceArea[i].first.first = id;
                }
            }
            else
            {
                ++idNumber;
                spaceArea[i].first.first = idNumber;
            }
        }

        return spaceArea;
    }
};

/*
* �����, ���������� �� ������� ������ � �����
* ������:
*   ss - ����� �������� � ������ �������
*   firstFigure - Tool1, � ������ ContourWalker
*   secondFigure - Tool2, � ������ ContourWalker
*   wpFigure - WP, � ������ ContourWalker
*   spaceAreaTool1 - ������ ������ ������� Wp � Tool1, �� �������, �� �����
*   spaceAreaTool2 - ������ ������ ������� Wp � Tool2, �� �������, �� �����
*/
class CWM
{
private:
    std::stringstream ss;
    ContourWalker firstFigure;
    ContourWalker secondFigure;
    ContourWalker wpFigure;

    std::vector<std::pair<std::pair<unsigned, float>, std::vector<std::vector<double>>>> spaceAreaTool1 = {};
    std::vector<std::pair<std::pair<unsigned, float>, std::vector<std::vector<double>>>> spaceAreaTool2 = {};


public:

    CWM(unsigned time)
    {
        ss << std::setw(3) << std::setfill('0') << time;

        firstFigure = ContourWalker("../data(1)/" + ss.str() + "-t1.csv2d");
        secondFigure = ContourWalker("../data(1)/" + ss.str() + "-t2.csv2d");
        wpFigure = ContourWalker("../data(1)/" + ss.str() + "-wp.csv2d");
    };


    CWM(unsigned time, std::pair<std::vector<std::pair<std::pair<unsigned, float>, std::vector<std::vector<double>>>>,
                                std::vector<std::pair<std::pair<unsigned, float>, std::vector<std::vector<double>>>>> spaceAreas)
    {
        ss << std::setw(3) << std::setfill('0') << time;

        firstFigure = ContourWalker("../data(1)/" + ss.str() + "-t1.csv2d");
        secondFigure = ContourWalker("../data(1)/" + ss.str() + "-t2.csv2d");
        wpFigure = ContourWalker("../data(1)/" + ss.str() + "-wp.csv2d");

        spaceAreaTool1 = spaceAreas.first;
        spaceAreaTool2 = spaceAreas.second;
    };
    
    
    //��������� �������� � ������ � ����� returnData
    void findSpace()
    {
        firstFigure.symAxisInizialisation();
        secondFigure.symAxisInizialisation();
        wpFigure.symAxisInizialisation();

        firstFigure.intersectionSpace(wpFigure, wp);
        secondFigure.intersectionSpace(wpFigure, wp);
        wpFigure.intersectionSpace(firstFigure, tool1);
        wpFigure.intersectionSpace(secondFigure, tool2);

        spaceAreaTool1 = wpFigure.intersection(firstFigure, tool1, spaceAreaTool1);
        spaceAreaTool2 = wpFigure.intersection(secondFigure, tool2, spaceAreaTool2);

        std::ofstream fileOut("../returnData/" + ss.str() + ".txt", std::ofstream::out | std::ofstream::trunc);

        if (fileOut.is_open())
        {
            for (auto elem : spaceAreaTool1)
            {
                fileOut << "Tool" + std::to_string(tool1) + "; " + "Square" + std::to_string(elem.first.first) + "\n";
                fileOut << "Square" + std::to_string(elem.first.second) + "\n";
                fileOut << "x; z;\n";
                for (auto point : elem.second)
                {
                    fileOut << std::to_string(point[0]) + "; " + std::to_string(point[1]) + "\n";
                }
            }

            for (auto elem : spaceAreaTool2)
            {
                fileOut << "Tool" + std::to_string(tool2) + "; " + "Square" + std::to_string(elem.first.first) + "\n";
                fileOut << "Square- " + std::to_string(elem.first.second) + "\n";
                fileOut << "x; z;\n";
                for (auto point : elem.second)
                {
                    fileOut << std::to_string(point[0]) + "; " + std::to_string(point[1]) + "\n";
                }
            }
            fileOut.close();
        }
    }

    //��������� ������ ��������
    void drawSpace(sf::RenderWindow& window, unsigned toolNumber)
    {
        auto spaceArea = toolNumber == tool1 ? spaceAreaTool1 : spaceAreaTool2;

        sf::Text text;
        sf::Font font;

        if (!font.loadFromFile("ArialRegular.ttf"))
        {
            std::cerr << "�� ������� ����� �����\n";
        }

        text.setFont(font);
        text.setCharacterSize(16);
        text.setFillColor(sf::Color::Red);

        for (auto elem: spaceArea)
        {
            auto point0 = elem.second[0];

            for (auto point = std::next(elem.second.begin()); point != elem.second.end(); point++)
            {
                Drawer().drawLine(point0, *point, window);

                point0 = *point;
            }

            Drawer().drawLine(point0, elem.second[0], window);

            auto textPoint = Drawer().drawScale(point0);
            text.setPosition(textPoint.first + 50, textPoint.second + 50);
            text.setString(std::to_string(elem.first.second));
            window.draw(text);

            text.setPosition(textPoint.first + 50, textPoint.second);
            text.setString(std::to_string(toolNumber) + '.' + std::to_string(elem.first.first));
            window.draw(text);
        }
    }

    //��������� �������� � �������� �����
    void drawAll(sf::RenderWindow& window)
    {
        firstFigure.draw(window);
        secondFigure.draw(window);
        wpFigure.draw(window);

        drawSpace(window, tool1);
        drawSpace(window, tool2);
    }

    //���������� ���� - ������ ������� wp � Tool1 � Tool2
    std::pair<std::vector<std::pair<std::pair<unsigned, float>, std::vector<std::vector<double>>>>,
        std::vector<std::pair<std::pair<unsigned, float>, std::vector<std::vector<double>>>>> returnSpaceArea()
    {
        return std::make_pair(spaceAreaTool1, spaceAreaTool2);
    }
};