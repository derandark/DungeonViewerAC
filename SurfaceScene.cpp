

#include "StdAfx.h"
#include "MainDialog.h"
#include "Render.h"
#include "RenderScene.h"
#include "Surface.h"
#include "Texture.h"
#include "Palette.h"
#include "ListSlider.h"

class SurfaceHUD : public RenderHUD
{
public:
    SurfaceHUD(SurfaceScene *pScene);
    virtual ~SurfaceHUD();

    void SetSelectedID(DWORD ID, COLORREF Color);
    void MoveSelUp();
    void MoveSelDown();
    void MoveSelUpPage();
    void MoveSelDownPage();

    virtual void Draw();

private:

    void DrawSlider();
    void DrawSurfInfo();

    SurfaceScene *m_pSurfScene;
    FileSliderHC *m_pFileSlider;
};

SurfaceHUD::SurfaceHUD(SurfaceScene *pScene)
{
    m_pSurfScene = pScene;

    RECT RenderRect = Render::GetRenderRect();

    RECT SliderRect =
        Render::CreateChildRect(RenderRect.right - 136, RenderRect.top + 16, 128, 400);

    m_pFileSlider = new FileSliderHC(SliderRect, 0x08);
}

SurfaceHUD::~SurfaceHUD()
{
    if (m_pFileSlider)
        delete m_pFileSlider;
}

void SurfaceHUD::Draw()
{
    DrawSlider();
    DrawSurfInfo();
}

void SurfaceHUD::DrawSlider()
{
    m_pFileSlider->Draw();
}

void SurfaceHUD::DrawSurfInfo()
{
    CSurface *pSurf = m_pSurfScene->GetCurrentSurface();

    if (!pSurf)
        return;

    RECT InfoRect;

    InfoRect = Render::GetRenderRect();
    InfoRect.left = InfoRect.right - 128;
    InfoRect.top = InfoRect.bottom - 128;

    char InfoString[ 256 ]; 

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
    }

    Render::DrawString(&InfoRect, InfoString);
}

void SurfaceHUD::SetSelectedID(DWORD ID, COLORREF Color)
{
    m_pFileSlider->SelectID(ID, Color);
}

void SurfaceHUD::MoveSelUp()
{
    DWORD ID = m_pFileSlider->GetPrevSel();

    if (ID)
        m_pSurfScene->SetCurrentID(ID);
}

void SurfaceHUD::MoveSelDown()
{
    DWORD ID = m_pFileSlider->GetNextSel();

    if (ID)
        m_pSurfScene->SetCurrentID(ID);
}

void SurfaceHUD::MoveSelUpPage()
{
    DWORD ID = m_pFileSlider->GetPrevPageSel();

    if (ID)
        m_pSurfScene->SetCurrentID(ID);
}

void SurfaceHUD::MoveSelDownPage()
{
    DWORD ID = m_pFileSlider->GetNextPageSel();

    if (ID)
        m_pSurfScene->SetCurrentID(ID);
}

struct SSVERTEX {
    inline SSVERTEX() { }
    inline SSVERTEX(float _x, float _y, float _z, float _tu, float _tv)
    {
        x = _x; y = _y; z = _z;
        tu = _tu; tv = _tv;
    }

    float x, y, z;
    float tu, tv;
};

const DWORD
SSVERTEXFVF = D3DFVF_XYZ | D3DFVF_TEX1;

SurfaceScene::SurfaceScene()
{
    m_pSurfaceVB = NULL;
    m_pSurface = NULL;
    m_pSurfaceHUD = NULL;
}

SurfaceScene::~SurfaceScene()
{
    Destroy();

    m_VertexArray.DestroyVertex();
    m_Polygon.Destroy();
}

int SurfaceScene::SceneType()
{
    return eSurfaceScene;
}

void SurfaceScene::Destroy()
{
    if (m_pSurfaceVB)
    {
        m_pSurfaceVB->Release();
        m_pSurfaceVB = NULL;
    }

    if (m_pSurface)
    {
        CSurface::Release(m_pSurface);
        m_pSurface = NULL;
    }

    if (m_pSurfaceHUD)
    {
        delete m_pSurfaceHUD;
        m_pSurfaceHUD = NULL;
    }
}

void SurfaceScene::Reset()
{
    if (m_pSurfaceVB)
    {
        m_pSurfaceVB->Release();
        m_pSurfaceVB = NULL;
    }

    if (m_pSurface)
    {
        CSurface::Release(m_pSurface);
        m_pSurface = NULL;
    }
}

BOOL SurfaceScene::Init(DWORD ID)
{
    if (!m_pSurfaceHUD)
        m_pSurfaceHUD = new SurfaceHUD(this);

    CreateView();
    SetCurrentID(ID);

    return TRUE;
}

void SurfaceScene::Input_KeyDown(BYTE bKey)
{
    switch (bKey)
    {
    case VK_UP:
        m_pSurfaceHUD->MoveSelUp();

        break;
    case VK_DOWN:
        m_pSurfaceHUD->MoveSelDown();

        break;
    case VK_PRIOR:
        m_pSurfaceHUD->MoveSelUpPage();

        break;
    case VK_NEXT:
        m_pSurfaceHUD->MoveSelDownPage();

        break;
    }
}

void SurfaceScene::Input_KeyUp(BYTE bKey)
{
}

CSurface *SurfaceScene::GetCurrentSurface()
{
    return m_pSurface;
}

DWORD SurfaceScene::GetCurrentID(DWORD ID)
{
    if (m_pSurface)
        return m_pSurface->m_Key;

    return 0;
}

BOOL SurfaceScene::SetCurrentID(DWORD ID)
{
    Reset();

    DEBUGOUT("SurfaceScene::SetCurrentID(0x%08X)\r\n", ID);

    m_pSurface = CSurface::Get(ID);

    if (m_pSurface)
    {
        CreatePolygons();
        m_pSurfaceHUD->SetSelectedID(ID, 0x0000FF00);
        return TRUE;
    }
    else
    {
        m_pSurfaceHUD->SetSelectedID(ID, 0x000000FF);
        return FALSE;
    }
}

BOOL SurfaceScene::CreateView()
{
    D3DXMatrixLookAtLH(
        &m_mView,
        &D3DXVECTOR3(0.0f, 0.0f, 1.5f), /* Eye */
        &D3DXVECTOR3(0.0f, 0.0f, 0.0f), /* Target */
        &D3DXVECTOR3(0.0f, -1.0f, 0.0f) /* Up */);

    float FOV = D3DXToRadian(90.0f);
    float AspectRatio = 4.0f / 3.0f;
    
    D3DXMatrixPerspectiveFovLH(
        &m_mProj, FOV, AspectRatio, 0.01f, 10.0f);

    D3DXMatrixIdentity(&m_mWorld);

    return TRUE;
}

void SurfaceScene::GetDimFactors(float *pWidthFactor, float *pHeightFactor)
{
    ImgTex *pTexture;

    float WidthFactor, HeightFactor; 

    if (!m_pSurface || !(pTexture = m_pSurface->GetTexture()))
    {
        WidthFactor        = 1.0;
        HeightFactor    = 1.0;
    }
    else
    {
        if (pTexture->m_lWidth >= pTexture->m_lHeight)
        {
            HeightFactor    = pTexture->m_lHeight / (float)pTexture->m_lWidth;
            WidthFactor        = 1.0;

            if (pTexture->m_lWidth <= 256)
            {
                HeightFactor    *= pTexture->m_lWidth / 196.0f;
                WidthFactor        *= pTexture->m_lWidth / 196.0f;
            }
        }
        else
        {
            WidthFactor        = pTexture->m_lWidth / (float)pTexture->m_lHeight;
            HeightFactor    = 1.0;

            if (pTexture->m_lHeight <= 256)
            {
                HeightFactor    *= pTexture->m_lHeight / 196.0f;
                WidthFactor        *= pTexture->m_lHeight / 196.0f;
            }
        }
    }

    *pWidthFactor    = WidthFactor;
    *pHeightFactor    = HeightFactor;
}

BOOL SurfaceScene::CreatePolygons()
{
    float width, height;

    GetDimFactors
        (&width, &height);

    m_VertexArray.DestroyVertex();
    m_VertexArray.AllocateVertex(4, 1);

    m_Polygon.Destroy();

    m_Polygon.m_pVertexObjects = new CSWVertex* [4];
    m_Polygon.m_pVertexObjects[0] = (CSWVertex *)((BYTE *)m_VertexArray.m_pVertexBuffer + 0 * CVertexArray::vertex_size);
    m_Polygon.m_pVertexObjects[1] = (CSWVertex *)((BYTE *)m_VertexArray.m_pVertexBuffer + 1 * CVertexArray::vertex_size);
    m_Polygon.m_pVertexObjects[2] = (CSWVertex *)((BYTE *)m_VertexArray.m_pVertexBuffer + 2 * CVertexArray::vertex_size);
    m_Polygon.m_pVertexObjects[3] = (CSWVertex *)((BYTE *)m_VertexArray.m_pVertexBuffer + 3 * CVertexArray::vertex_size);
    m_Polygon.m_iVertexCount = 4;
    m_Polygon.m_pVertexIndices = new WORD [4];
    m_Polygon.m_pVertexIndices[0] = 0;
    m_Polygon.m_pVertexIndices[1] = 1;
    m_Polygon.m_pVertexIndices[2] = 2;
    m_Polygon.m_pVertexIndices[3] = 3;
    m_Polygon.m_iCullType = 0;
    m_Polygon.m_iPolyIndex = 0;
    m_Polygon.m_iPolyType = 0;
    m_Polygon.m_Face1_TexIndex = 0;
    m_Polygon.m_Face1_UVIndices = new BYTE[4];
    m_Polygon.m_Face1_UVIndices[0] = 0;
    m_Polygon.m_Face1_UVIndices[1] = 0;
    m_Polygon.m_Face1_UVIndices[2] = 0;
    m_Polygon.m_Face1_UVIndices[3] = 0;

    m_Polygon.m_pVertexObjects[0]->index = 0;
    m_Polygon.m_pVertexObjects[0]->origin = Vector(-width, -height, 0);
    m_Polygon.m_pVertexObjects[0]->normal = Vector(0, 0, 1);
    m_Polygon.m_pVertexObjects[0]->uvarray = new CSWVertexUV[ 1 ];
    m_Polygon.m_pVertexObjects[0]->uvarray[0].u = 0.0;
    m_Polygon.m_pVertexObjects[0]->uvarray[0].v = 0.0;
    m_Polygon.m_pVertexObjects[0]->uvcount = 1;

    m_Polygon.m_pVertexObjects[1]->index = 1;
    m_Polygon.m_pVertexObjects[1]->origin = Vector(-width, height, 0);
    m_Polygon.m_pVertexObjects[1]->normal = Vector(0, 0, 1);
    m_Polygon.m_pVertexObjects[1]->uvarray = new CSWVertexUV[ 1 ];
    m_Polygon.m_pVertexObjects[1]->uvarray[0].u = 0.0;
    m_Polygon.m_pVertexObjects[1]->uvarray[0].v = 1.0;
    m_Polygon.m_pVertexObjects[1]->uvcount = 1;

    m_Polygon.m_pVertexObjects[2]->index = 2;
    m_Polygon.m_pVertexObjects[2]->origin = Vector(width, height, 0);
    m_Polygon.m_pVertexObjects[2]->normal = Vector(0, 0, 1);
    m_Polygon.m_pVertexObjects[2]->uvarray = new CSWVertexUV[ 1 ];
    m_Polygon.m_pVertexObjects[2]->uvarray[0].u = 1.0;
    m_Polygon.m_pVertexObjects[2]->uvarray[0].v = 1.0;
    m_Polygon.m_pVertexObjects[2]->uvcount = 1;

    m_Polygon.m_pVertexObjects[3]->index = 3;
    m_Polygon.m_pVertexObjects[3]->origin = Vector(width, -height, 0);
    m_Polygon.m_pVertexObjects[3]->normal = Vector(0, 0, 1);
    m_Polygon.m_pVertexObjects[3]->uvarray = new CSWVertexUV[ 1 ];
    m_Polygon.m_pVertexObjects[3]->uvarray[0].u = 1.0;
    m_Polygon.m_pVertexObjects[3]->uvarray[0].v = 0.0;
    m_Polygon.m_pVertexObjects[3]->uvcount = 1;

    return TRUE;

    /*
    float WidthMult, HeightMult;
    
    GetDimFactors(&WidthMult, &HeightMult);

    SSVERTEX Vertices[4];
    Vertices[0] = SSVERTEX(-1.0f * WidthMult, -1.0f * HeightMult, 0.0f, 0.0f, 0.0f);
    Vertices[1] = SSVERTEX(-1.0f * WidthMult, 1.0f * HeightMult, 0.0f, 0.0f, 1.0f);
    Vertices[2] = SSVERTEX(1.0f * WidthMult, -1.0f * HeightMult, 0.0f, 1.0f, 0.0f);
    Vertices[3] = SSVERTEX(1.0f * WidthMult, 1.0f * HeightMult, 0.0f, 1.0f, 1.0f);

    HRESULT hResult = g_pD3DDevice->CreateVertexBuffer(sizeof(Vertices), 0, SSVERTEXFVF, D3DPOOL_DEFAULT, &m_pSurfaceVB, NULL);

    if (FAILED(hResult) || !m_pSurfaceVB)
        return FALSE;

    LPVOID lpMem = NULL;
    hResult = m_pSurfaceVB->Lock(0, sizeof(Vertices), &lpMem, 0);

    if (FAILED(hResult))
        return FALSE;

    memcpy(lpMem, Vertices, sizeof(Vertices));
    
    hResult = m_pSurfaceVB->Unlock();

    if (FAILED(hResult))
        return FALSE;

    return TRUE;
    */
}

void SurfaceScene::Render(double FrameTime)
{
    Render::SetZFill();
    Render::SetFogState(FALSE);
    Render::SetFilling(D3DFILL_SOLID);

    Render::SetLighting(FALSE);
    Render::SetCulling(D3DCULL_NONE);

    Render::SetProjMatrix(&m_mProj);
    Render::SetViewMatrix(&m_mView);
    Render::SetWorldMatrix(&m_mWorld);

    /*
    if (m_pSurface && m_pSurfaceVB)
    {
        Render::SetSurface(m_pSurface);
        Render::DrawVBDirect(D3DPT_TRIANGLESTRIP, SSVERTEXFVF, 2, sizeof(SSVERTEX), m_pSurfaceVB);
    }
    */

    if (m_pSurface)
    {
        Render::SetVertexArray(&m_VertexArray);
        Render::SetSurfaceArray(&m_pSurface);
        Render::draw_pos_surf(&m_Polygon);
    }

    if (m_pSurfaceHUD)
        m_pSurfaceHUD->Draw();
}





