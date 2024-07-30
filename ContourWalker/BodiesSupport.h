#pragma once

#include <vector>
#include <string>

struct SourceObjInfo 
{
    SourceObjInfo() = default;

    SourceObjInfo(int source_body_id, int mesh_obj_id) noexcept
        : source_body_id{ source_body_id }
        , mesh_obj_id{ mesh_obj_id } {}

    SourceObjInfo(int source_body_id, int mesh_obj_id, void* src_ptr) noexcept
        : source_body_id{ source_body_id }
        , mesh_obj_id{ mesh_obj_id }
        , ptr{ src_ptr } {}

    int source_body_id = -1;
    int mesh_obj_id = -1;
    void* ptr = nullptr;
};


struct Coordinate
{
    double x = 0;
    double z = 0;
};


class Node
{
public:
    SourceObjInfo sourceObjInfo;

    bool isSym = false;

    Coordinate coordinate;
    

    Node() = default;


    Node(double x, double z);


    Node(int source_body_id, int mesh_obj_id, double x, double z);


    bool operator == (const Node& n) const;


    bool operator != (const Node& n) const;
};


class Contour
{
private:
    std::vector<Node*> nodes = {};

public:

    Contour() = default;


    Contour(Node* node);


    void push_back(Node* node);


    void push_front(Node* node);


    void reserve(int amount);


    size_t size() const;


    Node* front();


    Node* back();


    std::vector<Node*>::iterator begin();


    std::vector<Node*>::iterator end();


    std::vector<Node*>::const_iterator cbegin() const;


    std::vector<Node*>::const_iterator cend() const;
};