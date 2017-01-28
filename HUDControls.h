
#pragma once

class Region
{
public:
    Region(const RECT& Rect);
    virtual ~Region();

    BOOL PointInRegion(long x, long y);

protected:

    RECT m_Rect;
};

class HUDControl : public Region
{
public:
    HUDControl(const RECT& Rect);
    virtual ~HUDControl();

    virtual void Draw();

    virtual BOOL GetVisible();
    virtual void SetVisible(BOOL Visible);

protected:

    virtual void DrawInternal() = 0;

    void DrawBox(long x, long y, long cx, long cy, COLORREF Color, float Width, BOOL Antialias);
    void DrawRegionBox();

    BOOL m_Visible;
};

class ButtonControl : public HUDControl
{
public:
    ButtonControl(const RECT& Rect);
    virtual ~ButtonControl();

protected:

    virtual void DrawInternal();

};



