

#include "StdAfx.h"
#include "Render.h"
#include "HUDControls.h"

Region::Region(const RECT& Rect)
{
    m_Rect = Rect;
}

Region::~Region()
{
}

BOOL Region::PointInRegion(long x, long y)
{
    if (x >= m_Rect.left && 
         x <= m_Rect.right && 
         y >= m_Rect.top && 
         y <= m_Rect.bottom)
         return TRUE;

    return FALSE;
}

HUDControl::HUDControl(const RECT& Rect) : Region(Rect)
{
    m_Visible = TRUE;
}

HUDControl::~HUDControl()
{
}

BOOL HUDControl::GetVisible()
{
    return m_Visible;
}

void HUDControl::SetVisible(BOOL Visible)
{
    m_Visible = Visible;
}

void HUDControl::Draw()
{
    if (!GetVisible())
        return;

    DrawInternal();
}

void HUDControl::DrawBox(long x, long y, long cx, long cy, COLORREF Color, float Width, BOOL Antialias)
{
    LPD3DXLINE lpLineDrawer = Render::GetLineDrawer();

    if (!lpLineDrawer)
        return;

    // Transform local coordinates to device coordinates..
    D3DXVECTOR2 Box[5];

    Box[0] = Box[1] = Box[2] = Box[3] = Box[4] =
        D3DXVECTOR2(m_Rect.left + x, m_Rect.top + y);

    Box[1].x += cx;
    Box[2].x += cx;
    Box[2].y += cy;
    Box[3].y += cy;

    lpLineDrawer->SetWidth(Width);
    lpLineDrawer->SetAntialias(Antialias);
    lpLineDrawer->Begin();
    lpLineDrawer->Draw(Box, 5, Color);
    lpLineDrawer->End();
}

void HUDControl::DrawRegionBox()
{
    LPD3DXLINE lpLineDrawer = Render::GetLineDrawer();

    if (!lpLineDrawer)
        return;

    D3DXVECTOR2 Box[5];
    
    Box[0].x = Box[4].x = m_Rect.left;
    Box[0].y = Box[4].y = m_Rect.top;
    Box[1].x = m_Rect.right;
    Box[1].y = m_Rect.top;
    Box[2].x = m_Rect.right;
    Box[2].y = m_Rect.bottom;
    Box[3].x = m_Rect.left;
    Box[3].y = m_Rect.bottom;

    lpLineDrawer->SetWidth(3);
    lpLineDrawer->Begin();
    lpLineDrawer->Draw(Box, 5, 0xFFFF0000);
    lpLineDrawer->End();
}

ButtonControl::ButtonControl(const RECT& Rect) : HUDControl(Rect)
{
}

ButtonControl::~ButtonControl()
{
}

void ButtonControl::DrawInternal()
{
}







