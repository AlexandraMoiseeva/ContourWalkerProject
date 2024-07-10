#pragma once

#include "Geometry.h"


/*
Переработать структуру и названия файлов.
Их название не соответствует содержанию.
*/


class CM_CavityModel2D // Все сущности перенести в namespace cavity2d
{
protected:
    CM_CavityModel2D();


    CM_CavityModel2D(int toolNumber, int wpNumber); // оставить один конструктор по умолчанию, инициализацию вынести в отдельный метод,
                                                    // а лучше добавить методы Tool
    std::vector<Tool> toolFigures; // Переименовать в tools/workpieces
    std::vector<Workpiece> wpFigures;

public:

    // Утечка памяти
    // найденные полости можно просто хранить в каком-нибудь контейнере
    // и добавлять по мере их нахождения
    std::vector<CM_Cavity2D>* cavitys = nullptr;


    std::vector<Tool>& getToolFigures();


    std::vector<Workpiece>& getWpFigures();


    void findSpace();


    void trackSpaceArea(std::vector<CM_Cavity2D>*& lastSpaceAreas);


    void contactInizialisation(); // Initialization
};