#include "Geometry.h"


DetailInit::DetailInit(int nodeAmount, detailType detail_type_value, int detail_id_value)
    : detail_type(detail_type_value, detail_id_value) 
{
    nodes.reserve(nodeAmount);
    contour.reserve(nodeAmount);
    contactInit.reserve(nodeAmount);
    contact.reserve(nodeAmount);
};


void DetailInit::addNode(Node nodeValue)
{
    nodes.push_back(nodeValue);
};


void DetailInit::addContour(int idNode)
{
    contour.push_back(&nodes[idNode]);
};


void DetailInit::addSymAxisPoint(int point)
{
    symAxisPoints.push_back(point);
};


void DetailInit::addContact(std::pair<int, Edge> contactPoint)
{
    contactInit.push_back(contactPoint);
};


void DetailInit::inizialisation()
{
    symAxisInizialisation(symAxisPoints);
};


void DetailInit::symAxisInizialisation(std::vector<int>& symAxisPoints)
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


Tool::Tool(int nodeAmount, detailType detail_type_value, int detail_id_value) : DetailInit(nodeAmount, detail_type_value, detail_id_value) {};


Workpiece::Workpiece(int nodeAmount, detailType detail_type_value, int detail_id_value) : DetailInit(nodeAmount, detail_type_value, detail_id_value) {};


void Workpiece::contactInizialisation(std::vector<Tool>& details)
{
    for (auto const& elem : contactInit)
    {
        if (elem.first == std::numeric_limits<int>::max())
            contact.push_back(EdgeNode());
        else
        {
            auto tool = &details[elem.first + (elem.first == 0 ? 0 : -1)];

            auto it1 = std::find_if(tool->contour.begin(), tool->contour.end(),
                [=, *this](Node* node)
                {
                    return *node == tool->nodes[elem.second.n1];
                });

            auto it2 = std::find_if(tool->contour.begin(), tool->contour.end(),
                [=, *this](Node* node)
                {
                    return *node == tool->nodes[elem.second.n2];
                });

            contact.push_back(EdgeNode(&*it1, &*it2));
        }
    }
};


std::vector<SpaceArea> Workpiece::intersectionSpace(Tool& otherDetail)
{
    std::vector<SpaceArea> spaceAreas = {};
    int detailTypeValue = otherDetail.detail_type.detail_id;
    bool StartSpaceContour = false;
    int spaceAreaCount = 0;
    int dopusk = 0;

    for (auto it = contour.begin() + 1; it != contour.end(); ++it)
    {
        if (std::next(contactInit.begin(), (*it)->sourceObjInfo.mesh_obj_id)->first != detailTypeValue && !(*it)->isSym)
        {
            if (StartSpaceContour == false)
            {
                if (contactInit[(*std::prev(it))->sourceObjInfo.mesh_obj_id].first == detailTypeValue)
                {
                    spaceAreas.emplace_back(
                        detail_type.detail_id,
                            detailTypeValue,
                            Contour(&*(it - 1)),
                            Contour(contact[(*(it - 1))->sourceObjInfo.mesh_obj_id].second_point));
                }
                else
                    if ((*(it - 1))->isSym)
                    {
                        spaceAreas.emplace_back(
                            detail_type.detail_id,
                                detailTypeValue,
                                Contour(&*(it - 1)),
                                Contour());
                    }
                    else
                    {
                        ++dopusk;
                        continue;
                    }

                StartSpaceContour = true;
                ++spaceAreaCount;
            }
            else
            {
                spaceAreas.back().contourWP.push_back(&*(it - 1));
            }
        }
        if (contactInit[(*it)->sourceObjInfo.mesh_obj_id].first == detailTypeValue || (*it)->isSym)
        {
            if (StartSpaceContour == true)
            {
                StartSpaceContour = false;

                spaceAreas.back().contourWP.push_back(&*(it - 1));
                spaceAreas.back().contourWP.push_back(&*it);

                if (std::next(contactInit.begin(), (*it)->sourceObjInfo.mesh_obj_id)->first == detailTypeValue)
                {
                    if (spaceAreas.back().contourTool.beginNode != std::numeric_limits<int>::max())
                    {
                        Node** it2 = std::prev(*(spaceAreas.back()).contourTool.begin());
                        Node** it3 = contact[(*it)->sourceObjInfo.mesh_obj_id].first_point;

                        for (int it1 = 0; it1 != otherDetail.contour.size(); ++it1)
                        {
                            spaceAreas.back().contourTool.push_front((it2 - &*otherDetail.contour.begin()) >= it1 ? (it2 - it1)
                                : &*otherDetail.contour.end() - (it1 - (it2 - &*otherDetail.contour.begin())));
                            if (*spaceAreas.back().contourTool.begin() == it3)
                                break;
                        }
                    }
                    else
                    {
                        Node** it1 = contact[(*it)->sourceObjInfo.mesh_obj_id].first_point;

                        spaceAreas.back().contourTool.push_front(it1);

                        while (it1 - &*otherDetail.contour.begin() != otherDetail.contour.size())
                        {
                            ++it1;
                            spaceAreas.back().contourTool.push_back(it1);
                            if ((*it1)->isSym)
                                break;
                        }
                    }
                }
                else
                {
                    if (spaceAreas.back().contourTool.beginNode != std::numeric_limits<int>::max())
                    {
                        Node** it1 = *(spaceAreas.back()).contourTool.begin();

                        spaceAreas.back().contourTool.push_back(it1);

                        while (it1 - &*otherDetail.contour.begin() != 0)
                        {
                            --it1;
                            spaceAreas.back().contourTool.push_front(it1);
                            if ((*it1)->isSym)
                                break;
                        }
                    }
                    else
                    {
                        spaceAreas.pop_back();
                        --spaceAreaCount;
                    }
                }
            }
        }
    }

    if (spaceAreaCount == 0)
        return {};

    if (dopusk != 0)
    {
        spaceAreas.pop_back();
        /*
        for (auto it = std::next(contour.begin()); it != std::next(contour.begin()) + dopusk; ++it)
        {
            spaceAreas.back().contourWP.push_back(&*it);
        }

        Node** it2 = *(spaceAreas.back()).contourTool.begin();
        Node** it3 = contact[((*otherDetail.contour.begin() + dopusk))->sourceObjInfo.mesh_obj_id].first_point;

        for (auto it1 = it2; **it1 != **it3; --it1)
        {
            spaceAreas.back().contourTool.push_front(it1 < 0 ? otherDetail.contour.size() + it1 : it1);
        }
        */

    }

    std::vector<SpaceArea>::iterator maxSquareIterator = std::prev(spaceAreas.end());
    double maxSquare = 0;

    for (std::vector<SpaceArea>::iterator elem = spaceAreas.begin(); elem != spaceAreas.end(); ++elem)
    {
        elem->intersection(contour, otherDetail.contour);

        if (elem->spaceSquare > maxSquare)
        {
            maxSquare = elem->spaceSquare;
            maxSquareIterator = elem;
        }
    }

    spaceAreas.erase(maxSquareIterator);

    return spaceAreas;
}