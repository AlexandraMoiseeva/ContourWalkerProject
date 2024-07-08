#include "GeometryInit.h"


Node::Node() = default;


Node::Node(double xValue, double zValue) : coordinate(xValue, zValue) {};


Node::Node(int idValue, int detailIdValue, double xValue, double zValue) : sourceObjInfo(detailIdValue, idValue), coordinate(xValue, zValue) {};


bool Node::operator == (Node n)
{
    return sourceObjInfo.mesh_obj_id == n.sourceObjInfo.mesh_obj_id && sourceObjInfo.source_body_id == n.sourceObjInfo.source_body_id;
};


bool Node::operator != (Node n)
{
    return !(sourceObjInfo.mesh_obj_id == n.sourceObjInfo.mesh_obj_id && sourceObjInfo.source_body_id == n.sourceObjInfo.source_body_id);
};


Edge::Edge() = default;


Edge::Edge(int n1Value, int n2Value) : n1(n1Value), n2(n2Value) {};


Contour::Contour() = default;


Contour::Contour(Node** nodeValue) : beginNode(0)
{
    contour.push_back(nodeValue);
};


void Contour::push_back(Node** nodeValue)
{
    contour.push_back(nodeValue);
    endNode = 0;
};


void Contour::push_front(Node** nodeValue)
{
    contour.push_front(nodeValue);
    if (endNode == std::numeric_limits<int>::max())
        endNode = beginNode;
    beginNode = 0;
}


size_t Contour::size() const
{
    return contour.size();
};


std::deque<Node**>::iterator Contour::begin()
{
    return contour.begin();
};


std::deque<Node**>::iterator Contour::end()
{
    return contour.end();
};


std::deque<Node**>::const_iterator Contour::cbegin() const
{
    return contour.cbegin();
};


std::deque<Node**>::const_iterator Contour::cend() const
{
    return contour.cend();
};


CM_Cavity2D::CM_Cavity2D() = default;


CM_Cavity2D::CM_Cavity2D(int detailWPIdValue, int detailToolIdValue, const Contour contourWPValue, const Contour contourToolValue) :
    detailToolId(detailToolIdValue), detailWPId(detailWPIdValue), contourTool(contourToolValue), contourWP(contourWPValue) {};


void CM_Cavity2D::intersection(std::vector<Node*>& cntrWP, std::vector<Node*>& cntrTool)
{
    double sumSquare = 0.0f;
    Node point1;
    Node point2;

    point1 = **cntrWP.begin();

    for (auto const& elem : contourWP)
    {
        point2 = **elem;

        sumSquare += 0.5 * (point1.coordinate.x * point2.coordinate.z - point2.coordinate.x * point1.coordinate.z);

        point1 = point2;
    }

    for (auto const& elem1 : contourTool)
    {
        point2 = **elem1;

        sumSquare += 0.5 * (point1.coordinate.x * point2.coordinate.z - point2.coordinate.x * point1.coordinate.z);

        point1 = point2;
    }

    point2 = **cntrWP.begin();

    sumSquare += 0.5 * (point1.coordinate.x * point2.coordinate.z - point2.coordinate.x * point1.coordinate.z);

    spaceSquare = abs(sumSquare);
};


bool CM_Cavity2D::colocationSpaceArea(CM_Cavity2D& lastSpaceArea)
{
    if (lastSpaceArea.detailToolId != detailToolId || lastSpaceArea.detailWPId != detailWPId)
        return false;

    if (isContourIntersection(lastSpaceArea.contourTool))
    {
        cavityId = lastSpaceArea.cavityId;
        return true;
    }

    return false;
};


bool CM_Cavity2D::isContourIntersection(Contour& otherDetail) const
{

    if (contourTool.beginNode > otherDetail.endNode && contourTool.endNode > contourTool.beginNode)
        return false;
    if (otherDetail.beginNode > contourTool.endNode && otherDetail.endNode > otherDetail.beginNode)
        return false;
    return true;
};



LineSymStruct::LineSymStruct() = default;


LineSymStruct::LineSymStruct(double xzSum, double xSum, double zSum, double x2Sum, double n)
{
    if (abs(n * x2Sum - xSum * xSum) < 1e-3)
    {
        a = xSum / n;
        linetype = lineDirection::vertical;
    }
    else
    {
        a = (n * xzSum - xSum * zSum) / (n * x2Sum - xSum * xSum);
        b = (zSum - a * xSum) / n;
    }
};


Node LineSymStruct::getSymNode(Node n) const
{
    if (linetype == lineDirection::vertical)
        return Node(-n.coordinate.x, n.coordinate.z);

    if (abs(a) < 1e-5)
        return Node(n.coordinate.x, 2 * b - n.coordinate.z);

    double x0 = (n.coordinate.z + (1.0f / a) * n.coordinate.x - b) / (a + (1.0f / a));
    double z0 = a * x0 + b;
    return (Node(2 * x0 - n.coordinate.x, 2 * z0 - n.coordinate.z));
}