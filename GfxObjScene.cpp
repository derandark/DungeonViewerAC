
#include "StdAfx.h"
#include "PartArray.h"
#include "Render.h"
#include "Frame.h"
#include "RenderScene.h"
#include "PhysicsPart.h"
#include "ListSlider.h"

class GfxObjHUD : public RenderHUD
{
public:
    GfxObjHUD(GfxObjScene *pScene);
    virtual ~GfxObjHUD();

    virtual void Draw();

    void SetSelectedID(DWORD ID, COLORREF Color);
    void MoveSelUp();
    void MoveSelDown();
    void MoveSelUpPage();
    void MoveSelDownPage();

private:

    void DrawSlider();
    void DrawGOInfo();

    GfxObjScene *m_pGfxObjScene;
    FileSliderHC *m_pFileSlider;
};

GfxObjHUD::GfxObjHUD(GfxObjScene *pScene)
{
    m_pGfxObjScene = pScene;

    RECT SliderRect;
    
    SliderRect = Render::GetRenderRect();
    SliderRect.left = SliderRect.right - 128;
    SliderRect.top += 32;

    m_pFileSlider = new FileSliderHC(SliderRect, 0x01);
}

GfxObjHUD::~GfxObjHUD()
{
    if (m_pFileSlider)
        delete m_pFileSlider;
}

void GfxObjHUD::Draw()
{
    DrawSlider();
    DrawGOInfo();
}

void GfxObjHUD::DrawSlider()
{
    m_pFileSlider->Draw();
}

void GfxObjHUD::DrawGOInfo()
{
    CGfxObj *pGfxObj = m_pGfxObjScene->GetGfxObj();

    char InfoString[ 256 ]; 

    *InfoString = 0;
    /*
    if (pSurf->GetFlags() & 6)
    {
        sprintf(InfoString,
            "SurfaceID: 0x%08X\n"
            "Flags: 0x%08X\n"
            "TextureID: 0x%08X\n"
            "PaletteID: 0x%08X\n"
            "Translucency: %.4f\n"
            "Luminosity: %.4f\n"
            "Diffuse: %.4f",
            pSurf->m_Key,
            pSurf->GetFlags(),
            pSurf->m_dwTextureID,
            pSurf->m_dwPaletteID,
            pSurf->m_fTranslucency,
            pSurf->m_fLuminosity,
            pSurf->m_fDiffuse);
    }
    else
    {
        sprintf(InfoString,
            "SurfaceID: %08X\n"
            "Flags: %08X\n"
            "Color: %08X\n"
            "Translucency: %f\n"
            "Luminosity: %f\n"
            "Diffuse: %f",
            pSurf->m_Key,
            pSurf->GetFlags(),
            pSurf->m_dwRGBColor,
            pSurf->m_fTranslucency,
            pSurf->m_fLuminosity,
            pSurf->m_fDiffuse);
    }*/

    ULONG Width, Height;
    Render::GetWindowSize(&Width, &Height);

    ULONG x = Width - 128;
    ULONG y = Height - 128;

    Render::DrawString(x, y, InfoString);
}

void GfxObjHUD::SetSelectedID(DWORD ID, COLORREF Color)
{
    m_pFileSlider->SelectID(ID, Color);
}

void GfxObjHUD::MoveSelUp()
{
    DWORD ID = m_pFileSlider->GetPrevSel();

    if (ID)
        m_pGfxObjScene->SetCurrentID(ID);
}

void GfxObjHUD::MoveSelDown()
{
    DWORD ID = m_pFileSlider->GetNextSel();

    if (ID)
        m_pGfxObjScene->SetCurrentID(ID);
}

void GfxObjHUD::MoveSelUpPage()
{
    DWORD ID = m_pFileSlider->GetPrevPageSel();

    if (ID)
        m_pGfxObjScene->SetCurrentID(ID);
}

void GfxObjHUD::MoveSelDownPage()
{
    DWORD ID = m_pFileSlider->GetNextPageSel();

    if (ID)
        m_pGfxObjScene->SetCurrentID(ID);
}

GfxObjScene::GfxObjScene()
{
    m_pPartArray = NULL;
    m_pGfxObj = NULL;
    m_pHUD = NULL;
}

GfxObjScene::~GfxObjScene()
{
    Destroy();
}

int GfxObjScene::SceneType()
{
    return eGfxObjScene;
}

CGfxObj *GfxObjScene::GetGfxObj()
{
    return m_pGfxObj;
}

CPartArray *GfxObjScene::GetPartArray()
{
    return m_pPartArray;
}

void GfxObjScene::Input_KeyDown(BYTE bKey)
{
    switch (bKey)
    {
    case VK_PRIOR:    m_pHUD->MoveSelUp(); break;
    case VK_NEXT:    m_pHUD->MoveSelDown();    break;
    case VK_HOME:    m_pHUD->MoveSelUpPage(); break;
    case VK_END:    m_pHUD->MoveSelDownPage(); break;
    }

    ModelScene::Input_KeyDown(bKey);
}

void GfxObjScene::Input_KeyUp(BYTE bKey)
{
    ModelScene::Input_KeyUp(bKey);
}

BOOL GfxObjScene::Init(DWORD ID)
{
    if (!m_pHUD)
        m_pHUD = new GfxObjHUD(this);

    InitCamera();
    SetCurrentID(ID);

    return TRUE;
}

void GfxObjScene::InitCamera()
{
    Vector DefaultOrigin(0.0, -2.2, 1.1);
    Vector DefaultAngles(D3DXToRadian(-5.0f), 0, 0);

    Frame DefaultView;

    // Rotation starts at zero.
    DefaultView.euler_set_rotate(Vector(0, 0, 0), 0);

    // Rotate to default angles.
    DefaultView.rotate(DefaultAngles);

    // Set default origin.
    DefaultView.m_origin = DefaultOrigin;

    m_Camera.SetFOV(90.0f);
    m_Camera.SetViewPosition(DefaultView);
}

BOOL GfxObjScene::SetCurrentID(DWORD ID)
{
    Reset();

    DEBUGOUT("GfxObjScene::SetCurrentID(0x%08X)\r\n", ID);

    m_pPartArray = CPartArray::CreateMesh(NULL, ID);

    if (m_pPartArray)
    {
        if (m_pPartArray->m_pPartObjs && m_pPartArray->m_pPartObjs[0] && m_pPartArray->m_pPartObjs[0]->m_GfxObjs)
            m_pGfxObj = m_pPartArray->m_pPartObjs[0]->m_GfxObjs[0];
        else
            m_pGfxObj = NULL;

        m_pHUD->SetSelectedID(ID, 0x0000FF00);
        return TRUE;
    }
    else
    {
        m_pGfxObj = NULL;
        m_pHUD->SetSelectedID(ID, 0x000000FF);
        return FALSE;
    }
}

void GfxObjScene::Destroy()
{
    if (m_pPartArray)
    {
        delete m_pPartArray;
        m_pPartArray = NULL;
    }

    if (m_pHUD)
    {
        delete m_pHUD;
        m_pHUD = NULL;
    }

    m_pGfxObj = NULL;
}

void GfxObjScene::Reset()
{
    if (m_pPartArray)
    {
        delete m_pPartArray;
        m_pPartArray = NULL;
    }

    m_pGfxObj = NULL;
}

void GfxObjScene::Render(double FrameTime)
{
    UpdateCamera();

    Render::SetZFill();
    Render::SetFogState(FALSE);
    Render::SetFilling(D3DFILL_SOLID);

    Render::SetLighting(FALSE);
    Render::SetCulling(D3DCULL_NONE);

    Render::SetProjMatrix(m_Camera.GetProjMatrix());
    Render::SetViewMatrix(m_Camera.GetViewMatrix());

    if (m_pPartArray)
        m_pPartArray->Draw(NULL);

    if (m_pHUD)
        m_pHUD->Draw();
}










