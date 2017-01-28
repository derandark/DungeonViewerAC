
#pragma once
#include "ObjCell.h"

class CBuildingObj;

class CSortCell : public CObjCell
{
public:
    CSortCell();

    void add_building(CBuildingObj *building);
    void remove_building(void);

    CBuildingObj *m_Building; // 0xE8
};