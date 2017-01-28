
#pragma once

class CGfxObj;
class GfxObjHUD;

class GfxObjScene :
    public ModelScene
{
public:
    GfxObjScene();
    virtual ~GfxObjScene();

    void Destroy();
    void Reset();
    void InitCamera();
    BOOL SetCurrentID(DWORD ID);
    BOOL Init(DWORD ID);

    virtual int SceneType();
    virtual void Render(double);
    virtual void Input_KeyDown(BYTE bKey);
    virtual void Input_KeyUp(BYTE bKey);

    CGfxObj *GetGfxObj();
    CPartArray *GetPartArray();

private:

    GfxObjHUD *m_pHUD;
    CGfxObj *m_pGfxObj;
    CPartArray *m_pPartArray;
};
