
#include "StdAfx.h"
#include "SortCell.h"

CSortCell::CSortCell()
{
    m_Building = NULL;
}

void CSortCell::add_building(CBuildingObj *building)
{
    if (!m_Building)
        m_Building = building;
}

void CSortCell::remove_building(void)
{
    m_Building = NULL;
}