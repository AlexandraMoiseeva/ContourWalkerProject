#pragma once

#include <vector>
#include <deque>
#include <string>

const enum class lineDirection { none, vertical, other };


const enum class detailType { tool, workpiece };

struct detailTypeValue
{
public:
    detailType detail_type = detailType::tool;
    int detail_id = -1;

    detailTypeValue() = default;

    detailTypeValue(detailType detail_type_value, int detail_id_value)
        : detail_type(detail_type_value),
        detail_id(detail_id_value) {};
};


class Node
{
public:
    struct SourceObjInfo {
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
    } sourceObjInfo;

    bool isSym = false;

    struct Coordinate
    {
        double x = 0;
        double z = 0;

        Coordinate() = default;


        Coordinate(double xValue, double zValue) : x(xValue), z(zValue) {};
    } coordinate;
    

    Node();


    Node(double xValue, double zValue);


    Node(int idValue, int detailIdValue, double xValue, double zValue);


    bool operator == (Node n) const;


    bool operator != (Node n) const;
};


class Edge
{
public:
    int n1 = 0;
    int n2 = 0;

    Edge();


    Edge(int n1Value, int n2Value);
};


class Contour
{
private:
    std::deque<Node**> contour = {};

    Node** beginNodeIt = nullptr;

public:
    int beginNode = std::numeric_limits<int>::max();
    int endNode = std::numeric_limits<int>::max();

    Contour();


    Contour(Node** beginNodeIt);


    Contour(Node** nodeValue, Node** beginNodeIt);


    void push_back(Node** nodeValue);


    void push_front(Node** nodeValue);


    size_t size() const;


    std::deque<Node**>::iterator begin();


    std::deque<Node**>::iterator end();


    std::deque<Node**>::const_iterator cbegin() const;


    std::deque<Node**>::const_iterator cend() const;
};


class CM_Cavity2D
{
protected:
    Contour contourTool;
    Contour contourWP;

public:
    int detailToolId = 0;
    int detailWPId = 0;

    int cavityId = std::numeric_limits<int>::max();

    double spaceSquare = 0;

    class Iterator
    {
    private:
        const CM_Cavity2D* cavity;
        std::deque<Node**>::const_iterator currentIterator;
        bool isVec1;

    public:
        Iterator(const CM_Cavity2D* cmc, bool isVec1) :
            cavity(cmc), isVec1(isVec1), currentIterator(isVec1 ? cmc->contourWP.cbegin()
                : cmc->contourTool.cend()) {};


        Iterator& operator++()
        {
            ++currentIterator;
            if (isVec1 && currentIterator == cavity->contourWP.cend())
            {
                isVec1 = false;
                currentIterator = cavity->contourTool.cbegin();
            }
            return *this;
        };


        Node** operator*() const
        {
            return *currentIterator;
        };


        bool operator!=(const Iterator& other) const
        {
            return isVec1 != other.isVec1 ||
                currentIterator != other.currentIterator;
        };

    };


    Iterator begin() const
    {
        return Iterator(this, true);
    };


    Iterator end() const
    {
        return Iterator(this, false);
    };

    CM_Cavity2D();

    
    CM_Cavity2D(int detailWPIdValue, int detailToolIdValue, const Contour contourWPValue, const Contour contourToolValue);

    
    void intersection();


    bool colocationSpaceArea(CM_Cavity2D& lastSpaceArea);

private:
    bool isContourIntersection(Contour& otherDetail) const;
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

