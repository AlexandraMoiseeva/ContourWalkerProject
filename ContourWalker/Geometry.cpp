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


std::vector<CM_Cavity2D> Workpiece::intersectionSpace(Tool& otherDetail)
{
    std::vector<CM_Cavity2D> cavitys = {};
    Contour contourWP;
    Contour contourTool;
    int detailTypeValue = otherDetail.detail_type.detail_id;
    bool StartSpaceContour = false;
    int cavityCount = 0;
    int dopusk = 0;

    for (auto it = contour.begin() + 1; it != contour.end(); ++it)
    {
        if (std::next(contactInit.begin(), (*it)->sourceObjInfo.mesh_obj_id)->first != detailTypeValue && !(*it)->isSym)
        {
            if (StartSpaceContour == false)
            {
                if (contactInit[(*std::prev(it))->sourceObjInfo.mesh_obj_id].first == detailTypeValue)
                {
                    contourWP = Contour(&*(it - 1));
                    contourTool = Contour(contact[(*(it - 1))->sourceObjInfo.mesh_obj_id].second_point);
                }
                else
                    if ((*(it - 1))->isSym)
                    {
                        contourWP = Contour(&*(it - 1));
                        contourTool = Contour();
                    }
                    else
                    {
                        ++dopusk;
                        continue;
                    }

                StartSpaceContour = true;
                ++cavityCount;
            }
            else
            {
                contourWP.push_back(&*(it - 1));
            }
        }
        if (contactInit[(*it)->sourceObjInfo.mesh_obj_id].first == detailTypeValue || (*it)->isSym)
        {
            if (StartSpaceContour == true)
            {
                StartSpaceContour = false;

                contourWP.push_back(&*(it - 1));
                contourWP.push_back(&*it);

                if (std::next(contactInit.begin(), (*it)->sourceObjInfo.mesh_obj_id)->first == detailTypeValue)
                {
                    if (contourTool.beginNode != std::numeric_limits<int>::max())
                    {
                        Node** it2 = std::prev(*contourTool.begin());
                        Node** it3 = contact[(*it)->sourceObjInfo.mesh_obj_id].first_point;

                        for (int it1 = 0; it1 != otherDetail.contour.size(); ++it1)
                        {
                            contourTool.push_front((it2 - &*otherDetail.contour.begin()) >= it1 ? (it2 - it1)
                                : &*(otherDetail.contour.end() - (it1 + 1 - (it2 - &*otherDetail.contour.begin()))));
                            if (*contourTool.begin() == it3)
                                break;
                        }
                    }
                    else
                    {
                        Node** it1 = contact[(*it)->sourceObjInfo.mesh_obj_id].first_point;

                        contourTool.push_front(it1);

                        while (it1 - &*otherDetail.contour.begin() != otherDetail.contour.size())
                        {
                            ++it1;
                            contourTool.push_back(it1);
                            if ((*it1)->isSym)
                                break;
                        }
                    }

                    cavitys.emplace_back(
                        detail_type.detail_id,
                        detailTypeValue,
                        contourWP,
                        contourTool);
                }
                else
                {
                    if (contourTool.beginNode != std::numeric_limits<int>::max())
                    {
                        Node** it1 = *contourTool.begin();

                        contourTool.push_back(it1);

                        while (it1 - &*otherDetail.contour.begin() != 0)
                        {
                            --it1;
                            contourTool.push_front(it1);
                            if ((*it1)->isSym)
                                break;
                        }

                        cavitys.emplace_back(
                            detail_type.detail_id,
                            detailTypeValue,
                            contourWP,
                            contourTool);
                    }
                    else
                    {
                        --cavityCount;
                    }
                }
            }
        }
    }

    if (cavityCount == 0)
        return {};

    if (dopusk != 0)
    {
        for (auto it = contour.begin(); it != contour.begin() + dopusk + 1; ++it)
        {
            contourWP.push_back(&*it);
        }

        Node** it2 = *contourTool.begin();
        Node** it3 = contact[(*(contour.begin() + dopusk + 1))->sourceObjInfo.mesh_obj_id].first_point;

        for (int it1 = 1; it1 != otherDetail.contour.size(); ++it1)
        {
            contourTool.push_front((it2 - &*otherDetail.contour.begin()) >= it1 ? (it2 - it1)
                : &*(otherDetail.contour.end() - (it1 - (it2 - &*otherDetail.contour.begin()))));
            if (*contourTool.begin() == it3)
                break;
        }

        cavitys.emplace_back(
            detail_type.detail_id,
            detailTypeValue,
            contourWP,
            contourTool);
    }

    std::vector<CM_Cavity2D>::iterator maxSquareIterator = std::prev(cavitys.end());
    double maxSquare = 0;

    for (std::vector<CM_Cavity2D>::iterator elem = cavitys.begin(); elem != cavitys.end(); ++elem)
    {
        elem->intersection(contour, otherDetail.contour);

        if (elem->spaceSquare > maxSquare)
        {
            maxSquare = elem->spaceSquare;
            maxSquareIterator = elem;
        }
    }

    cavitys.erase(maxSquareIterator);

    return cavitys;
}