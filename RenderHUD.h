
#pragma once


class RenderHUD
{
public:
    RenderHUD();
    virtual ~RenderHUD();

    virtual void Draw();
};

class PrimaryHUD : public RenderHUD
{
public:
    PrimaryHUD();
    virtual ~PrimaryHUD();

    virtual void Draw();
};
