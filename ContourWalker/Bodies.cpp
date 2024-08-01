#include "stdafx.h"
#include "Bodies.h"

#include <stdexcept>
#include <set>

cavity2d::LineSymStruct::LineSymStruct(double xzSum, double xSum,
    double zSum, double x2Sum, double n)
{
    if (abs(n * x2Sum - xSum * xSum) < 1e-3)
    {
        a = xSum / n;
        linetype = lineDirection::vertical;
    }
    else
    {
        linetype = lineDirection::other;

        a = (n * xzSum - xSum * zSum) / (n * x2Sum - xSum * xSum);
        b = (zSum - a * xSum) / n;
    }
};


Node cavity2d::LineSymStruct::getSymNode(Node n) const
{
    if (linetype == lineDirection::vertical)
        return Node(-n.coordinate.x, n.coordinate.z);

    if (abs(a) < 1e-5)
        return Node(n.coordinate.x, 2 * b - n.coordinate.z);

    double x0 = (n.coordinate.z + (1.0f / a) * n.coordinate.x - b) /
        (a + (1.0f / a));
    double z0 = a * x0 + b;
    return (Node(2 * x0 - n.coordinate.x, 2 * z0 - n.coordinate.z));
};


cavity2d::Body::Body(int node_amount, detailType type, int id)
    : type(type), id(id)
{
    nodes.reserve(node_amount);
    contour.reserve(node_amount + 1);
};


Contour& cavity2d::Body::getContour()
{
    return contour;
};


void cavity2d::Body::addNode(Node node)
{
    nodes.push_back(node);
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


cavity2d::Tool::Tool(int nodeAmount, int id)
    : Tool(nodeAmount, detailType::tool, id) {};


cavity2d::Tool::Tool(int nodeAmount, detailType type, int id)
    : Body(nodeAmount, type, id)
{
    contactInit.resize(nodeAmount);
    std::fill(contactInit.begin(), contactInit.end(), Edge());
    contact.reserve(nodeAmount);
};


void cavity2d::Tool::addSymAxisPoint(int point)
{
    symAxisPoints.push_back(point);
};


void cavity2d::Tool::addContact(int nodeId, Edge contactPoint)
{
    contactInit[nodeId] = contactPoint;
};


void cavity2d::Tool::initialisation()
{
    symAxisInitialisation(symAxisPoints);
};


void cavity2d::Tool::symAxisInitialisation(std::vector<int>& symAxisPoints)
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





cavity2d::CM_Cavity2D::CM_Cavity2D(Contour& contourWP, Contour& contourTool) :
    Body(contourTool.size() + contourWP.size(), detailType::cavity, -1),
    contourTool(contourTool), contourWP(contourWP) 
{
    for (auto const& elem : contourWP)
        nodes.push_back(*elem);

    for (auto const& elem : contourTool)
        nodes.push_back(*elem);

    for (auto& elem : nodes)
        contour.push_back(&elem);

    contour.push_back(contour.front());
};


void cavity2d::CM_Cavity2D::squareCalculating()
{
    double sumSquare = 0.0;

    for (auto point = contour.begin() + 1;
        point != contour.end(); ++point)
    {
        sumSquare += 0.5 * ((*(point - 1))->coordinate.x *
            (*point)->coordinate.z -
            (*point)->coordinate.x *
            (*(point - 1))->coordinate.z);
    }

    sumSquare += 0.5 * (contour.front()->coordinate.x *
        contour.back()->coordinate.z -
        contour.back()->coordinate.x *
        contour.front()->coordinate.z);

    cavitySquare = abs(sumSquare);
};


bool cavity2d::CM_Cavity2D::colocationCavity(CM_Cavity2D& lastCavity)
{
    if (isContourIntersection(lastCavity.contour))
    {
        id = lastCavity.id;
        return true;
    }

    return false;
};


bool cavity2d::CM_Cavity2D::isContourIntersection(Contour& tool) const
{
    auto it = std::find_if(contour.cbegin(), contour.cend(),
        [&tool](const Node* node1) {
            return std::find_if(tool.cbegin(), tool.cend(),
            [node1](const Node* node2) {
                    return *node1 == *node2;
                }) != tool.end();
        });


    return it != contour.cend();
};


cavity2d::Workpiece::Workpiece(int nodeAmount, int id)
    : Tool(nodeAmount, detailType::workpiece, id) {};


void cavity2d::Workpiece::contactInitialisation(std::vector<Tool>& tools, std::map<int, int>& placeInVectorByIdTool)
{
    for (auto const& elem : contactInit)
    {
        if (elem.source_body_id == -1)
            contact.push_back(EdgeNode());
        else
        {
            auto tool = &tools[placeInVectorByIdTool[elem.source_body_id]];

            auto it1 = std::find_if(tool->getContour().begin(),
                tool->getContour().end(),
                [=, *this](Node* node)
                {
                    return *node == tool->getNode(elem.first_point);
                });

            auto it2 = std::find_if(tool->getContour().begin(),
                tool->getContour().end(),
                [=, *this](Node* node)
                {
                    return *node == tool->getNode(elem.second_point);
                });

            contact.push_back(EdgeNode(&*it1, &*it2));
        }
    }
};


cavity2d::EdgeNode cavity2d::Workpiece::getContactEdge(Node* node)
{
    return contact[node->sourceObjInfo.mesh_obj_id];
}