#include "Geometry.h"
#include <stdexcept>
#include <set>


cavity2d::Body::Body(int node_amount, detailType type_value, int id_value)
    : type(type_value), id(id_value)
{
    nodes.reserve(node_amount);
    contour.reserve(node_amount + 1);
};


std::vector<Node*>::iterator cavity2d::Body::begin()
{
    return contour.begin();
};


std::vector<Node*>::iterator cavity2d::Body::end()
{
    return contour.end();
};


void cavity2d::Body::addNode(Node nodeValue)
{
    nodes.push_back(nodeValue);
};


Node& cavity2d::Body::getNode(int nodeId)
{
    if ((nodeId > -1) && (nodeId < nodes.size()))
        return nodes[nodeId];
    else
    {
        throw std::out_of_range("");
    }
};


void cavity2d::Body::addContour(int idNode)
{
    contour.push_back(&nodes[idNode]);
};


void cavity2d::Tool::addSymAxisPoint(int point)
{
    symAxisPoints.push_back(point);
};


void cavity2d::Tool::addContact(std::pair<int, Edge> contactPoint)
{
    contactInit.push_back(contactPoint);
};


void cavity2d::Tool::inizialisation()
{
    symAxisInizialisation(symAxisPoints);
};


void cavity2d::Tool::symAxisInizialisation(std::vector<int>& symAxisPoints)
{
    if (symAxisPoints.empty())
        return;

    double xzSum = 0;
    double xSum = 0;
    double zSum = 0;
    double x2Sum = 0;
    double n = 0;

    for (int idElem : symAxisPoints)
    {
        Node elem = *std::next(nodes.begin(), idElem);
        xzSum += elem.coordinate.x * elem.coordinate.z;
        xSum += elem.coordinate.x;
        zSum += elem.coordinate.z;
        x2Sum += elem.coordinate.x * elem.coordinate.x;
        ++n;
    }

    lineSym = LineSymStruct(xzSum, xSum, zSum, x2Sum, n);
};


cavity2d::Tool::Tool(int nodeAmount, int detail_id_value) : Tool(nodeAmount, detailType::tool, detail_id_value) {};


cavity2d::Tool::Tool(int nodeAmount, detailType detail_type_value, int detail_id_value) : Body(nodeAmount, detail_type_value, detail_id_value)
{
    contactInit.reserve(nodeAmount);
    contact.reserve(nodeAmount);
};


cavity2d::CM_Cavity2D::CM_Cavity2D(const Contour& contourWPValue, const Contour& contourToolValue) :
    Body(contourToolValue.size() + contourWPValue.size(), detailType::cavity, -1),
    contourTool(contourToolValue), contourWP(contourWPValue) 
{
    for (auto const& elem : *this)
        nodes.push_back(*elem);

    for (auto const& elem : *this)
        contour.push_back(elem);
};


void cavity2d::CM_Cavity2D::intersection()
{
    double sumSquare = 0.0f;

    Node point0;
    Node point1;
    Node point2;

    for (auto const& point : *this)
    {
        if (point0 == Node())
        {
            point0 = *point;
            point1 = *point;

            continue;
        }

        point2 = *point;

        sumSquare += 0.5 * (point1.coordinate.x * point2.coordinate.z - point2.coordinate.x * point1.coordinate.z);

        point1 = point2;
    }

    sumSquare += 0.5 * (point1.coordinate.x * point0.coordinate.z - point0.coordinate.x * point0.coordinate.z);

    spaceSquare = abs(sumSquare);
};


bool cavity2d::CM_Cavity2D::colocationSpaceArea(CM_Cavity2D& lastSpaceArea)
{
    if (isContourIntersection(lastSpaceArea.contourTool))
    {
        id = lastSpaceArea.id;
        return true;
    }

    return false;
};


bool cavity2d::CM_Cavity2D::isContourIntersection(Contour& otherDetail) const
{
    std::set<const Node*> set1(contour.cbegin(), contour.cend());

    for(const auto& elem : otherDetail)
        if (set1.count(elem))
            return true;
    return false;
};


cavity2d::Workpiece::Workpiece(int nodeAmount, int detail_id_value) : Tool(nodeAmount, detailType::workpiece, detail_id_value) {};


void cavity2d::Workpiece::contactInizialisation(std::vector<Tool>& details)
{
    for (auto const& elem : contactInit)
    {
        if (elem.first == std::numeric_limits<int>::max())
            contact.push_back(EdgeNode());
        else
        {
            auto tool = &details[elem.first + (elem.first == 0 ? 0 : -1)];

            auto it1 = std::find_if(tool->begin(), tool->end(),
                [=, *this](Node* node)
                {
                    return *node == tool->getNode(elem.second.n1);
                });

            auto it2 = std::find_if(tool->begin(), tool->end(),
                [=, *this](Node* node)
                {
                    return *node == tool->getNode(elem.second.n2);
                });

            contact.push_back(EdgeNode(&*it1, &*it2));
        }
    }
};
