#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <set>
#include <list>
#include <iterator>
#include <SFML/Graphics.hpp>

static enum DetailEnum { symAxis, wp, tool };
static enum ReaderBikesEnum { object, nodeTool, nodeWP, edge, bound, contacts };
static enum ReaderSettingEnum { skip, readNodes, readEdge, readContacts };
static enum lineDirectionn { vertical, other };

static std::map<std::string, unsigned> readerBikes
{
    {"node_id;x;z;T;volume", nodeTool},
    {"node_id;x;z;v_x;v_z;force_x;force_z;T;stress_mean;strain_plast;stress_eff;stress_flow;strain_rate;contact_dist;volume", nodeWP},
    {"#edge;0", edge},
    {"#bound;0", bound},
    {"#contacts", contacts}
};

static std::map<std::string, unsigned> detailType
{
    {"SymAxis", symAxis},
    {"Tool", tool},
    {"Workpiece", wp}
};

class Node
{
public:
    int placeInContour = -1;
    int id = NULL;
    float x = NULL;
    float z = NULL;

    Node() {};

    Node(int idValue, float xValue, float zValue) 
    {
        id = idValue;
        x = xValue;
        z = zValue;
    };

};

class Segment
{
public :
    Node *n1;
    Node *n2;

    float A, B, C;

    Segment(Node& n1Value, Node& n2Value)
    {
        n1 = new Node{ n1Value };
        n2 = new Node{ n2Value };

        A = (n2->z - n1->z);
        B = (n1->x - n2->x);
        C = (n1->z * n2->x - n2->z * n1->x);
    };

    float inArea(Node *n)
    {
        return -B * (n->z - n1->z) - A * (n->x - n1->x);
    }

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

    bool isSegmentCross(Segment s1)
    {
        return boudingBox(s1) && (inArea(s1.n1) * inArea(s1.n2) <= 0)
            && (s1.inArea(n1) * s1.inArea(n2) <= 0);
    };

    Node SegmentCross (Segment s1)
    {
        float det = A * s1.B - s1.A * B;
        if (det == 0)
            return *n1;
        return Node(-1, -(C * s1.B - s1.C * B) / det, -(A * s1.C - s1.A * C) / det);
    };

    void swap()
    {
        auto temp = n1;
        n1 = n2;
        n2 = temp;
    };
};

std::pair<float, float> drawScale(Node node, float scale = 5)
{
    return std::make_pair(node.x * 1920 * scale + 50, (0.4 - node.z) * 1080 * scale);
};

struct LineSymStruct
{
    unsigned linetype = other;

    float a = NULL;
    float b = NULL;

    LineSymStruct() {};


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

class Reader
{
public:
    int detailTypeNum = tool;
    std::vector<Node> nodes = {};
    std::vector<Node*> contour = {};
    
    std::list<int> symAxisPoints = {};
    LineSymStruct LineSym;

    std::map<int, std::pair<int, int> > connect = {};
    std::map<int, std::pair<int, int> > connect1 = {};

    std::vector<std::pair<std::list<int>, Segment>> connectSpace = {};
    std::vector<std::pair<std::list<int>, Segment>> connectSpace1 = {};


    Reader(std::string filePathValue, int detailTypeValue = tool)
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
                if (s.find("#")!=0 and s.find("node")!=0)
                {

                    switch(readerSetting)
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
                                    if (toolNumber == 2)
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
            contour.pop_back();
            std::cout << filePathValue << std::endl;
        }
        file.close();
        std::cout << "File has been written" << std::endl;
    };


    void draw(sf::RenderWindow &window)
    {
        for (auto it = contour.cbegin(); it != contour.end(); ++it)
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

            auto vectorpoin1result = drawScale(nodepoint1);
            sf::Vector2f vectorpoint1 = sf::Vector2f(vectorpoin1result.first, vectorpoin1result.second);

            auto vectorpoin2result = drawScale(nodepoint2);
            sf::Vector2f vectorpoint2 = sf::Vector2f(vectorpoin2result.first, vectorpoin2result.second);

            sf::Vertex line[] =
            {
                sf::Vertex(vectorpoint1),
                sf::Vertex(vectorpoint2)
            };

            line[0].color = sf::Color(255, 255, 255, 100);
            line[1].color = sf::Color(255, 255, 255, 100);

            window.draw(line, 2, sf::Lines);

            if (symAxisPoints.size() == 0)
                continue;

            auto vectorpoin1resultSym = drawScale(LineSym.getSymNode(nodepoint1));
            sf::Vector2f vectorpoint1Sym = sf::Vector2f(vectorpoin1resultSym.first, vectorpoin1resultSym.second);

            auto vectorpoin2resultSym = drawScale(LineSym.getSymNode(nodepoint2));
            sf::Vector2f vectorpoint2Sym = sf::Vector2f(vectorpoin2resultSym.first, vectorpoin2resultSym.second);

            sf::Vertex lineSym[] =
            {
                sf::Vertex(vectorpoint1Sym),
                sf::Vertex(vectorpoint2Sym)
            };

            lineSym[0].color = sf::Color(255, 255, 255, 100);
            lineSym[1].color = sf::Color(255, 255, 255, 100);

            window.draw(lineSym, 2, sf::Lines);
        }
        
        sf::Text text;
        sf::Font font;
        if (!font.loadFromFile("ArialRegular.ttf"))
        {
            return ;
        }
        text.setFont(font);
        text.setCharacterSize(16);
        text.setFillColor(sf::Color::Red);

        for (int i = 0; i < nodes.size(); ++i)
        {
            sf::CircleShape shape(1.f);
            auto shaperesult = drawScale(nodes[i]);
            auto textresult = drawScale(nodes[i]);
            shape.setPosition(shaperesult.first, shaperesult.second);
            shape.setFillColor(sf::Color::Green);
            if (detailTypeNum == wp)
                shape.setFillColor(sf::Color::Blue);
            text.setString(std::to_string(i));
            text.setPosition(textresult.first, textresult.second);
            //window.draw(text);
            //window.draw(shape);
        }
        return ;
    };


    void symAxisInizialisation()
    {
        if (symAxisPoints.size() == 0)
            return;

        float xzSum = 0, xSum = 0, zSum = 0, x2Sum = 0, n = 0;

        for (int idElem : symAxisPoints)
        {
            auto elem = nodes[idElem];

            xzSum += elem.x * elem.z;
            xSum += elem.x;
            zSum += elem.z;
            x2Sum += elem.x * elem.x;
            ++n;
        }

        LineSym = LineSymStruct(xzSum, xSum, zSum, x2Sum, n);
    };


    void intersectionSpace(sf::RenderWindow& window, Reader &otherDetail, unsigned detailTypeValue = tool)
    {
        bool StartSpaceContour = false;
        bool startRot = true;
        auto connectWish = detailTypeValue == tool + 1 ? connect1 : connect;

        std::vector<std::pair<std::list<int>, Segment>> connectSpaceWish;

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
                        connectSpaceWish.push_back(std::make_pair( std::list<int>{(*std::next(it, -1))->id},
                            Segment(*otherDetail.contour[otherDetail.nodes[connectWish[(*std::next(it, -1))->id].second].placeInContour],
                                    *otherDetail.contour[otherDetail.nodes[connectWish[(*std::next(it, -1))->id].second].placeInContour] )));
                    else
                        connectSpaceWish.push_back(std::make_pair(std::list<int>{(*std::next(it, -1))->id},
                            Segment(*otherDetail.contour[otherDetail.nodes[4].placeInContour],
                                *otherDetail.contour[otherDetail.nodes[4].placeInContour]) ));
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
                        connectSpaceWish[connectSpaceWish.size() - 1].second.n1 = otherDetail.contour[otherDetail.nodes[connectWish[(*it)->id].second].placeInContour];
                    else
                        connectSpaceWish[connectSpaceWish.size() - 1].second.n2 = otherDetail.contour[otherDetail.nodes[4].placeInContour];
                }
            }
        }
        
        if (detailTypeValue != tool)
            connectSpaceWish.pop_back();
        else
            connectSpaceWish.erase(connectSpaceWish.begin());

        if (detailTypeValue == tool + 1)
            std::copy(connectSpaceWish.begin(), connectSpaceWish.end(), std::back_inserter(connectSpace1));
        else
            std::copy(connectSpaceWish.begin(), connectSpaceWish.end(), std::back_inserter(connectSpace));
    }


    bool intersectionPart(Segment pointOfConnect, std::pair<int, int> connectPointOtherDetai)
    {
        int segment1Begin = pointOfConnect.n1->placeInContour;
        int segment1End = pointOfConnect.n2->placeInContour;
        if ((segment1End - segment1Begin) < 0)
        {
            std::swap(segment1End, segment1Begin);
        }
        int segment2Begin = connectPointOtherDetai.first;
        int segment2End = connectPointOtherDetai.second;

        if (segment1Begin > segment2End or segment1End < segment2Begin)
            return false;
        return true;

    };


    void intersection(Reader& otherDetail, sf::RenderWindow& window, unsigned detailTypeValue)
    {
        std::vector<std::pair<std::list<int>, int>> pointOfConnection = {};
        std::vector<std::pair<std::list<int>, Segment>> connectSpaceWish = detailTypeValue == tool + 1 ? connectSpace1 : connectSpace;

        pointOfConnection.reserve(connectSpaceWish.size());

        for (int n = 0; n < connectSpaceWish.size(); ++n)
            pointOfConnection.push_back(std::make_pair(std::list<int>{}, 0));

        for (int i = 0; i < connectSpaceWish.size(); ++i)
        { 
            auto elem = connectSpaceWish[i];
            for (int j = 0; j < otherDetail.connectSpace.size(); ++j)
            {
                auto elem1 = otherDetail.connectSpace[j];

                if (intersectionPart(elem.second, std::make_pair(otherDetail.nodes[*elem1.first.begin()].placeInContour, otherDetail.nodes[*std::prev(elem1.first.end())].placeInContour )))
                {
                    pointOfConnection[i].first.push_back(j);
                    pointOfConnection[i].second += elem1.first.size();
                }
            }
        }
        sf::Text text;
        sf::Font font;
        if (!font.loadFromFile("ArialRegular.ttf"))
        {
            return;
        }
        text.setFont(font);
        text.setCharacterSize(16);
        text.setFillColor(sf::Color::Red);

        for (int i = 0; i < connectSpaceWish.size(); ++i)
        {
            float sumSquare = 0.0f;
            sf::Vector2f vectorpoint1, vectorpoint2;

            auto vectorpoin1result = drawScale(nodes[*connectSpaceWish[i].first.begin()]);
            vectorpoint1 = sf::Vector2f(vectorpoin1result.first, vectorpoin1result.second);

            for (auto elem : connectSpaceWish[i].first)
            {
                auto vectorpoin2result = drawScale(nodes[elem]);

                vectorpoint2 = sf::Vector2f(vectorpoin2result.first, vectorpoin2result.second);

                sf::Vertex line[] =
                {
                    sf::Vertex(vectorpoint1),
                    sf::Vertex(vectorpoint2)
                };

                sumSquare += 0.5 * (vectorpoint1.x * vectorpoint2.y - vectorpoint2.x * vectorpoint1.y);

                window.draw(line, 2, sf::Lines);

                vectorpoint1 = vectorpoint2;
            }

            if (pointOfConnection[i].second > 0)
                for (int idelem : pointOfConnection[i].first)
                {
                    for (auto elem1 = otherDetail.contour[otherDetail.nodes[*otherDetail.connectSpace[idelem].first.begin()].placeInContour];
                        elem1 != std::next(otherDetail.contour[otherDetail.nodes[*std::prev(otherDetail.connectSpace[idelem].first.end())].placeInContour]); ++elem1)
                    {
                        if (connectSpaceWish[i].second.n2->placeInContour - elem1->placeInContour < 0)
                            continue;

                        if (elem1->placeInContour - connectSpaceWish[i].second.n1->placeInContour < 0)
                            continue;

                        auto vectorpoin2result = drawScale(*elem1);
                        vectorpoint2 = sf::Vector2f(vectorpoin2result.first, vectorpoin2result.second);

                        sf::Vertex line[] =
                        {
                            sf::Vertex(vectorpoint1),
                            sf::Vertex(vectorpoint2)
                        };

                        sumSquare += 0.5 * (vectorpoint1.x * vectorpoint2.y - vectorpoint2.x * vectorpoint1.y);

                        window.draw(line, 2, sf::Lines);

                        vectorpoint1 = vectorpoint2;
                    }
                }
            else
            {

                if (connectSpaceWish[i].second.n2->placeInContour > connectSpaceWish[i].second.n1->placeInContour)
                    connectSpaceWish[i].second.swap();

                for (auto elem1 = &otherDetail.contour[connectSpaceWish[i].second.n2->placeInContour]; elem1 != std::next(&otherDetail.contour[connectSpaceWish[i].second.n1->placeInContour]); ++elem1)
                {

                    std::cout << (*elem1)->id << std::endl;

                    auto vectorpoin2result = drawScale((**elem1));
                    vectorpoint2 = sf::Vector2f(vectorpoin2result.first, vectorpoin2result.second);

                    sf::Vertex line[] =
                    {
                        sf::Vertex(vectorpoint1),
                        sf::Vertex(vectorpoint2)
                    };

                    sumSquare += 0.5 * (vectorpoint1.x * vectorpoint2.y - vectorpoint2.x * vectorpoint1.y);

                    window.draw(line, 2, sf::Lines);

                    vectorpoint1 = vectorpoint2;
                }
            }
            vectorpoint2 = sf::Vector2f(vectorpoin1result.first, vectorpoin1result.second);

            sf::Vertex line[] =
            {
                sf::Vertex(vectorpoint1),
                sf::Vertex(vectorpoint2)
            };

            sumSquare += 0.5 * (vectorpoint1.x * vectorpoint2.y - vectorpoint2.x * vectorpoint1.y);

            window.draw(line, 2, sf::Lines);

            text.setString(std::to_string(abs(sumSquare)));
            text.setPosition(vectorpoin1result.first + 50, vectorpoin1result.second + 50);
            window.draw(text);
        }
    }
};
int main()
{
    sf::RenderWindow window(sf::VideoMode(960 * 1.5, 540 * 1.5), "SFML works!");
    sf::View view(sf::FloatRect(0, 0, 960 * 1.5, 540 * 1.5));
    window.setView(view);
    
    int time = 1;
    while (window.isOpen())
    {
        std::stringstream ss;
        ss << std::setw(3) << std::setfill('0') << time;
        Reader fistFigure("../data(1)/" + ss.str() + "-t1.csv2d"),
           secondFigure("../data(1)/" + ss.str() + "-t2.csv2d", tool+1),
            wpFigure("../data(1)/" + ss.str() + "-wp.csv2d", wp);
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
            {

                if (event.mouseWheelScroll.delta > 0)
                    view.zoom(0.93);
                else
                    view.zoom(1.03);
                window.setView(view);
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                {
                    view.move(-50.0f, 0.0f);
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                {
                    view.move(50.0f, 0.0f);
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                {
                    view.move(0.0f, -50.0f);
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                {
                    view.move(0.0f, 50.0f);
                }

                window.setView(view);
            }
        }

        window.clear();

        fistFigure.symAxisInizialisation();
        secondFigure.symAxisInizialisation();
        wpFigure.symAxisInizialisation();

        fistFigure.intersectionSpace(window, wpFigure, wp);
        secondFigure.intersectionSpace(window, wpFigure, wp);
        wpFigure.intersectionSpace(window, fistFigure, tool);
        wpFigure.intersectionSpace(window, secondFigure, tool + 1);

        wpFigure.intersection(fistFigure, window, tool);
        wpFigure.intersection(secondFigure, window, tool + 1);

        fistFigure.draw(window);
        secondFigure.draw(window);
        wpFigure.draw(window);

        sf::sleep(sf::milliseconds(300*1));

        window.display();

        if (time++ == 113)
            time = 1;
    }

    return 0;
}