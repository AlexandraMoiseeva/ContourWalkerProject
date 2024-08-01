#include "stdafx.h"

#include "BodiesSupport.h"


Node::Node(double x, double z) : coordinate({ x, z }) {};


Node::Node(int mesh_obj_id, int source_body_id, double x, double z)
    : sourceObjInfo(source_body_id, mesh_obj_id),
    coordinate({ x, z }) {};


bool Node::operator == (const Node& n) const
{
    return sourceObjInfo.mesh_obj_id == 
        n.sourceObjInfo.mesh_obj_id &&
        sourceObjInfo.source_body_id == 
        n.sourceObjInfo.source_body_id;
};


bool Node::operator != (const Node& n) const
{
    return !(*this == n);
};


Contour::Contour(Node* node)
{
    nodes.push_back(node);
};


void Contour::push_back(Node* node)
{
    nodes.push_back(node);
};


void Contour::push_front(Node* node)
{
    nodes.insert(nodes.begin(), node);
};


void Contour::reserve(int amount)
{
    nodes.reserve(amount);
};


size_t Contour::size() const
{
    return nodes.size();
};


Node* Contour::front()
{
    return nodes.front();
};


Node* Contour::back()
{
    return nodes.back();
};


std::vector<Node*>::iterator Contour::begin()
{
    return nodes.begin();
};


std::vector<Node*>::iterator Contour::end()
{
    return nodes.end();
};


std::vector<Node*>::const_iterator Contour::cbegin() const
{
    return nodes.cbegin();
};


std::vector<Node*>::const_iterator Contour::cend() const
{
    return nodes.cend();
};