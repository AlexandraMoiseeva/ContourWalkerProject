#include "GeometryInit.h"


Node::Node(double xValue, double zValue) : coordinate({ xValue, zValue }) {};


Node::Node(int idValue, int detailIdValue, double xValue, double zValue) : sourceObjInfo(detailIdValue, idValue), coordinate({ xValue, zValue }) {};


bool Node::operator == (Node n) const
{
    return sourceObjInfo.mesh_obj_id == n.sourceObjInfo.mesh_obj_id && sourceObjInfo.source_body_id == n.sourceObjInfo.source_body_id;
};


bool Node::operator != (Node n) const
{
    return !(sourceObjInfo.mesh_obj_id == n.sourceObjInfo.mesh_obj_id && sourceObjInfo.source_body_id == n.sourceObjInfo.source_body_id);
};


Contour::Contour(Node* nodeValue)
{
    contour.push_back(nodeValue);
};


void Contour::push_back(Node* nodeValue)
{
    contour.push_back(nodeValue);
};


void Contour::push_front(Node* nodeValue)
{
    contour.insert(contour.begin(), nodeValue);
};


void Contour::reserve(int amount)
{
    contour.reserve(amount);
};


size_t Contour::size() const
{
    return contour.size();
};


std::vector<Node*>::iterator Contour::begin()
{
    return contour.begin();
};


std::vector<Node*>::iterator Contour::end()
{
    return contour.end();
};


std::vector<Node*>::const_iterator Contour::cbegin() const
{
    return contour.cbegin();
};


std::vector<Node*>::const_iterator Contour::cend() const
{
    return contour.cend();
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
        linetype = lineDirection::other;

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