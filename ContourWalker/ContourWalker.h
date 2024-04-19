#pragma once

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


    Node(float xValue, float zValue)
    {
        x = xValue;
        z = zValue;
    };

    Node(int idValue, float xValue, float zValue)
    {
        id = idValue;
        x = xValue;
        z = zValue;
    };

};


class Segment
{
public:
    Node* n1;
    Node* n2;

    float A, B, C;

    Segment(Node& n1Value, Node& n2Value)
    {
        n1 = new Node{ n1Value };
        n2 = new Node{ n2Value };

        A = (n2->z - n1->z);
        B = (n1->x - n2->x);
        C = (n1->z * n2->x - n2->z * n1->x);
    };


    Node proection(Node n)
    {
        if (A * A + B * B != 0)
            return Node((B * B * n.x - C * A - A * B * n.z) / (B * B + A * A),
                ((B * B * n.x - C * A - A * B * n.z) / (B * B + A * A)) * A / (-B) - C / B);
        return n;
    };


    float inArea(Node* n)
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


    Node SegmentCross(Segment s1)
    {
        float det = A * s1.B - s1.A * B;
        if (det == 0)
            return *n1;
        return Node(-(C * s1.B - s1.C * B) / det, -(A * s1.C - s1.A * C) / det);
    };


    void swap()
    {
        auto temp = n1;
        n1 = n2;
        n2 = temp;
    };
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