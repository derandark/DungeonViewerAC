
#include "StdAfx.h"
#include "LandDefs.h"
#include "LandCell.h"

CLandCell::CLandCell()
{
    m_Key = 0;
    m_lLinks = 0;

    m_Polys = new CPolygon*[ LandDefs::polys_per_landcell ];
    m_InView = FALSE;
}

CLandCell::~CLandCell()
{
    delete [] m_Polys;
}

BOOL CLandCell::IsInView(void)
{
    return m_InView;
}

