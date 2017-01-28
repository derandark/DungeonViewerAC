
#pragma once
#include "SortCell.h"

class CPolygon;

class CLandCell : public CSortCell
{
public:
    CLandCell();
    ~CLandCell();

    BOOL        IsInView(void);

    CPolygon**    m_Polys;        // 0xF0
    BOOL        m_InView;        // 0xF4
};