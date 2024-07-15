#pragma once

#include <vector>
#include <string>

#include "GeometryInit.h"

struct EdgeNode
{
    Node** first_point = nullptr;
    Node** second_point = nullptr;

    EdgeNode() = default;


    EdgeNode(Node** first_point_value, Node** second_point_value)
        : first_point(first_point_value), second_point(second_point_value) {};

};


namespace cavity2d
{
    class Body
    {
    protected:
        std::vector<Node> nodes;
        Contour contour;

    public:

        detailType type = detailType::tool;
        int id = -1;

        Body() = default;


        Body(int node_amount, detailType type_value, int id_value);


        std::vector<Node*>::iterator begin();


        std::vector<Node*>::iterator end();


        void addNode(Node node_value);


        Node& getNode(int node_id);


        void addContour(int id_node);
    };


    class Tool : public Body
    {
    protected:
        Tool(int nodeAmount, detailType detail_type_value, int detail_id_value);

        std::vector<int> symAxisPoints;

    public:
        std::vector<std::pair<int, Edge>> contactInit;
        std::vector<EdgeNode> contact;


        LineSymStruct lineSym;


        Tool(int nodeAmount, int detail_id_value);


        void addSymAxisPoint(int point);


        void addContact(std::pair<int, Edge> contact_init);


        void inizialisation();

    protected:
        void symAxisInizialisation(std::vector<int>& symAxisPoints);
    };


    /*
    Заготовка, инструмент, полость - три типа сущностей с множеством общих свойств.
    Все состоят из узлов, имеют контур и т.д.
    Убрать все общее в базовый класс, продумать иерархию наследования (из коробки получим устранение замечания ниже).
    В частных классах реализовать уникальные методы для конкретного класса.
    Обновить диаграмму классов.
    */
    class CM_Cavity2D : public Body
    {
    private:
        Contour contourTool;
        Contour contourWP;

    public:
        double spaceSquare = 0;

        class Iterator
        {
        private:
            const CM_Cavity2D* cavity;
            std::vector<Node*>::const_iterator currentIterator;
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


            Node* operator*() const
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


        CM_Cavity2D(const Contour& contourWPValue, const Contour& contourToolValue);


        void intersection();


        bool colocationSpaceArea(CM_Cavity2D& lastSpaceArea);

    private:
        bool isContourIntersection(Contour& otherDetail) const;
    };


    class Workpiece : public Tool
    {
    public:

        Workpiece(int nodeAmount, int detail_id_value);


        void contactInizialisation(std::vector<Tool>& details);
    };
}