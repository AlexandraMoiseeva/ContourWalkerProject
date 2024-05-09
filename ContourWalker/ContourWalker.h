#include "list"
#include "sstream"
#include "fstream"
#include "iomanip"

#include "DTW.hpp"

//Перечисление типов объектов на основе тех, что в #contacts
static enum DetailEnum { symAxis, tool1, tool2, wp};

//Перечисление типов строк в файле, ограничивающих данные; см.readerBikes
static enum ReaderBikesEnum { object, nodeTool, nodeWP, edge, bound, contacts };

//Перечисление моментот при считывание соответсвующих типов данных в файле
static enum ReaderSettingEnum { skip, readNodes, readEdge, readContacts };

//Типы прямых для оси симметрии
static enum lineDirectionn { vertical, other };

//Словарь сопоставлеения string значение с unsigned из ReaderBikesEnum
static std::map<std::string, unsigned> readerBikes
{
    {"node_id;x;z;T;volume", nodeTool},
    {"node_id;x;z;v_x;v_z;force_x;force_z;T;stress_mean;strain_plast;stress_eff;stress_flow;strain_rate;contact_dist;volume", nodeWP},
    {"#edge;0", edge},
    {"#bound;0", bound},
    {"#contacts", contacts}
};

//Словарь сопоставлеения string значение с unsigned из DetailEnum
static std::map<std::string, unsigned> detailType
{
    {"SymAxis", symAxis},
    {"Tool", tool1},
    {"Workpiece", wp}
};

static std::vector<std::string> detailTypeToString = { "symAxis", "Tool1", "Tool2", "Workpiece" };

/*
* Класс для определения узла
* Хранит:
*   placeInContour - место в векторе, определяющем контур
*   id - место в векторе, хранящем все вершины
*   x, z - соответсвующие координаты
*/
class Node
{
public:
    unsigned toolType = symAxis;
    unsigned placeInContour = std::numeric_limits<unsigned>::max();
    unsigned id = NULL;
    double x = NULL;
    double z = NULL;

    Node() {};


    Node(double xValue, double zValue)
    {
        x = xValue;
        z = zValue;
    };


    Node(int idValue, double xValue, double zValue)
    {
        id = idValue;
        x = xValue;
        z = zValue;
    };
};

/*
* Класс для определения отрезка
* Хранит:
*   *n1, *n2 - указатели на узлы, характеризующие отрезок
*/
class Segment
{
public:
    Node* n1;
    Node* n2;

    Segment(Node& n1Value, Node& n2Value)
    {
        n1 = new Node{ n1Value };
        n2 = new Node{ n2Value };
    };
};

/*
* Структура для определения оси симметрии
* Хранит:
*   a, b - переменные характеризующие прямую
*   тип прямой - вертикальная прямая - x = a
*              - любая другая прямая - y = a * x + b
*/
struct LineSymStruct
{
private:
    unsigned linetype = other;

    double a = NULL;
    double b = NULL;

public:
    LineSymStruct() {};

    //Определение идет с помощью МНК по полученым значения
    LineSymStruct(double xzSum, double xSum, double zSum, double x2Sum, double n)
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

    //Возвращает узел симметричный n
    Node getSymNode(Node n)
    {
        if (linetype == vertical)
            return Node(-1, -n.x, n.z);
        if (a == 0)
            return Node(-1, n.x, 2 * b - n.z);
        double x0 = (n.z + (1.0f / a) * n.x - b) / (a + (1.0f / a));
        double z0 = a * x0 + b;
        return (Node(-1, 2 * x0 - n.x, 2 * z0 - n.z));
    }

};

//Структура сопровождающая всю отрисовку
struct Drawer
{
public:
    /*
    * Возвращает координаты для экрана SFML.Задан в подобранном разрешене,
    * меняется только масштаб для детального рассмотрения
    */
    std::pair<double, double> drawScale(Node node, double scale = 5)
    {
        return std::make_pair(node.x * 1920 * scale + 50, (0.4 - node.z) * 1080 * scale);
    };


    std::pair<double, double> drawScale(std::vector<double> node, double scale = 5)
    {
        return std::make_pair(node[0] * 1920 * scale + 50, (0.4 - node[1]) * 1080 * scale);
    };

    /*
    * Отрисовывает отрезок на данных узлах
    * меняется только прозрачность. Все в белом цвете
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
* Класс, отвечающий за всю работу с одной из фигур Tool
* Хранит:
*   detailTypeNum - тип фигуры
*   nodes - вектор, хранящий узлы в порядке считыввания
*   contour - вектор, хранящий указатели на узлы в порядке, образующем контур 
*   symAxisPoints - хранит номера узлом, лежащих на оси симметрии
*   LineSym - ось симметрии, структуры LineSymStruct
*   connect - хранит словарь, размера количества узлов, где в соответсвие номеру узла
              ставится пара - номера узлов отрезка, которого он касается. 
              В случае Tool1 и Tool2 - касание с wp, в случае с wp - касание с Tool1.
              Если узел не касается, то в соответсвие ставится пара (-1, -1)
*   connect1 - хранит словарь, размера количества узлов, где в соответсвие номеру узла
              ставится пара - номера узлов отрезка, которого он касается. 
              В случае Tool1 и Tool2 - этот вектор пуст, в случае с wp - касание с Tool2.
              Если узел не касается, то в соответсвие ставится пара (-1, -1)
*/
class ContourWalkerTool
{
public:
    int detailTypeNum = tool1;
    std::vector<Node> nodes = {};
    std::vector<Node*> contour = {};

    std::list<unsigned> symAxisPoints = {};
    LineSymStruct LineSym;

    std::map<unsigned, std::pair<unsigned, unsigned> > connect = {};
    std::map<unsigned, std::pair<unsigned, unsigned> > connect1 = {};


    ContourWalkerTool() {};


    ContourWalkerTool(std::string filePathValue, int detailTypeValue = tool1)
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
                        node.toolType = detailTypeValue;

                        connect[id] = std::make_pair(-1, -1);
                        connect1[id] = std::make_pair(-1, -1);

                        nodes.push_back(node);
                        break;
                    }
                    case readEdge:
                    {
                        int numContour = std::stoi(s);
                        if (nodes[numContour].placeInContour == std::numeric_limits<unsigned>::max())
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
            //Удаляем последний элемент в контуре, т.к. они повторяются
            contour.pop_back();
        }
        file.close();
    };

    //Отрисовка контура
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

    //Определение оси симметрии
    void symAxisInizialisation()
    {
        if (symAxisPoints.size() == 0)
            return;

        double xzSum = 0, xSum = 0, zSum = 0, x2Sum = 0, n = 0;

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
};

/*
* Класс, отвечающий за всю работу с одной из фигур Tool
* Хранит:
*   detailTypeNum - тип фигуры
*   nodes - вектор, хранящий узлы в порядке считыввания
*   contour - вектор, хранящий указатели на узлы в порядке, образующем контур
*   symAxisPoints - хранит номера узлом, лежащих на оси симметрии
*   LineSym - ось симметрии, структуры LineSymStruct
*   connect - хранит словарь, размера количества узлов, где в соответсвие номеру узла
              ставится пара - номера узлов отрезка, которого он касается.
              Если узел не касается, то в соответсвие ставится пара (-1, -1)
*   connect1 - хранит словарь, размера количества узлов, где в соответсвие номеру узла
              ставится пара - номера узлов отрезка, которого он касается.
              Если узел не касается, то в соответсвие ставится пара (-1, -1)
*   connectSpace - хранит вектор пустых областей
                   - элемент - пара:
                                  1) Список вершин, рассматриваемой фигуры
                                  2) Пара - отрезок, которого касается первый и последний узел
*   connectSpace - хранит вектор пустых областей
                   - элемент - пара:
                                  1) Список вершин, рассматриваемой фигуры
                                  2) Пара - отрезок, которого касается первый и последний узел
*/
class ContourWalker : public ContourWalkerTool
{
public:
    std::vector<std::pair< std::list<unsigned>, std::pair<Segment, Segment> >> connectSpace = {};

    std::vector<std::pair< std::list<unsigned>, std::pair<Segment, Segment> >> connectSpace1 = {};


    ContourWalker() : ContourWalkerTool() {}


    ContourWalker(std::string filePathValue, int detailTypeValue = tool1) : ContourWalkerTool(filePathValue, detailTypeValue = tool1) {};

    //Определние соответсвующих connectSpace - нахождение контуров из некасающихся узлов и соответсвующих отрезков
    void intersectionSpace(ContourWalkerTool& otherDetail)
    {
        unsigned detailTypeValue = otherDetail.detailTypeNum;
        bool StartSpaceContour = false;
        bool startRot = true;
        auto connectWish = detailTypeValue == tool2 ? connect1 : connect;

        std::vector<std::pair< std::list<unsigned>, std::pair<Segment, Segment> >> connectSpaceWish;

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
                        connectSpaceWish.push_back(std::make_pair(std::list<unsigned>{(*std::next(it, -1))->id},
                            std::make_pair(
                                Segment(*otherDetail.contour[otherDetail.nodes[connectWish[(*std::prev(it))->id].first].placeInContour],
                                    *otherDetail.contour[otherDetail.nodes[connectWish[(*std::prev(it))->id].second].placeInContour]),
                                Segment(*otherDetail.contour[otherDetail.nodes[connectWish[(*std::prev(it))->id].first].placeInContour],
                                    *otherDetail.contour[otherDetail.nodes[connectWish[(*std::prev(it))->id].second].placeInContour]))));
                    else
                        connectSpaceWish.push_back(std::make_pair(std::list<unsigned>{(*std::next(it, -1))->id},
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

    //Построение контуров пустых областей и нахождение их площадей - формула площади Гаусса
    std::vector<std::pair<std::pair<unsigned, double>, std::pair<std::vector<unsigned>, std::vector<unsigned>>>> intersection(ContourWalkerTool& otherDetail, 
        std::vector<std::pair<std::pair<unsigned, double>, std::pair<std::vector<unsigned>, std::vector<unsigned>>>> lastSpaceArea)
    {
        unsigned detailTypeValue = otherDetail.detailTypeNum;

        std::vector<std::pair< std::list<unsigned>, std::pair<Segment, Segment> >> connectSpaceWish = detailTypeValue == tool2 ? connectSpace1 : connectSpace;
        std::vector<std::pair<std::pair<unsigned, double>, std::pair<std::vector<unsigned>, std::vector<unsigned>>>> spaceArea = {};
        for (int i = 0; i < connectSpaceWish.size(); ++i)
        {
            double sumSquare = 0.0f;
            Node point1, point2;

            point1 = nodes[*connectSpaceWish[i].first.begin()];

            spaceArea.push_back(std::make_pair(std::make_pair(1000, 0), std::make_pair(std::vector<unsigned> {}, std::vector<unsigned> {})));

            for (auto elem : connectSpaceWish[i].first)
            {
                point2 = nodes[elem];

                spaceArea[spaceArea.size() - 1].second.first.push_back(point2.id);

                sumSquare += 0.5 * (point1.x * point2.z - point2.x * point1.z);

                point1 = point2;
            }

            if (connectSpaceWish[i].second.first.n1->id != connectSpaceWish[i].second.first.n2->id and
                connectSpaceWish[i].second.second.n1->id != connectSpaceWish[i].second.second.n2->id)
                for (auto elem1 = std::next(&otherDetail.contour[connectSpaceWish[i].second.first.n1->placeInContour]);
                    elem1 != std::next(&otherDetail.contour[connectSpaceWish[i].second.second.n2->placeInContour]); ++elem1)
                    {
                        point2 = **elem1;

                        spaceArea[spaceArea.size() - 1].second.second.push_back(point2.id);

                        sumSquare += 0.5 * (point1.x * point2.z - point2.x * point1.z);

                        point1 = point2;
                    }
            else
                if (connectSpaceWish[i].second.first.n1->id != connectSpaceWish[i].second.first.n2->id)
                    for (auto elem1 = std::next(&otherDetail.contour[connectSpaceWish[i].second.first.n1->placeInContour]);
                        std::find(otherDetail.symAxisPoints.begin(), otherDetail.symAxisPoints.end(), (*std::prev(elem1))->id) == otherDetail.symAxisPoints.end(); ++elem1)
                    {
                        point2 = **elem1;

                        spaceArea[spaceArea.size() - 1].second.second.push_back(point2.id);

                        sumSquare += 0.5 * (point1.x * point2.z - point2.x * point1.z);

                        point1 = point2;
                    }
                else
                    if (connectSpaceWish[i].second.second.n1->id != connectSpaceWish[i].second.second.n2->id)
                        for (auto elem1 = std::next(&otherDetail.contour[connectSpaceWish[i].second.second.n2->placeInContour]);
                            std::find(otherDetail.symAxisPoints.begin(), otherDetail.symAxisPoints.end(), (*std::prev(elem1))->id) == otherDetail.symAxisPoints.end(); --elem1)
                        {
                            point2 = **elem1;

                            spaceArea[spaceArea.size() - 1].second.second.insert(spaceArea[spaceArea.size() - 1].second.second.begin(), point2.id);

                            sumSquare += 0.5 * (point1.x * point2.z - point2.x * point1.z);

                            point1 = point2;
                        }

            point2 = nodes[*connectSpaceWish[i].first.begin()];

            sumSquare += 0.5 * (point1.x * point2.z - point2.x * point1.z);

            spaceArea[i].first.second = abs(sumSquare);
        }
        
        //Попытка отслеживания контуров

        int idNumber = lastSpaceArea.size();

        for (int i = 0; i < spaceArea.size(); i++)
        {
            for (int j = 0; j < lastSpaceArea.size(); j++)
            {
                if (isContourIntersection(spaceArea[i].second.second, lastSpaceArea[j].second.second, otherDetail))
                    spaceArea[i].first.first = lastSpaceArea[j].first.first;
            }

            if (spaceArea[i].first.first == 1000)
                spaceArea[i].first.first = ++idNumber;
        }

        return spaceArea;
    }

private:
    bool isContourIntersection(std::vector<unsigned> oneContour,
        std::vector<unsigned> twoContour, ContourWalkerTool& otherDetail)
    {
        if (otherDetail.nodes[*oneContour.begin()].placeInContour > otherDetail.nodes[*std::prev(twoContour.end())].placeInContour)
            return false;
        if (otherDetail.nodes[*twoContour.begin()].placeInContour > otherDetail.nodes[*std::prev(oneContour.end())].placeInContour)
            return false;
        return true;
    };

};

/*
* Класс, отвечающий за решение задачи в общем
* Хранит:
*   ss - номер итерации в нужном формате
*   firstFigure - Tool1, в классе ContourWalker
*   secondFigure - Tool2, в классе ContourWalker
*   wpFigure - WP, в классе ContourWalker
*   spaceAreaTool1 - хранит пустые области Wp с Tool1, их площадь, их номер
*   spaceAreaTool2 - хранит пустые области Wp с Tool2, их площадь, их номер
*/
class CWM
{
private:
    std::stringstream ss;
    ContourWalkerTool firstFigure;
    ContourWalkerTool secondFigure;
    ContourWalker wpFigure;

    std::vector<std::pair<std::pair<unsigned, double>, std::pair<std::vector<unsigned>, std::vector<unsigned>>>> spaceAreaTool1 = {};
    std::vector<std::pair<std::pair<unsigned, double>, std::pair<std::vector<unsigned>, std::vector<unsigned>>>> spaceAreaTool2 = {};

    void writeInOut(std::ofstream& fileOutValue, std::pair<std::pair<unsigned, double>, std::pair<std::vector<unsigned>, std::vector<unsigned>>> elem,
        unsigned detailTypeValue, bool isSym = false)
    {
        fileOutValue << "Tool" + std::to_string(detailTypeValue) + "; " + "Square" + std::to_string(elem.first.first) + (isSym ? ".Sym" : "") + "\n";
        fileOutValue << "Square: " << std::setprecision(15) << elem.first.second << "\n";
        fileOutValue << "Object type; node id;\n";
        for (auto point : elem.second.first)
        {
            fileOutValue << detailTypeToString[wp] + "; " + std::to_string(point) + (isSym ? ".Sym" : "") + "\n";
        }
        for (auto point : elem.second.second)
        {
            fileOutValue << detailTypeToString[detailTypeValue] + "; " + std::to_string(point) + (isSym ? ".Sym" : "") + "\n";
        }
    }

    Node returnNode(unsigned detailValue, unsigned node)
    {
        switch (detailValue)
        {
        case wp:
            return wpFigure.nodes[node];
            break;

        case tool1:
            return firstFigure.nodes[node];
            break;

        case tool2:
            return secondFigure.nodes[node];
            break;

        default:
            break;
        }
    }

public:

    CWM(unsigned time)
    {
        ss << std::setw(3) << std::setfill('0') << time;

        firstFigure = ContourWalkerTool("../data(1)/" + ss.str() + "-t1.csv2d", tool1);
        secondFigure = ContourWalkerTool("../data(1)/" + ss.str() + "-t2.csv2d", tool2);
        wpFigure = ContourWalker("../data(1)/" + ss.str() + "-wp.csv2d", wp);
    };


    CWM(unsigned time, std::pair<std::vector<std::pair<std::pair<unsigned, double>, std::pair<std::vector<unsigned>, std::vector<unsigned>>>>,
        std::vector<std::pair<std::pair<unsigned, double>, std::pair<std::vector<unsigned>, std::vector<unsigned>>>>> spaceAreas)
    {
        ss << std::setw(3) << std::setfill('0') << time;

        firstFigure = ContourWalkerTool("../data(1)/" + ss.str() + "-t1.csv2d", tool1);
        secondFigure = ContourWalkerTool("../data(1)/" + ss.str() + "-t2.csv2d", tool2);
        wpFigure = ContourWalker("../data(1)/" + ss.str() + "-wp.csv2d", wp);

        spaceAreaTool1 = spaceAreas.first;
        spaceAreaTool2 = spaceAreas.second;
    };
    
    
    //Нахождние областей и запись в папку returnData
    void findSpace()
    {
        firstFigure.symAxisInizialisation();
        secondFigure.symAxisInizialisation();
        wpFigure.symAxisInizialisation();

        wpFigure.intersectionSpace(firstFigure);
        wpFigure.intersectionSpace(secondFigure);

        spaceAreaTool1 = wpFigure.intersection(firstFigure, spaceAreaTool1);
        spaceAreaTool2 = wpFigure.intersection(secondFigure, spaceAreaTool2);

        std::ofstream fileOut("../returnData/" + ss.str() + ".txt", std::ofstream::out | std::ofstream::trunc);

        if (fileOut.is_open())
        {
            for (auto elem : spaceAreaTool1)
            {
                writeInOut(fileOut, elem, tool1);
                /*if (firstFigure.symAxisPoints.size() != 0)
                    writeInOut(fileOut, elem, tool1, true);*/
            }

            for (auto elem : spaceAreaTool2)
            {
                writeInOut(fileOut, elem, tool2);
                /*if (secondFigure.symAxisPoints.size() != 0)
                    writeInOut(fileOut, elem, tool2, true);*/
            }
            fileOut.close();
        }
    }

    //Отрисовка пустых областей
    void drawSpace(sf::RenderWindow& window, unsigned toolNumber)
    {
        auto spaceArea = toolNumber == tool1 ? spaceAreaTool1 : spaceAreaTool2;

        sf::Text text;
        sf::Font font;

        if (!font.loadFromFile("ArialRegular.ttf"))
        {
            std::cerr << "Не удалось найти шрифт\n";
        }

        text.setFont(font);
        text.setCharacterSize(16);
        text.setFillColor(sf::Color::Red);

        for (auto elem: spaceArea)
        {
            auto point0 = elem.second.first[0];

            for (auto point = std::next(elem.second.first.begin()); point != elem.second.first.end(); point++)
            {
                Drawer().drawLine(returnNode(wp, point0), returnNode(wp, *point), window);

                point0 = *point;
            }

            if (elem.second.second.size() > 0)
            {
                Drawer().drawLine(returnNode(wp, point0), returnNode(toolNumber, elem.second.second[0]), window);

                point0 = elem.second.second[0];

                for (auto point = std::next(elem.second.second.begin()); point != elem.second.second.end(); point++)
                {
                    Drawer().drawLine(returnNode(toolNumber, point0), returnNode(toolNumber, *point), window);

                    point0 = *point;
                }

                Drawer().drawLine(returnNode(toolNumber, point0), returnNode(wp, elem.second.first[0]), window);
            }
            else
                Drawer().drawLine(returnNode(wp, point0), returnNode(wp, elem.second.first[0]), window);

            auto textPoint = Drawer().drawScale(returnNode(toolNumber, point0));
            text.setPosition(textPoint.first + 50, textPoint.second + 50);
            text.setString(std::to_string(elem.first.second));
            window.draw(text);

            text.setPosition(textPoint.first + 50, textPoint.second);
            text.setString(std::to_string(toolNumber) + '.' + std::to_string(elem.first.first));
            window.draw(text);
        }
    }

    //Отрисовка областей и контуров фигур
    void drawAll(sf::RenderWindow& window)
    {
        firstFigure.draw(window);
        secondFigure.draw(window);
        wpFigure.draw(window);
        
        drawSpace(window, tool1);
        drawSpace(window, tool2);
    }

    //Возвращает пару - пустые полости wp с Tool1 И Tool2
    std::pair<std::vector<std::pair<std::pair<unsigned, double>, std::pair<std::vector<unsigned>, std::vector<unsigned>>>>,
        std::vector<std::pair<std::pair<unsigned, double>, std::pair<std::vector<unsigned>, std::vector<unsigned>>>> > returnSpaceArea()
    {
        return std::make_pair(spaceAreaTool1, spaceAreaTool2);
    }
};