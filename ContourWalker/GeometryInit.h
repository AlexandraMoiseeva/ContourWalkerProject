#pragma once

#include <vector>
#include <vector>
#include <string>

enum class lineDirection { none, vertical, other };


enum class detailType { tool, workpiece, cavity };


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


    Node(double xValue, double zValue);


    Node(int idValue, int detailIdValue, double xValue, double zValue);


    bool operator == (Node n) const;


    bool operator != (Node n) const;
};


struct Edge
{
public:
    int n1 = 0; // Хранить указатели на узлы
    int n2 = 0;
};


class Contour
{
private:
    std::vector<Node*> contour = {};

public:

    Contour() = default;


    Contour(Node* nodeValue);


    void push_back(Node* nodeValue);


    void push_front(Node* nodeValue);


    void reserve(int amount);


    size_t size() const;


    std::vector<Node*>::iterator begin();


    std::vector<Node*>::iterator end();


    std::vector<Node*>::const_iterator cbegin() const;


    std::vector<Node*>::const_iterator cend() const;
};


struct LineSymStruct
{
private:
    
    double a = std::numeric_limits<int>::max();
    double b = std::numeric_limits<int>::max();

public:
    lineDirection linetype = lineDirection::none;


    LineSymStruct();


    LineSymStruct(double xzSum, double xSum, double zSum, double x2Sum, double n);


    Node getSymNode(Node n) const;
};

