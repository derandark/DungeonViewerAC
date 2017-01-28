
#include "StdAfx.h"
#include "Preferences.h"
#include "RenderScene.h"
#include "Frame.h"
#include "Render.h"

#include "Vertex.h"
#include "Surface.h"
#include "Texture.h"
#include "Palette.h"
#include "Material.h"
#include "Polygon.h"
#include "GfxObj.h"
#include "Environment.h"
#include "EnvCell.h"
#include "PhysicsObj.h"
#include "PartArray.h"
#include "Setup.h"
#include "Random.h"
#include "LandDefs.h"
#include "LandCell.h"
#include "LandBlockStruct.h"
#include "LandBlockInfo.h"
#include "LandBlock.h"
#include "LandScape.h"

LPDIRECT3D9            g_pD3D = NULL;
LPDIRECT3DDEVICE9    g_pD3DDevice = NULL;

LightManager*    Render::pLightManager = NULL;

BOOL            Render::CurrFogState = FALSE;
D3DCOLOR        Render::CurrFogColor = 0xFFFFFFFF;
float            Render::CurrFogStart = 0.0f;
float            Render::CurrFogEnd = 100.0f;
D3DCMPFUNC        Render::CurrZFunc = D3DCMP_LESSEQUAL;
BOOL            Render::CurrZWrite = TRUE;
D3DFILLMODE        Render::CurrFill = D3DFILL_SOLID;
D3DCULL            Render::CurrCull = D3DCULL_NONE;
BOOL            Render::CurrAlphaBlend = FALSE;
D3DBLEND        Render::CurrSrcBlend = D3DBLEND_ONE;
D3DBLEND        Render::CurrDestBlend = D3DBLEND_ZERO;
BOOL            Render::CurrLighting = TRUE;
D3DCOLOR        Render::CurrTextColor = 0xFFFFFFFF;
D3DCOLOR        Render::CurrFillColor = 0x30303030;

XFormFrame        Render::FrameStack[10];
XFormFrame*        Render::FrameCurrent = NULL;
DWORD            Render::FramePushCount = 0;

D3DPRESENT_PARAMETERS Render::m_PParams;

// Attributes set by surfaces.
D3DCOLORVALUE    Render::CurrSurfaceColor;
long            Render::CurrAlpha;
float            Render::CurrOffsetU;
float            Render::CurrOffsetV;
float            Render::CurrScaleU;
float            Render::CurrScaleV;
D3DCOLOR        Render::CurrColor;
D3DCOLORVALUE    Render::CurrDiffuse;
D3DCOLORVALUE    Render::CurrLuminosity;
D3DCMPFUNC        Render::TransparentZFuncMode = D3DCMP_ALWAYS;
D3DCMPFUNC        Render::NormalZFuncMode = D3DCMP_LESSEQUAL;
BOOL            Render::NormalZWriteMode = TRUE;

LPDIRECT3DTEXTURE9    Render::CurrD3DTexture = NULL;
D3DMATERIAL9        Render::DefaultD3DMaterial;
LPD3DXMATRIXSTACK    Render::MatrixStack;

CPolygon*        Render::PolyList[1024];
CPolygon**        Render::PolyNext = Render::PolyList;
DWORD            Render::FramePolyCount = 0;

CVertexArray*    Render::CurrVertexArray = NULL;
DWORD            Render::CurrVertexType = 0;
LPVOID            Render::CurrVertices = NULL;
DWORD            Render::CurrNumVertices = 0;
BYTE*            Render::CurrUVIndices = NULL;
CSurface**        Render::CurrSurfaceArray = NULL;
CSurface*        Render::CurrSurface = NULL;
DWORD            Render::CurrSurfaceType = 0;
ImgTex*            Render::CurrTexture = NULL;
Palette*        Render::CurrPalette = NULL;
CMaterial*        Render::CurrMaterial = NULL;
BOOL            Render::CurrMaterialAlphaBlend = FALSE;

FPSTracker        Render::CurrFPS;
RECT            Render::CurrRenderRect;

PrimaryHUD        Render::SharedHUD;
LPD3DXLINE        Render::DXLine = NULL;
LPD3DXFONT        Render::DXFont = NULL;

bool Render::FullBrightOverride = false;

BOOL Render::InitDirect3D()
{
    g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

    if (!g_pD3D)
        return FALSE;

    return TRUE;
}

BOOL Render::InitDirect3DDevice(HWND hWnd)
{
    D3DDISPLAYMODE DisplayMode;

    if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &DisplayMode)))
    {
        DEBUGOUT("Failed retrieving adapter modes..\r\n");
        return FALSE;
    }

    ZeroMemory(&m_PParams, sizeof(m_PParams));

    m_PParams.Windowed                    = TRUE;
    m_PParams.SwapEffect                = D3DSWAPEFFECT_DISCARD;
    m_PParams.BackBufferFormat            = DisplayMode.Format;
    m_PParams.EnableAutoDepthStencil    = TRUE;
    m_PParams.AutoDepthStencilFormat    = D3DFMT_D16;
    m_PParams.PresentationInterval        = D3DPRESENT_INTERVAL_IMMEDIATE;

    if (SUCCEEDED(g_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_D16, FALSE, (D3DMULTISAMPLE_TYPE)RenderPreferences::MultiSample, NULL)))
        m_PParams.MultiSampleType = (D3DMULTISAMPLE_TYPE) RenderPreferences::MultiSample;

    if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_PParams, &g_pD3DDevice)))
    {
        DEBUGOUT("Failed creating device..\r\n");
        return FALSE;
    }

    return TRUE;
}

BOOL Render::InitDirect3DFont()
{
    D3DXFONT_DESC fd;
    fd.Height = DEFAULT_RENDER_FONT_HEIGHT;
    fd.Width = 0;
    fd.Weight = FW_NORMAL;
    fd.Italic = FALSE;
    fd.MipLevels = D3DX_DEFAULT;
    fd.CharSet = DEFAULT_CHARSET;
    fd.OutputPrecision = OUT_DEFAULT_PRECIS;
    fd.Quality = ANTIALIASED_QUALITY;
    fd.PitchAndFamily = VARIABLE_PITCH;
    strcpy(fd.FaceName, DEFAULT_RENDER_FONT_NAME);

    if (FAILED(D3DXCreateFontIndirect(g_pD3DDevice, &fd, &DXFont)))
        return FALSE;

    return TRUE;
}

BOOL Render::InitDirect3DLine()
{
    if (FAILED(D3DXCreateLine(g_pD3DDevice, &DXLine)))
        return FALSE;

    return TRUE;
}

void Render::SetupD3DDevice()
{
    g_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, CurrFogState);
    g_pD3DDevice->SetRenderState(D3DRS_FOGCOLOR, *((DWORD *)&CurrFogColor));
    g_pD3DDevice->SetRenderState(D3DRS_FOGSTART, *((DWORD *)&CurrFogStart));
    g_pD3DDevice->SetRenderState(D3DRS_FOGEND, *((DWORD *)&CurrFogEnd));
    g_pD3DDevice->SetRenderState(D3DRS_ZFUNC, CurrZFunc);
    g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, CurrFill);
    g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, CurrCull);
    g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, CurrLighting);

    // g_pD3DDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
    // g_pD3DDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 16);

    g_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

    DefaultD3DMaterial.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    DefaultD3DMaterial.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    DefaultD3DMaterial.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	DefaultD3DMaterial.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
    DefaultD3DMaterial.Power = 10.0f;
    g_pD3DDevice->SetMaterial(&DefaultD3DMaterial);

    D3DXCreateMatrixStack(0, &MatrixStack);
}

BOOL Render::Init(HWND hWnd)
{
    if (!InitDirect3D())
    {
        DEBUGOUT("Failed InitDirect3D..\r\n");

        MessageBox(NULL, "Failed allocating Direct3D!", "Render::Init", MB_OK);
        return FALSE;
    }
    
    if (!InitDirect3DDevice(hWnd))
    {
        DEBUGOUT("Failed InitDirect3DDevice..\r\n");

        MessageBox(NULL, "Failed allocating Direct3D device!", "Render::Init", MB_OK);
        return FALSE;
    }

    if (!InitDirect3DFont())
    {
        DEBUGOUT("Failed InitDirect3DFont..\r\n");
        return FALSE;
    }

    if (!InitDirect3DLine())
    {
        DEBUGOUT("Failed InitDirect3DLine..\r\n");
        return FALSE;
    }

    if (!pLightManager)
    {
        pLightManager = new LightManager;
        pLightManager->Init();
    }

    ::GetClientRect(hWnd, &CurrRenderRect);

    SetupD3DDevice();

    SetTextColor(Preferences::GetTextColor());
    SetFillColor(Preferences::GetFillColor());

    CurrFPS.Init(1.0f);

    InitFrameStack();

    ImgTex::InitTemporaryBuffer();

    SetDegradeLevelInternal(1.0);

    return TRUE;
}

void Render::ReleaseDirect3D()
{
    if (g_pD3D)
    {
        g_pD3D->Release();
        g_pD3D = NULL;
    }
}

void Render::ReleaseDirect3DDevice()
{
    if (g_pD3DDevice)
    {
        g_pD3DDevice->Release();
        g_pD3DDevice = NULL;
    }
}

void Render::Cleanup()
{
    RenderScene::Cleanup();

    ImgTex::CleanupTemporaryBuffer();

    if (pLightManager)
    {
        delete pLightManager;
        pLightManager = NULL;
    }

    if (DXLine)
    {
        DXLine->Release();
        DXLine = NULL;
    }

    if (DXFont)
    {
        DXFont->Release();
        DXFont = NULL;
    }

    ReleaseDirect3DDevice();
    ReleaseDirect3D();
}

void Render::DrawFrame()
{
    // Update Time data (move this)
    Time::Update();

    // Update FPS statistics
    CurrFPS.Update();

    // Begin..
    g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, CurrFillColor, 1.0f, 0);
    g_pD3DDevice->BeginScene();

    // Reset Polygon stats.
    FramePolyCount = 0;

    // Render Current Scene
    RenderScene::RenderCurrent();

    // Render Generic HUD
    if (RenderPreferences::RenderHud)
        SharedHUD.Draw();

    // End..
    g_pD3DDevice->EndScene();
    g_pD3DDevice->Present(NULL, NULL, NULL, NULL);

    // Increment FPS counter.
    CurrFPS.IncrementFrame();
}

void Render::GetWindowSize(ULONG *Width, ULONG *Height)
{
    *Width = CurrRenderRect.right - CurrRenderRect.left;
    *Height = CurrRenderRect.bottom - CurrRenderRect.top;
}

RECT& Render::GetRenderRect()
{
    return CurrRenderRect;
}

RECT Render::CreateChildRect(long x, long y, long cx, long cy)
{
    RECT Child;

    Child.left        = min(CurrRenderRect.right, max(CurrRenderRect.left, x));
    Child.top        = min(CurrRenderRect.bottom, max(CurrRenderRect.top, y));
    Child.right        = min(CurrRenderRect.right, max(CurrRenderRect.left, x + cx));
    Child.bottom    = min(CurrRenderRect.bottom, max(CurrRenderRect.top, y + cy));

    return Child;
}

float Render::GetFPS()
{
    return CurrFPS.GetFPS();
}

DWORD Render::GetFramePolyCount()
{
    return FramePolyCount;
}

BOOL Render::GetFogState()
{
    return CurrFogState;
}

void Render::SetFogState(BOOL Enabled)
{
    if (CurrFogState != Enabled)
    {
        CurrFogState = Enabled;
        g_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, Enabled); 
    }
}

void Render::SetFogColor(D3DCOLOR Color)
{
    if (CurrFogColor != Color)
    {
        CurrFogColor = Color;
        g_pD3DDevice->SetRenderState(D3DRS_FOGCOLOR, Color);
    }
}

void Render::SetFogStart(float Distance)
{
    if (CurrFogStart != Distance)
    {
        CurrFogStart = Distance;
        g_pD3DDevice->SetRenderState(D3DRS_FOGSTART, Distance);
    }
}

void Render::SetFogEnd(float Distance)
{
    if (CurrFogEnd != Distance)
    {
        CurrFogEnd = Distance;
        g_pD3DDevice->SetRenderState(D3DRS_FOGEND, Distance);
    }
}

void Render::SetZCompare()
{
    if (CurrZFunc != D3DCMP_LESSEQUAL)
    {
        CurrZFunc = D3DCMP_LESSEQUAL;
        g_pD3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    }
}

void Render::SetZFill()
{
    if (CurrZFunc != D3DCMP_ALWAYS)
    {
        CurrZFunc = D3DCMP_ALWAYS;
        g_pD3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
    }
}

void Render::SetZFunc(D3DCMPFUNC ZFunc)
{
    if (CurrZFunc != ZFunc)
    {
        CurrZFunc = ZFunc;
        g_pD3DDevice->SetRenderState(D3DRS_ZFUNC, ZFunc);
    }
}

void Render::SetZWrite(BOOL ZWrite)
{
    if (CurrZWrite != ZWrite)
    {
        CurrZWrite = ZWrite;
        g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, ZWrite);
    }
}

void Render::SetFilling(D3DFILLMODE Fill)
{
    if (CurrFill != Fill)
    {
        CurrFill = Fill;
        g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, Fill);
    }
}

void Render::SetMultiSample(D3DMULTISAMPLE_TYPE MultiSample)
{
    if (!g_pD3DDevice)
        return;

    if (SUCCEEDED(g_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_D16, FALSE, MultiSample, NULL)))
    {
        // m_PParams.MultiSampleType = MultiSample;
        // g_pD3DDevice->Reset(&m_PParams);
        MessageBox(NULL, "You must restart for the multisampling to take affect.\r\n", "Note:", MB_OK);
    }
}

void Render::SetCulling(D3DCULL Cull)
{
    if (CurrCull != Cull)
    {
        CurrCull = Cull;
        g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, Cull);
    }
}

void Render::SetLighting(BOOL Enabled)
{
    if (CurrLighting != Enabled)
    {
        CurrLighting = Enabled;
        g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, Enabled);
    }
}

void Render::SetMaterial(CMaterial *pMaterial)
{
	/*
    if (CurrMaterial == pMaterial)
        return;
    else
        CurrMaterial = pMaterial;
		*/

	D3DMATERIAL9 *pD3DMaterial = (pMaterial != NULL) ? &pMaterial->m_D3DMaterial : &DefaultD3DMaterial;

	if (!RenderPreferences::RenderLights || (Render::FullBrightOverride && RenderPreferences::RenderFullbrightOutsideCells))
	{
		// Make the material full brightness instead.
		D3DMATERIAL9 FullbrightMat;
		FullbrightMat = *pD3DMaterial;
		FullbrightMat.Emissive = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
		g_pD3DDevice->SetMaterial(&FullbrightMat);
	}
	else
	{
		g_pD3DDevice->SetMaterial(pD3DMaterial);
	}

    if (pMaterial)
    {
        CurrMaterialAlphaBlend = pMaterial->m_bTranslucent;
    }
    else
    {        
		CurrMaterialAlphaBlend = FALSE;
    }
}

void Render::SetVertexArray(CVertexArray *pVertexArray)
{
    switch (pVertexArray->m_VertexType)
    {
    case 1:    CurrVertexType = 3; break;
    case 2: CurrVertexType = 2; break;
    case 3: CurrVertexType = 1; break;
    }
    
    CurrVertexArray = pVertexArray;
    CurrVertices = pVertexArray->m_pVertexBuffer;
    CurrNumVertices = pVertexArray->m_VertexCount;
}

void Render::SetSurfaceArray(CSurface **ppSurfaces)
{
    CurrSurfaceArray = ppSurfaces;
}

void Render::SetSurfaceInternal(CSurface *pSurface)
{
    CurrSurface = pSurface;

    if (pSurface)
        CurrSurfaceType = pSurface->GetFlags() | 0x10000000;
    else
        CurrSurfaceType = 0x10000000;
}

void Render::SetSurface(CPolygon *pPolygon, int iFace)
{
    BOOL ShouldWrap = FALSE;
    short TexIndex;

    if (!iFace)
    {
        TexIndex = pPolygon->m_Face1_TexIndex;

        if (pPolygon->m_iPolyType & 1)
            ShouldWrap = TRUE;
    }
    else
    {
        TexIndex = pPolygon->m_Face2_TexIndex;

        if (pPolygon->m_iPolyType & 2)
            ShouldWrap = TRUE;
    }

    if (!CurrSurfaceArray)
    {
        CurrSurface = NULL;
        CurrColor = 0xFFFFFFFF;
        CurrAlpha = 255;
        CurrSurfaceColor.r = 1.0;
        CurrSurfaceColor.g = 1.0;
        CurrSurfaceColor.b = 1.0;
        CurrSurfaceType = 1;

        if (CurrD3DTexture != NULL)
        {
            g_pD3DDevice->SetTexture(0, NULL);
            CurrD3DTexture = NULL;
        }

        SetAlphaBlend(FALSE);
        SetSrcBlend(D3DBLEND_ONE);
        SetDestBlend(D3DBLEND_ZERO);
        SetZWrite(NormalZWriteMode);
        SetZFunc(NormalZFuncMode);
        return;
    }
    
    LPDIRECT3DTEXTURE9 NewD3DTexture9 = NULL;

    SetSurfaceInternal(CurrSurfaceArray[ TexIndex ]);

    if (CurrTexture = CurrSurface->m_pTexture)
    {
        if (ShouldWrap)
        {
            CurrSurfaceType |= 0x40000000;

            g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
            g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
        }
        else
        {
            g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
            g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
        }

        if (!iFace)
            CurrUVIndices = pPolygon->m_Face1_UVIndices;
        else
            CurrUVIndices = pPolygon->m_Face2_UVIndices;

        /*
        if (CurrTexture->m_pSurfaceChunk)
        {
            if (ShouldWrap)
            {
                ...
            }
            ...
        }
        */

        CurrOffsetU = 0;
        CurrOffsetV = 0;
        CurrScaleU = 1.0f;
        CurrScaleV = 1.0f;

        NewD3DTexture9 = CurrTexture->m_pD3DTexture9;
    }
    else
    {
        if (CurrColor != CurrSurface->m_dwRGBColor)
        {
            RGBAUnion::Get(CurrSurface->m_dwRGBColor, &CurrSurfaceColor.r, &CurrSurfaceColor.g, &CurrSurfaceColor.b);
            CurrColor = CurrSurface->m_dwRGBColor;
        }
    }

    CurrLuminosity.r = CurrSurface->m_fLuminosity;
    CurrLuminosity.g = CurrSurface->m_fLuminosity;
    CurrLuminosity.b = CurrSurface->m_fLuminosity;

    CurrDiffuse.r = CurrSurface->m_fDiffuse;
    CurrDiffuse.g = CurrSurface->m_fDiffuse;
    CurrDiffuse.b = CurrSurface->m_fDiffuse;

    D3DBLEND    SrcBlend, DstBlend;
    BOOL        AlphaBlend;

    if (CurrSurfaceType & 0x100)
    {
        AlphaBlend = TRUE;
        SrcBlend = D3DBLEND_SRCALPHA;

        if (CurrSurfaceType & 0x10000)
            DstBlend = D3DBLEND_ONE;
        else
            DstBlend = D3DBLEND_INVSRCALPHA;
    }
    else 
    if (CurrSurfaceType & 0x200)
    {
        AlphaBlend        = TRUE;
        SrcBlend        = D3DBLEND_INVSRCALPHA;

        if (CurrSurfaceType & 0x10000)
            DstBlend = D3DBLEND_ONE;
        else
            DstBlend = D3DBLEND_SRCALPHA;
    }
    else
    {
        if (CurrSurfaceType & 0x10000)
        {
            AlphaBlend        = TRUE;
            SrcBlend        = D3DBLEND_ONE;
            DstBlend        = D3DBLEND_ONE;
        }
        else
        {
            AlphaBlend        = FALSE;
            SrcBlend        = D3DBLEND_ONE;
            DstBlend        = D3DBLEND_ZERO;
        }
    }

    if (CurrSurfaceType & 4)
    {
        AlphaBlend        = TRUE;
        SrcBlend        = D3DBLEND_SRCALPHA;
        DstBlend        = D3DBLEND_INVSRCALPHA;
    }

    if (CurrSurfaceType & 0x10)
    {
        if (!AlphaBlend)
        {
            AlphaBlend        = TRUE;
            SrcBlend        = D3DBLEND_SRCALPHA;
            DstBlend        = D3DBLEND_INVSRCALPHA;
        }

        CurrAlpha = (long)((1.0f - CurrSurface->m_fTranslucency) * 255.0f);
    }
    else
        CurrAlpha = 255;


    D3DCMPFUNC    ZFunc;
    BOOL        ZWrite;

    if (AlphaBlend)
    {
        ZFunc    = TransparentZFuncMode; // CurrTransparentZFuncMode = D3DCMP_ALWAYS;
        ZWrite    = FALSE;
    }
    else
    {
        ZFunc    = NormalZFuncMode; // CurrNormalZFuncMode = D3DCMP_LESSEQUAL;
        ZWrite    = NormalZWriteMode; // CurrNormalZWriteMode = TRUE;
    }

    if (CurrD3DTexture != NewD3DTexture9)
    {
        g_pD3DDevice->SetTexture(0, NewD3DTexture9);
        CurrD3DTexture = NewD3DTexture9;
    }

    SetAlphaBlend    (AlphaBlend);
    SetSrcBlend        (SrcBlend);
    SetDestBlend    (DstBlend);
    SetZWrite        (ZWrite);
    SetZFunc        (ZFunc);
}

void Render::SetSurface(CSurface *pSurface)
{
    SetSurfaceInternal(pSurface);

    if (pSurface)
        CurrTexture = pSurface->GetTexture();
    else
        CurrTexture = NULL;

    LPDIRECT3DTEXTURE9 pD3DTexture = CurrTexture ?
        CurrTexture->m_pD3DTexture9 : NULL;

    if (CurrD3DTexture != pD3DTexture)
    {
        CurrD3DTexture = pD3DTexture;
        g_pD3DDevice->SetTexture(0, pD3DTexture);
    }

    D3DBLEND SrcBlend;
    D3DBLEND DestBlend;
    BOOL AlphaBlend;
    BOOL ZWrite;
    D3DCMPFUNC ZFunc;

    if (CurrSurfaceType & 0x20000)
    {
        SrcBlend        = D3DBLEND_SRCALPHA; // curr_detail_src_blend
        DestBlend        = D3DBLEND_INVSRCALPHA; // curr_detail_dst_blend
        AlphaBlend        = TRUE;
    }
    else if (CurrSurfaceType & 0x100)
    {
        SrcBlend        = D3DBLEND_SRCALPHA;
        DestBlend        = D3DBLEND_ONE;

        if (!(CurrSurfaceType & 0x10000))
            DestBlend    = D3DBLEND_INVSRCALPHA;

        AlphaBlend        = TRUE;
    }
    else if (CurrSurfaceType & 0x200)
    {
        SrcBlend        = D3DBLEND_INVSRCALPHA;
        DestBlend        = D3DBLEND_ONE;

        if (!(CurrSurfaceType & 0x10000))
            DestBlend    = D3DBLEND_SRCALPHA;

        AlphaBlend        = TRUE;
    }
    else if (CurrSurfaceType & 0x10000)
    {
        SrcBlend        = D3DBLEND_ONE;
        DestBlend        = D3DBLEND_ONE;
        AlphaBlend        = TRUE;
    }
    else if (CurrMaterialAlphaBlend)
    {
        SrcBlend        = D3DBLEND_SRCALPHA;
        DestBlend        = D3DBLEND_INVSRCALPHA;
        AlphaBlend        = TRUE;
    }
    else
    {
        SrcBlend        = D3DBLEND_ONE;
        DestBlend        = D3DBLEND_ZERO;
        AlphaBlend        = FALSE;
    }

    if (CurrSurfaceType & 0x4)
    {
        SrcBlend    = D3DBLEND_SRCALPHA;
        DestBlend    = D3DBLEND_INVSRCALPHA;
        AlphaBlend    = TRUE;
        ZWrite        = FALSE;
        ZFunc        = D3DCMP_ALWAYS; // transparent_zfunc_mode
    }
    else if (AlphaBlend)
    {
        ZWrite        = FALSE;
        ZFunc        = D3DCMP_ALWAYS; // transparent_zfunc_mode
    }
    else
    {
        ZWrite        = TRUE;
        ZFunc        = D3DCMP_LESSEQUAL;
    }

    SetAlphaBlend(AlphaBlend);
    SetSrcBlend(SrcBlend);
    SetDestBlend(DestBlend);
    SetZWrite(ZWrite);
    SetZFunc(ZFunc);
}

void Render::SetAlphaBlend(BOOL Enabled)
{
    if (CurrAlphaBlend != Enabled)
    {
        CurrAlphaBlend = Enabled;
        g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, Enabled);
    }
}

void Render::SetSrcBlend(D3DBLEND Blend)
{
    if (CurrSrcBlend != Blend)
    {
        CurrSrcBlend = Blend;
        g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, Blend);
    }
}

void Render::SetDestBlend(D3DBLEND Blend)
{
    if (CurrDestBlend != Blend)
    {
        CurrDestBlend = Blend;
        g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, Blend);
    }
}

void Render::DrawVBDirect(D3DPRIMITIVETYPE PrimType, DWORD PrimFVF, DWORD PrimCount, DWORD VertexSize, LPDIRECT3DVERTEXBUFFER9 lpVB)
{
    HRESULT hResult;

    hResult = g_pD3DDevice->SetStreamSource(0, lpVB, 0, VertexSize);

    if (FAILED(hResult))
        DEBUGOUT("Failed SetStreamSource\r\n");

    hResult = g_pD3DDevice->SetFVF(PrimFVF);

    if (FAILED(hResult))
        DEBUGOUT("Failed SetFVF\r\n");

    hResult = g_pD3DDevice->DrawPrimitive(PrimType, 0, PrimCount);

    if (FAILED(hResult))
        DEBUGOUT("Failed DrawPrimitive\r\n");
}

void Render::SetWorldMatrix(D3DMATRIX *pMatrix)
{
    g_pD3DDevice->SetTransform(D3DTS_WORLD, pMatrix);
}

void Render::SetViewMatrix(D3DMATRIX *pMatrix) 
{
    g_pD3DDevice->SetTransform(D3DTS_VIEW, pMatrix);
}

void Render::SetProjMatrix(D3DMATRIX *pMatrix)
{
    g_pD3DDevice->SetTransform(D3DTS_PROJECTION, pMatrix);
}

void Render::SetTextColor(COLORREF Color)
{
    CurrTextColor = ((Color >> 16) & 0xFF) | ((Color & 0xFF) << 16) | (Color & 0xFF00);
}

void Render::SetFillColor(COLORREF Color)
{
    CurrFillColor = ((Color >> 16) & 0xFF) | ((Color & 0xFF) << 16) | (Color & 0xFF00);
}

LPD3DXFONT Render::GetFontDrawer()
{
    return DXFont;
}

LPD3DXLINE Render::GetLineDrawer()
{
    return DXLine;
}

int Render::DrawString(long x, long y, const char* text, DWORD format)
{
    RECT RenderRect = GetRenderRect();
    RECT DrawRect;

    // Don't go offscreen ?
    DrawRect.left    = RenderRect.left + x;
    DrawRect.right    = RenderRect.right;
    DrawRect.top    = RenderRect.top + y;
    DrawRect.bottom = RenderRect.bottom;

    return DXFont->DrawTextA(NULL, text, -1, &DrawRect, format, 0xFF000000 | CurrTextColor);
}

int Render::DrawString(RECT *prect, const char* text, DWORD format)
{
    return DXFont->DrawTextA(NULL, text, -1, prect, format, 0xFF000000 | CurrTextColor);
}

BOOL UnderRay = FALSE;

int Render::DrawMesh(CGfxObj *Obj, Position *ObjPos, BOOL bObjBuilding)
{
    int retval = 0;

    positionPush(1, ObjPos);

    if (viewconeCheck(Obj->m_RenderSphere))
    {
        retval = 2;

        positionPush(2, NULL);

        if (bObjBuilding)
        {
            // Obj->m_RenderTree->build_draw_partial();
        }
        else
        {
            SetVertexArray(&Obj->m_VertexArray);

            /*
            for (DWORD i = 0; i < Obj->m_RenderTriangleCount; i++)
                PolyList_Add(&Obj->m_RenderTriangles[i]);
                        
            PolyList_Draw();
            */

            float depth;

            // highlighting
			/*
            if (GfxObjUnderSelectionRay(Obj, &depth))
            {
                UnderRay = TRUE;
                retval = 3;
            }
			*/
            //

            // UnderRay = FALSE;

            if (Obj->m_RenderTree)
               Obj->m_RenderTree->draw_no_check();

            UnderRay = FALSE;
        }

        positionPop();
    }
    else
    {
        positionPop();
        retval = 1;
    }

    return retval;
}

void OutputLightInfo()
{
    for (DWORD i = 0; i < MAX_ACTIVE_LIGHTS; i++)
    {
        D3DLIGHT9 light;
        BOOL enabled;
        g_pD3DDevice->GetLightEnable(i, &enabled);

        if (FAILED(g_pD3DDevice->GetLight(i, &light)))
            DEBUGOUT("Failed getting light %i\r\n", i);
        else
            DEBUGOUT("#%i %s @ %f %f %f\r\n", i, enabled?"Enabled":"Disabled", light.Position.x, light.Position.y, light.Position.z);
    }
}

int SortObjsByDistance(const void *p1, const void *p2)
{
    if (!p1)
    {
        if (!p2)
            return 0;
        else
            return 1;
    }
    else
    if (!p2)
        return -1;

    float dist1 = ((CPhysicsObj **)p1)[0]->m_ViewerDist;
    float dist2 = ((CPhysicsObj **)p2)[0]->m_ViewerDist;

    if (dist1 > dist2)
        return -1;
    if (dist1 < dist2)
        return 1;

    return 0;
}

void Render::DrawEnvCell(CEnvCell *pEnvCell)
{
    NormalZFuncMode = D3DCMP_ALWAYS;
    NormalZWriteMode = FALSE;
    // usesunlight = FALSE;

    SetMaterial(NULL);
    SetSurfaceArray(pEnvCell->m_Surfaces);

    ObjectScaleVec = Vector(1.0, 1.0, 1.0);
    ObjectScale = 1.0;

    positionPush(3, &pEnvCell->m_Position);

    // D3DMATRIX matrix;
    // g_pD3DDevice->GetTransform(D3DTS_WORLD, &matrix);
    // DEBUGOUTMATRIX(&matrix);

    // Draw some environment cells here?

    NormalZFuncMode = D3DCMP_LESSEQUAL;
    NormalZWriteMode = TRUE;

    RenderDevice::UpdateObjCell(pEnvCell);

    if (pEnvCell->m_CellStruct && pEnvCell->m_CellStruct->m_2C)
    {
        if (viewconeCheck(pEnvCell->m_CellStruct->m_2C->GetSphere()))
		  {		  
			  pEnvCell->m_CellStruct->m_2C->draw_no_check();
		  }
    }

    if (RenderPreferences::RenderObjects)
    {
        // THIS IS EVIL
        qsort(
            pEnvCell->m_Objects.array_data,
            pEnvCell->m_ObjectCount,
            sizeof(CPhysicsObj *), 
            &SortObjsByDistance);

        for (DWORD i = 0; i < pEnvCell->m_ObjectCount; i++)
        {
            if (pEnvCell->m_Objects.array_data[i])
                pEnvCell->m_Objects.array_data[i]->DrawRecursive();
        }
    }

    // OutputLightInfo();

    positionPop();

    // usesunlight = TRUE
    // object_scale_vec = Vector(1.0, 1.0, 1.0);
    // object_scale = 1.0
}

void Render::polyDraw(CPolygon *Poly)
{
    *(PolyNext++) = Poly;
}

void Render::polyDrawClip(CPolygon *Poly, DWORD flags)
{
    *(PolyNext++) = Poly;
}

void Render::polyListFinishInternal()
{
    DWORD Ambient;

    if (UnderRay)
    {
        g_pD3DDevice->GetRenderState(D3DRS_AMBIENT, &Ambient);
        g_pD3DDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_ARGB(255, 255, 255, 255));
    }

    CPolygon **PolyIt = PolyList;

	 Vector minv(0,0,0), maxv(0,0,0);

    while (PolyIt < PolyNext)
    {
        // FIX ME (also find where culling takes place)
        CPolygon *Poly = *PolyIt;

        float ViewProduct = Poly->m_plane.dot_product(FrameCurrent->m_LocalView.m_origin);

        if (Poly->m_iCullType == CullNone)
        {
            if (ViewProduct > F_EPSILON)
                draw_pos_surf(Poly);
            else
                draw_pneg_surf(Poly);
        }
        else
        {
            if (ViewProduct > F_EPSILON)
                draw_pos_surf(Poly);
            else
            {
                if (Poly->m_iCullType == CullUnknown)
                    draw_neg_surf(Poly);
            }
        }

		  // pea

		  for (unsigned int i = 0; i < Poly->m_iVertexCount; i++)
		  {
			  // int vindex = Poly->m_pVertexIndices[i];
			  CSWVertex *vertex = Poly->m_pVertexObjects[0]; // vindex];
			  
			  minv.x = min(minv.x, vertex->origin.x);
			  minv.y = min(minv.y, vertex->origin.y);
			  minv.z = min(minv.z, vertex->origin.z);

			  maxv.x = max(maxv.x, vertex->origin.x);
			  maxv.y = max(maxv.y, vertex->origin.y);
			  maxv.z = max(maxv.z, vertex->origin.z);
		  }

        PolyIt++;
    }

    if (UnderRay)
	 {
		 D3DCOLOR Diffuse[8], Specular = 0x7F008000;

		 float LightMod = 1.0; // LIGHT_MOD;

		 LPDIRECT3DSTATEBLOCK9 pblock = NULL;
		 g_pD3DDevice->CreateStateBlock(D3DSBT_ALL, &pblock);

		 g_pD3DDevice->SetTexture(0, NULL);
		 g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		 g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		 g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		 g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
		 g_pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFF0000);

		 for (int i = 0; i < 8; i++)
		 {
			 Diffuse[i] = 0;
			 Diffuse[i] |= (BYTE)(CurrSurfaceColor.b * 255.0f * LightMod);
			 Diffuse[i] |= (BYTE)(CurrSurfaceColor.g * 255.0f * LightMod) << 8;
			 Diffuse[i] |= (BYTE)(CurrSurfaceColor.r * 255.0f * LightMod) << 16;
			 Diffuse[i] |= (BYTE)(CurrAlpha) << 24;

			 // ahh screw it
			 Diffuse[i] = 0x80090080;
		 }

		 struct ACRD3DVertex
		 {
			  Vector Origin;
	#ifdef INCLUDE_NORMAL_DATA
			  Vector Normal;
	#endif
			  DWORD Diffuse;
			  DWORD Specular;
			  float TU, TV;
		 };

		 ACRD3DVertex VertexBuffer[8];

		 VertexBuffer[0].Origin = Vector(minv.x, maxv.y, maxv.z);
		 VertexBuffer[1].Origin = Vector(minv.x, maxv.y, minv.z);
		 VertexBuffer[2].Origin = Vector(minv.x, minv.y, maxv.z);
		 VertexBuffer[3].Origin = minv;
		 VertexBuffer[4].Origin = maxv;
		 VertexBuffer[5].Origin = Vector(maxv.x, maxv.y, minv.z);
		 VertexBuffer[6].Origin = Vector(maxv.x, minv.y, maxv.z);
		 VertexBuffer[7].Origin = Vector(maxv.x, minv.y, minv.z);

		 for (int i = 0; i < 8; i++)
		 {
			  VertexBuffer[i].Diffuse = Diffuse[i];
			  VertexBuffer[i].Specular = Specular;

			  /*
			  if (CurrTexture)
			  {
					VertexBuffer[i].TU = pVertices[i]->uvarray[ CurrUVIndices[i] ].u;
					VertexBuffer[i].TV = pVertices[i]->uvarray[ CurrUVIndices[i] ].v;
			  }
			  */
		 }

	#ifdef INCLUDE_NORMAL_DATA
		 g_pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1);
	#else
		 g_pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1);
	#endif

		DWORD indices[] = 
		{
			 0, 1, 2,    // side 1
			 2, 1, 3,
			 4, 0, 6,    // side 2
			 6, 0, 2,
			 7, 5, 6,    // side 3
			 6, 5, 4,
			 3, 1, 7,    // side 4
			 7, 1, 5,
			 4, 5, 0,    // side 5
			 0, 5, 1,
			 3, 7, 2,    // side 6
			 2, 7, 6,
		};

		// for (DWORD side = 0; side < (sizeof(indices) / sizeof(DWORD)); side += 3)
		{
			g_pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 36, 12, indices, D3DFMT_INDEX32, VertexBuffer, sizeof(ACRD3DVertex));
		}

		 pblock->Apply();

       g_pD3DDevice->SetRenderState(D3DRS_AMBIENT, Ambient);

		 pblock->Release();
	 }
}

void Render::polyListClear()
{
    PolyNext = PolyList;
}

void Render::draw_pos_surf(CPolygon *Poly)
{
    SetSurface(Poly, 0);

    switch (Poly->m_iVertexCount)
    {
    case 3:    
        DrawTrianglePrim(Poly->m_pVertexObjects);
        break;

    case 4:
        DrawQuadPrim(Poly->m_pVertexObjects);
        break;

    default:
        DrawMultiPrim(Poly->m_pVertexObjects, Poly->m_iVertexCount);
        break;
    }

    FramePolyCount += Poly->m_iVertexCount - 2;
}

void Render::draw_pneg_surf(CPolygon *Poly)
{
    SetSurface(Poly, 0);

    switch (Poly->m_iVertexCount)
    {
    case 3:    
        DrawTrianglePrim(Poly->m_pVertexObjects);
        break;

    case 4:
        DrawQuadPrim(Poly->m_pVertexObjects);
        break;

    default:
        DrawMultiPrim(Poly->m_pVertexObjects, Poly->m_iVertexCount);
        break;
    }

    FramePolyCount += Poly->m_iVertexCount - 2;
}

void Render::draw_neg_surf(CPolygon *Poly)
{
    SetSurface(Poly, 1);

    switch (Poly->m_iVertexCount)
    {
    case 3:    
        DrawTrianglePrim(Poly->m_pVertexObjects);
        break;

    case 4:
        DrawQuadPrim(Poly->m_pVertexObjects);
        break;

    default:
        DrawMultiPrim(Poly->m_pVertexObjects, Poly->m_iVertexCount);
        break;
    }

    FramePolyCount += Poly->m_iVertexCount - 2;
}

#define LIGHT_MOD                1.0f
// #define INCLUDE_NORMAL_DATA 

// These vertices received would normally be transformed (and in a different class)
void Render::DrawTrianglePrim(CSWVertex **pVertices)
{
    D3DCOLOR Diffuse[3], Specular = 0x7F000000;

    float LightMod = LIGHT_MOD;

    // Here's where vertex shading is possible.
    if (!CurrTexture)
    {
        for (int i = 0; i < 3; i++)
        {
            Diffuse[i] = 0;
            Diffuse[i] |= (BYTE)(CurrSurfaceColor.b * 255.0f * LightMod);
            Diffuse[i] |= (BYTE)(CurrSurfaceColor.g * 255.0f * LightMod) << 8;
            Diffuse[i] |= (BYTE)(CurrSurfaceColor.r * 255.0f * LightMod) << 16;
            Diffuse[i] |= (BYTE)(CurrAlpha) << 24;
        }
    }
    else
    {
        for (int i = 0; i < 3; i++)
        {
            Diffuse[i] = 0;
            Diffuse[i] |= (BYTE)(255.0f * LightMod);
            Diffuse[i] |= (BYTE)(255.0f * LightMod) << 8;
            Diffuse[i] |= (BYTE)(255.0f * LightMod) << 16;
            Diffuse[i] |= (BYTE)(CurrAlpha) << 24;
        }
    }

    struct ACRD3DVertex
    {
        Vector Origin;
#ifdef INCLUDE_NORMAL_DATA
        Vector Normal;
#endif
        DWORD Diffuse;
        DWORD Specular;
        float TU, TV;
    };

    ACRD3DVertex VertexBuffer[3];

    for (int i = 0; i < 3; i++)
    {
        VertexBuffer[i].Origin = pVertices[i]->origin;
#ifdef INCLUDE_NORMAL_DATA
        VertexBuffer[i].Normal = pVertices[i]->normal;
#endif
        VertexBuffer[i].Diffuse = Diffuse[i];
        VertexBuffer[i].Specular = Specular;

        if (CurrTexture)
        {
            VertexBuffer[i].TU = pVertices[i]->uvarray[ CurrUVIndices[i] ].u;
            VertexBuffer[i].TV = pVertices[i]->uvarray[ CurrUVIndices[i] ].v;
        }
    }

#ifdef INCLUDE_NORMAL_DATA
    g_pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1);
#else
    g_pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1);
#endif

    g_pD3DDevice->DrawPrimitiveUP(
        D3DPT_TRIANGLELIST, 1, VertexBuffer, sizeof(ACRD3DVertex));
}

// These vertices received would normally be transformed (and in a different class)
void Render::DrawQuadPrim(CSWVertex **pVertices)
{
    D3DCOLOR Diffuse[4], Specular = 0xFF000000;

    float LightMod = LIGHT_MOD;

    // Here's where vertex shading is possible.
    if (!CurrTexture)
    {
        for (int i = 0; i < 4; i++)
        {
            Diffuse[i] = 0;
            Diffuse[i] |= (BYTE)(CurrSurfaceColor.b * 255.0f * LightMod);
            Diffuse[i] |= (BYTE)(CurrSurfaceColor.g * 255.0f * LightMod) << 8;
            Diffuse[i] |= (BYTE)(CurrSurfaceColor.r * 255.0f * LightMod) << 16;
            Diffuse[i] |= (BYTE)(CurrAlpha) << 24;
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            Diffuse[i] = 0;
            Diffuse[i] |= (BYTE)(255.0f * LightMod);
            Diffuse[i] |= (BYTE)(255.0f * LightMod) << 8;
            Diffuse[i] |= (BYTE)(255.0f * LightMod) << 16;
            Diffuse[i] |= (BYTE)(CurrAlpha) << 24;
        }
    }

    struct ACRD3DVertex
    {
        Vector Origin;
#ifdef INCLUDE_NORMAL_DATA
        Vector Normal;
#endif
        DWORD Diffuse;
        DWORD Specular;
        float TU, TV;
    };

    ACRD3DVertex VertexBuffer[4];

    for (int i = 0; i < 4; i++)
    {
        VertexBuffer[i].Origin = pVertices[i]->origin;
#ifdef INCLUDE_NORMAL_DATA
        VertexBuffer[i].Normal = pVertices[i]->normal;
#endif
        VertexBuffer[i].Diffuse = Diffuse[i];
        VertexBuffer[i].Specular = Specular;

        if (CurrTexture)
        {
            VertexBuffer[i].TU = pVertices[i]->uvarray[ CurrUVIndices[i] ].u;
            VertexBuffer[i].TV = pVertices[i]->uvarray[ CurrUVIndices[i] ].v;
        }
    }

#ifdef INCLUDE_NORMAL_DATA
    g_pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1);
#else
    g_pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1);
#endif

    g_pD3DDevice->DrawPrimitiveUP(
        D3DPT_TRIANGLEFAN, 2, VertexBuffer, sizeof(ACRD3DVertex));
}

// These vertices received would normally be transformed (and in a different class)
void Render::DrawMultiPrim(CSWVertex **pVertices, DWORD VertexCount)
{
    if (VertexCount > 32)
    {
        DEBUGOUT("Too many vertices on multiprimitive!\r\n");
        VertexCount = 32;
    }

    D3DCOLOR Diffuse[32], Specular = 0xFF000000;

    float LightMod = LIGHT_MOD;

    // Here's where vertex shading is possible.
    if (!CurrTexture)
    {
        for (DWORD i = 0; i < VertexCount; i++)
        {
            Diffuse[i] = 0;
            Diffuse[i] |= (BYTE)(CurrSurfaceColor.b * 255.0f * LightMod);
            Diffuse[i] |= (BYTE)(CurrSurfaceColor.g * 255.0f * LightMod) << 8;
            Diffuse[i] |= (BYTE)(CurrSurfaceColor.r * 255.0f * LightMod) << 16;
            Diffuse[i] |= (BYTE)(CurrAlpha) << 24;
        }
    }
    else
    {
        for (DWORD i = 0; i < VertexCount; i++)
        {
            Diffuse[i] = 0;
            Diffuse[i] |= (BYTE)(255.0f * LightMod);
            Diffuse[i] |= (BYTE)(255.0f * LightMod) << 8;
            Diffuse[i] |= (BYTE)(255.0f * LightMod) << 16;
            Diffuse[i] |= (BYTE)(CurrAlpha) << 24;
        }
    }

    struct ACRD3DVertex
    {
        Vector Origin;
#ifdef INCLUDE_NORMAL_DATA
        Vector Normal;
#endif
        DWORD Diffuse;
        DWORD Specular;
        float TU, TV;
    };

    ACRD3DVertex VertexBuffer[32];

    for (DWORD i = 0; i < VertexCount; i++)
    {
        VertexBuffer[i].Origin = pVertices[i]->origin;
#ifdef INCLUDE_NORMAL_DATA
        VertexBuffer[i].Normal = pVertices[i]->normal;
#endif
        VertexBuffer[i].Diffuse = Diffuse[i];
        VertexBuffer[i].Specular = Specular;

        if (CurrTexture)
        {
            VertexBuffer[i].TU = pVertices[i]->uvarray[ CurrUVIndices[i] ].u;
            VertexBuffer[i].TV = pVertices[i]->uvarray[ CurrUVIndices[i] ].v;
        }
    }

#ifdef INCLUDE_NORMAL_DATA
    g_pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1);
#else
    g_pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1);
#endif

    g_pD3DDevice->DrawPrimitiveUP(
        D3DPT_TRIANGLEFAN, VertexCount - 2, VertexBuffer, sizeof(ACRD3DVertex));
}

void Render::Draw3DPoint(const Vector& Center, float Size)
{
    g_pD3DDevice->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
    g_pD3DDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
    g_pD3DDevice->SetRenderState(D3DRS_POINTSIZE, FloatToDWORD(Size));
    g_pD3DDevice->SetRenderState(D3DRS_POINTSIZE_MIN, FloatToDWORD(0.0f));
    g_pD3DDevice->SetRenderState(D3DRS_POINTSCALE_A, FloatToDWORD(0.0f));
    g_pD3DDevice->SetRenderState(D3DRS_POINTSCALE_B, FloatToDWORD(0.0f));
    g_pD3DDevice->SetRenderState(D3DRS_POINTSCALE_C, FloatToDWORD(1.0f));

    struct PointVertex
    {
        Vector    Origin;
        DWORD    Diffuse;
    };

    const DWORD PointFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

    if (CurrD3DTexture != NULL)
    {
        g_pD3DDevice->SetTexture(0, NULL);
        CurrD3DTexture = NULL;
    }

    PointVertex VertexBuffer[4];

    VertexBuffer[0].Origin = Center;
    VertexBuffer[0].Diffuse = D3DCOLOR_XRGB(255, 255, 0);

    g_pD3DDevice->DrawPrimitiveUP(D3DPT_POINTLIST, 1, VertexBuffer, sizeof(PointVertex));
}

void Render::Draw3DBox(const Vector& Center, const Vector& Dimensions)
{
    Draw3DPoint(Center, Dimensions.x);
    return;

    struct BoxVertex
    {
        Vector Origin;
        DWORD Diffuse;
        DWORD Specular;
    };

    const DWORD BoxDiffuse    = D3DCOLOR_RGBA(0, 0, 0, 0);
    const DWORD BoxSpecular = D3DCOLOR_RGBA(0, 0, 0, 255);

    BoxVertex VertexBuffer[8];

    Vector HalfDimension = Dimensions * 0.5;

    // Back, Top, Right
    VertexBuffer[0].Origin.x = Center.x + HalfDimension.x;
    VertexBuffer[0].Origin.y = Center.y + HalfDimension.y;
    VertexBuffer[0].Origin.z = Center.z + HalfDimension.z;

    // Back, Top, Left
    VertexBuffer[1].Origin.x = Center.x - HalfDimension.x;
    VertexBuffer[1].Origin.y = Center.y + HalfDimension.y;
    VertexBuffer[1].Origin.z = Center.z + HalfDimension.z;

    // Back, Bottom, Left
    VertexBuffer[2].Origin.x = Center.x - HalfDimension.x;
    VertexBuffer[2].Origin.y = Center.y - HalfDimension.y;
    VertexBuffer[2].Origin.z = Center.z + HalfDimension.z;

    // Back, Bottom, Right
    VertexBuffer[3].Origin.x = Center.x + HalfDimension.x;
    VertexBuffer[3].Origin.y = Center.y - HalfDimension.y;
    VertexBuffer[3].Origin.z = Center.z + HalfDimension.z;

    // Front, Top, Right
    VertexBuffer[4].Origin.x = Center.x + HalfDimension.x;
    VertexBuffer[4].Origin.y = Center.y + HalfDimension.y;
    VertexBuffer[4].Origin.z = Center.z - HalfDimension.z;

    // Front, Top, Left
    VertexBuffer[5].Origin.x = Center.x - HalfDimension.x;
    VertexBuffer[5].Origin.y = Center.y + HalfDimension.y;
    VertexBuffer[5].Origin.z = Center.z - HalfDimension.z;

    // Front, Bottom, Left
    VertexBuffer[6].Origin.x = Center.x - HalfDimension.x;
    VertexBuffer[6].Origin.y = Center.y - HalfDimension.y;
    VertexBuffer[6].Origin.z = Center.z - HalfDimension.z;

    // Front, Bottom, Right
    VertexBuffer[7].Origin.x = Center.x + HalfDimension.x;
    VertexBuffer[7].Origin.y = Center.y - HalfDimension.y;
    VertexBuffer[7].Origin.z = Center.z - HalfDimension.z;

    for (DWORD i = 0; i < 8; i++)
    {
        VertexBuffer[i].Diffuse        = BoxDiffuse;
        VertexBuffer[i].Specular    = BoxSpecular;
    }

    WORD VertexIndices[24] = {
        0, 1, 2, 3, // Back
        4, 5, 6, 7, // Front
        0, 1, 5, 4, // Top
        2, 3, 7, 6, // Bottom
        1, 2, 6, 5, // Left
        0, 3, 7, 4, // Right
    };

    if (CurrD3DTexture != NULL)
    {
        g_pD3DDevice->SetTexture(0, NULL);
        CurrD3DTexture = NULL;
    }

    g_pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR);

    const DWORD BoxType = 1;

    // draw box with triangles
    if (BoxType == 1)
    {
        for (DWORD i = 0; i < 24; i += 4)
        {
            g_pD3DDevice->DrawIndexedPrimitiveUP(
                D3DPT_TRIANGLEFAN, i, 4, 2, &VertexIndices, D3DFMT_INDEX16, &VertexBuffer, sizeof(BoxVertex));
        }
    }
}

int Render::checkplanes(CSphere& sphere, DWORD* flags)
{
    return 2;
}

BOOL Render::viewconeCheck(CSphere *pSphere)
{
    Vector ScaledOrigin = pSphere->m_origin * ObjectScale;
    Vector SphereOffset = ViewerPos.localtoglobal(FrameCurrent->m_Position, ScaledOrigin);

    LocalObjectCenter = ViewerPos.m_Frame.globaltolocal(SphereOffset);
    LocalObjectRadius = pSphere->m_radius * ObjectScale;

    float dp = ViewPlane.dot_product(SphereOffset);

    if (dp < (-LocalObjectRadius))
        return FALSE;

    // MISSING CODE HERE! (Goes through portal list)

    return TRUE;
}

void Render::InitFrameStack()
{
    
}

D3DXMATRIX *Render::GetTopMatrix()
{
    return MatrixStack->GetTop();
}

XFormFrame *Render::GetFrameCurrent()
{
    return FrameCurrent;
}

void Render::positionPush(DWORD Ops, Position *Pos)
{
    if (Ops & 1)
    {
        // Push Frame
        /*
        D3DXMATRIX Mat;
        Pos->m_Frame.MakeD3DMatrix(&Mat, CurrObjectScale);

        MatrixStack->Push();
        MatrixStack->LoadMatrix(&Mat);
        */

        if (!(FramePushCount++))
        {
            FrameCurrent = &FrameStack[0];
            FrameStack[0].m_Position = *Pos;
        }
        else
        {
            // missing code here
            FrameCurrent++;
            FrameCurrent->m_Position = *Pos;
        }
    }

    if (Ops & 2)
    {
        // Update ObjectMatrix
        // SetWorldMatrix(MatrixStack->GetTop());

        FrameCurrent->m_LocalView.m_origin =
            FrameCurrent->m_Position.localtolocal(ViewerPos, Vector(0, 0, 0));

        //FrameCurrent->m_LocalView.m_origin =
        //    *FrameCurrent->m_Position.m_Frame.localtolocal(&LocalViewOrigin, ViewerPos.m_Frame, Offset);

        FrameCurrent->m_LocalView.m_origin /= ObjectScaleVec;
        FrameCurrent->m_ObjectScaleVec    = ObjectScaleVec;
        FrameCurrent->m_ObjectScale        = ObjectScale;

        // Missing code that adds blockoffset to frame x/y/z
        Vector BlockOffset =
            LandDefs::get_block_offset(ViewerPos.m_LandCell, FrameCurrent->m_Position.m_LandCell);

        FrameCurrent->m_WorldOrigin = BlockOffset + FrameCurrent->m_Position.m_Frame.m_origin;

        CalcObjectMatrix();
    }
}

void Render::positionPop()
{
    // MatrixStack->Pop();
    if (--FramePushCount)
    {
        FrameCurrent--;

        // Missing code that sets frame era..

        // This isn't right..
        ObjectScale = FrameCurrent->m_ObjectScale;
        ObjectScaleVec = Vector(
            FrameCurrent->m_ObjectScale,
            FrameCurrent->m_ObjectScale,
            FrameCurrent->m_ObjectScale);

        CalcObjectMatrix();
    }
    else
    {
        FrameCurrent = NULL;

        // Missing code that sets frame era..

        ObjectScale = 1.0;
        ObjectScaleVec = Vector(1.0, 1.0, 1.0);
    }
}

LightManager::LightManager() : m_LightData(64, 64)
{
    m_LightCount = 0;
    m_EnabledLights = 0;
}

LightManager::~LightManager()
{
    for (DWORD i = 0; i < m_LightCount; i++)
    {
        if (m_LightData.array_data[i])
            delete m_LightData.array_data[i];
    }

    m_LightCount = 0;
}

void LightManager::Init()
{
    // Disable all lights, initially
    for (DWORD i = 0; i < MAX_ACTIVE_LIGHTS; i++)
        g_pD3DDevice->LightEnable(i, FALSE);

    m_EnabledLights = 0;
}

void LightManager::AddLight(LIGHTOBJ *Light, Position *pPos)
{
    if (m_LightCount >= m_LightData.alloc_size)
        m_LightData.grow(m_LightData.alloc_size + 10);

    LightData *L = new LightData;

    // D3DLight9 gets constructed later on.
    L->m_LightObj = Light;
    L->m_CellPosition = pPos;

    if (IsBadReadPtr(L->m_LightObj, sizeof(LIGHTOBJ)))
        _asm int 3;

    m_LightData.array_data[ m_LightCount++ ] = L;
}

void LightManager::RemoveLight(LIGHTOBJ *Light)
{
    for (DWORD i = 0; i < m_LightCount; i++)
    {
        if (Light == m_LightData.array_data[i]->m_LightObj)
        {
            delete m_LightData.array_data[i];

            m_LightData.array_data[i] = m_LightData.array_data[ --m_LightCount ];

            if ((m_LightCount+20) < m_LightData.alloc_size)
                m_LightData.shrink(m_LightCount+10);

            break;
        }
    }
}

void LightManager::DisableAll()
{
    for (DWORD i = 0; i < m_EnabledLights; i++)
        g_pD3DDevice->LightEnable(i, FALSE);

    m_EnabledLights = 0;
}

void LightManager::EnableAll()
{
    for (DWORD i = 0; i < m_LightCount; i++)
        EnableLight(m_LightData.array_data[i]);
}

void LightManager::EnableAllInCell(DWORD Cell)
{
    for (DWORD i = 0; i < m_LightCount; i++)
    {
        if (Cell == m_LightData.array_data[i]->m_CellPosition->m_LandCell)
            EnableLight(m_LightData.array_data[i]);
    }
}

void LightManager::EnableLight(LightData *Light)
{
    if (!RenderPreferences::RenderLights || (Render::FullBrightOverride && RenderPreferences::RenderFullbrightOutsideCells))
        return;

    if (m_EnabledLights >= MAX_ACTIVE_LIGHTS)
        return;

    if (IsBadReadPtr(Light, sizeof(LIGHTOBJ)))
        _asm int 3;

    D3DLIGHT9*    d3dlight9 = &Light->m_D3DLight9;
    ZeroMemory(d3dlight9, sizeof(d3dlight9));

    d3dlight9->Type            = D3DLIGHT_POINT;
    d3dlight9->Ambient        = D3DXCOLOR(1.0, 1.0, 1.0, 0.0); 
    d3dlight9->Diffuse        = D3DXCOLOR(1.0, 1.0, 1.0, 0.0); 
    d3dlight9->Specular        = D3DXCOLOR(1.0, 1.0, 1.0, 0.0); 
    d3dlight9->Attenuation0 = 0.0;
    d3dlight9->Attenuation1 = 1.0;
    d3dlight9->Attenuation2 = 0.0;
    d3dlight9->Range        = 10.0;
    d3dlight9->Falloff        = 1.0f;

    d3dlight9->Phi            = D3DX_PI;
    d3dlight9->Theta        = D3DX_PI / 2;
    d3dlight9->Direction    = D3DXVECTOR3(0.0, 0.0, 0.0);
    d3dlight9->Range        = Light->m_LightObj->m_LightInfo->m_54 * (1.0 + (RenderPreferences::LightMod / 20.0));
    // d3dlight9->Ambient        = D3DXCOLOR(Random::RollDice(0.0, 1.0), Random::RollDice(0.0, 1.0), Random::RollDice(0.0, 1.0), 1.0);
    d3dlight9->Ambient        = D3DXCOLOR(1.0, 1.0, 1.0, 1.0);

	 //float val = (((DWORD)Light * 777) & 0xff);
	 //float cosf = cos((Time::GetTimeElapsed()* 3.1415) + val);

	 //d3dlight9->Ambient      = D3DXCOLOR((cosf / 4.0) + 0.75, (cosf / 8.0) + 0.45, 0.05, (cosf / 4.0) + 0.75);
    d3dlight9->Diffuse.r    = Light->m_LightObj->m_LightInfo->m_RGBColor.m_fRed;
    d3dlight9->Diffuse.g    = Light->m_LightObj->m_LightInfo->m_RGBColor.m_fGreen;
    d3dlight9->Diffuse.b    = Light->m_LightObj->m_LightInfo->m_RGBColor.m_fBlue;
    d3dlight9->Diffuse.a    = 1.0;
    d3dlight9->Specular.r    = Light->m_LightObj->m_LightInfo->m_RGBColor.m_fRed;
    d3dlight9->Specular.g    = Light->m_LightObj->m_LightInfo->m_RGBColor.m_fGreen;
    d3dlight9->Specular.b    = Light->m_LightObj->m_LightInfo->m_RGBColor.m_fBlue;
    d3dlight9->Specular.a    = 1.0;
    d3dlight9->Position.x    = Light->m_LightObj->m_Frame.m_origin.x;
    d3dlight9->Position.y    = Light->m_LightObj->m_Frame.m_origin.y;
    d3dlight9->Position.z    = Light->m_LightObj->m_Frame.m_origin.z;

    // Light->m_Composite.combine(Light->m_CellFrame, &Light->m_LightObj->m_Frame);
    // Light->m_Composite.combine(&Light->m_Composite, &Light->m_LightObj->m_LightInfo->m_Frame);
    // Light->m_Composite.combine(Light->m_CellFrame, &Light->m_LightObj->m_Frame);
    Light->m_Composite.combine(&Light->m_LightObj->m_Frame, &Light->m_LightObj->m_LightInfo->m_Frame);

    /*
    DEBUGOUT("CellFrame: ");
    DEBUGOUTFRAME(Light->m_CellFrame);
    DEBUGOUT("\r\nLightObjFrame: ");
    DEBUGOUTFRAME(&Light->m_LightObj->m_Frame);
    DEBUGOUT("\r\nLightInfoFrame: ");
    DEBUGOUTFRAME(&Light->m_LightObj->m_LightInfo->m_Frame);
    DEBUGOUT("\r\nResult: ");
    DEBUGOUTFRAME(&Light->m_Composite);
    DEBUGOUT("\r\n");
    */

    long X, Y;

    if (LandDefs::blockid_to_lcoord(Light->m_CellPosition->m_LandCell, X, Y))
    {
        d3dlight9->Position.x    = (X * LandDefs::square_length) + Light->m_Composite.m_origin.x;
        d3dlight9->Position.y    = Light->m_Composite.m_origin.z;
        d3dlight9->Position.z    = (Y * LandDefs::square_length) + Light->m_Composite.m_origin.y;
    }
    else
    {
        d3dlight9->Position.x    = Light->m_Composite.m_origin.x;
        d3dlight9->Position.y    = Light->m_Composite.m_origin.z;
        d3dlight9->Position.z    = Light->m_Composite.m_origin.y;
    }

    Light->m_Origin.x = d3dlight9->Position.x;
    Light->m_Origin.y = d3dlight9->Position.y;
    Light->m_Origin.z = d3dlight9->Position.z;

    // d3dlight9->Position.x = Render::ViewerPos.m_Frame.m_origin.x;
    // d3dlight9->Position.y = Render::ViewerPos.m_Frame.m_origin.z;
    // d3dlight9->Position.z = Render::ViewerPos.m_Frame.m_origin.y;

    if (FAILED(g_pD3DDevice->SetLight(m_EnabledLights, d3dlight9)))
        DEBUGOUT("SetLight() failed\r\n");

    if (FAILED(g_pD3DDevice->LightEnable(m_EnabledLights++, TRUE)))
        DEBUGOUT("LightEnable() failed\r\n");
}



Position        Render::ViewerPos;
Vector            Render::ViewOrigin;
Vector            Render::Xaxis;
Vector            Render::Yaxis;
Vector            Render::Zaxis;
Plane            Render::ViewPlane;
D3DMATRIX        Render::ViewMatrix;

BOOL            Render::m_CheckSelection = TRUE;
BOOL            Render::m_CheckCurrObjectPolys = TRUE;
float            Render::m_CurrDepth = FLT_MAX;
Vector            Render::m_SelectionRay;
long            Render::m_SelectionX;
long            Render::m_SelectionY;

float            Render::m_DegMul;
float            Render::particle_distance_2dsq = 0.0f;
float            Render::object_distance_2dsq = 0.0f;

float            Render::m_FOV = 1.5707999;
float            Render::m_ZNear = 9.9999998e-3;
float            Render::m_ZFar = 1.0e3;//1.0e2;
float            Render::m_Scale = 4.0e3;
float            Render::m_XInvScale;
float            Render::m_YInvScale;
float            Render::m_TX;
float            Render::m_TY;
float            Render::m_VDst = 6.25e-2;
float            Render::m_BoxWidth;
float            Render::m_BoxHeight;
D3DMATRIX        Render::ProjMatrix;

Vector            Render::LocalObjectCenter;
float            Render::LocalObjectRadius;

Vector            Render::ObjectScaleVec = Vector(1.0, 1.0, 1.0);
float            Render::ObjectScale = 1.0;
D3DMATRIX        Render::ObjectMatrix;

void Render::update_viewpoint(const Frame& frame)
{
    Position NullCell;
    NullCell.m_Frame = frame;

    update_viewpoint(NullCell);
}

void Render::update_viewpoint(const Position& pos)
{
    ViewerPos = pos;

    Xaxis.x = ViewerPos.m_Frame.m00;
    Xaxis.y = ViewerPos.m_Frame.m01;
    Xaxis.z = ViewerPos.m_Frame.m02;

    Yaxis.x = ViewerPos.m_Frame.m10;
    Yaxis.y = ViewerPos.m_Frame.m11;
    Yaxis.z = ViewerPos.m_Frame.m12;

    Zaxis.x = ViewerPos.m_Frame.m20;
    Zaxis.y = ViewerPos.m_Frame.m21;
    Zaxis.z = ViewerPos.m_Frame.m22;

    ViewOrigin = ViewerPos.m_Frame.m_origin;

    ViewPlane.m_normal.x    = ViewerPos.m_Frame.m10;
    ViewPlane.m_normal.y    = ViewerPos.m_Frame.m11;
    ViewPlane.m_normal.z    = ViewerPos.m_Frame.m12;
    ViewPlane.m_dist =
        -(
        (ViewOrigin.x * ViewerPos.m_Frame.m10) + 
        (ViewOrigin.y * ViewerPos.m_Frame.m11) +
        (ViewOrigin.z * ViewerPos.m_Frame.m12)
        ) - m_ZNear;

    UpdateViewpointInternal();
    // UpdateLightsInternal();

    if (m_CheckSelection)
        m_SelectionRay = pick_ray(m_SelectionX, m_SelectionY);
}

Vector Render::pick_ray(long SelX, long SelY)
{
    Vector yaxis_vdst = Yaxis * m_VDst;
    Vector xaxis_xinv = Xaxis * (((double)SelX * m_XInvScale) - m_TX);
    Vector zaxis_yinv = Zaxis * (((double)SelY * m_YInvScale) - m_TY);
    
    Vector ray = ((yaxis_vdst + xaxis_xinv) - zaxis_yinv).normalize();

    return ray;
}

void Render::set_selection_cursor(long SelX, long SelY, BOOL CheckPolys)
{
    m_SelectionX = SelX;
    m_SelectionY = SelY;
    m_CheckSelection = TRUE;
    m_CheckCurrObjectPolys = CheckPolys;
}

void Render::clear_selection_cursor(void)
{
    m_CheckSelection = FALSE;
    m_CheckCurrObjectPolys = FALSE;
    m_CurrDepth = FLT_MAX;
}

BOOL Render::GfxObjUnderSelectionRay(CGfxObj *pObj, float *dist)
{
    *dist = 0;

    float dp1 = m_SelectionRay.dot_product(*((const Vector *)&FrameCurrent->m_Position.m_Frame.m00));
    float dp2 = m_SelectionRay.dot_product(*((const Vector *)&FrameCurrent->m_Position.m_Frame.m10));
    float dp3 = m_SelectionRay.dot_product(*((const Vector *)&FrameCurrent->m_Position.m_Frame.m20));

    Ray screen_ray;
    screen_ray.m_origin = FrameCurrent->m_LocalView.m_origin;
    screen_ray.m_direction = Vector(dp1, dp2, dp3);

    if (pObj->m_RenderSphere->sphere_intersects_ray(screen_ray))
    {
        if (m_CheckCurrObjectPolys)
        {
            for (DWORD i = 0; i < pObj->m_RenderTriangleCount; i++)
            {
                float time;

                if (pObj->m_RenderTriangles[i].polygon_hits_ray(screen_ray, &time))
                    return TRUE;
            }

            return FALSE;
        }

        return TRUE;
    }

    return FALSE;
}

void Render::UpdateViewpointInternal()
{
    Vector ViewLocalized =
        ViewerPos.m_Frame.globaltolocal(Vector(0, 0, 0));

    /*
    double inv_vdst = 1 / m_VDst;
    ViewMatrix._11 = (0.5 * m_BoxWidth * Yaxis.x * inv_vdst) + (Xaxis.x * m_Scale);
    ViewMatrix._21 = (0.5 * m_BoxHeight * Yaxis.x * inv_vdst) - (Zaxis.x * m_Scale);
    ViewMatrix._31 = Yaxis.x * inv_vdst;
    ViewMatrix._41 = Yaxis.x * inv_vdst;

    ViewMatrix._12 = (0.5 * m_BoxWidth * Yaxis.y * inv_vdst) + (Xaxis.y * m_Scale);
    ViewMatrix._22 = (0.5 * m_BoxHeight * Yaxis.y * inv_vdst) - (Zaxis.y * m_Scale);
    ViewMatrix._32 = Yaxis.y * inv_vdst;
    ViewMatrix._42 = Yaxis.y * inv_vdst;

    ViewMatrix._13 = (0.5 * m_BoxWidth * Yaxis.z * inv_vdst) + (Xaxis.z * m_Scale);
    ViewMatrix._23 = (0.5 * m_BoxHeight * Yaxis.z * inv_vdst) - (Zaxis.z * m_Scale);
    ViewMatrix._33 = Yaxis.z * inv_vdst;
    ViewMatrix._43 = Yaxis.z * inv_vdst;

    ViewMatrix._14 = (0.5 * m_BoxWidth * ViewLocalized.y * inv_vdst) + (ViewLocalized.x * m_Scale);
    ViewMatrix._24 = (0.5 * m_BoxHeight * ViewLocalized.y * inv_vdst) - (ViewLocalized.z * m_Scale);
    ViewMatrix._34 = (m_ZNear + ViewLocalized.y) * inv_vdst;
    ViewMatrix._44 = ViewLocalized.y * inv_vdst;
    */

    ViewMatrix._11 = Xaxis.x;
    ViewMatrix._21 = Xaxis.z;
    ViewMatrix._31 = Xaxis.y;
    ViewMatrix._41 = ViewLocalized.x;

    ViewMatrix._12 = Zaxis.x;
    ViewMatrix._22 = Zaxis.z;
    ViewMatrix._32 = Zaxis.y;
    ViewMatrix._42 = ViewLocalized.z;

    ViewMatrix._13 = Yaxis.x;
    ViewMatrix._23 = Yaxis.z;
    ViewMatrix._33 = Yaxis.y;
    ViewMatrix._43 = ViewLocalized.y;

    ViewMatrix._14 = 0.0f;
    ViewMatrix._24 = 0.0f;
    ViewMatrix._34 = 0.0f;
    ViewMatrix._44 = 1.0f;

    SetViewMatrix(&ViewMatrix);
}

void Render::UpdateBoxSize()
{
    RECT WindowRect    = GetRenderRect();

    m_BoxWidth        = (float)(WindowRect.right - WindowRect.left);
    m_BoxHeight        = (float)(WindowRect.bottom - WindowRect.top);
}

BOOL Render::SetFOV(float _fov_degree)
{
    if (_fov_degree < 0 || _fov_degree >= 180)
        return FALSE;

    SetFOVInternal(D3DXToRadian(_fov_degree));
    return TRUE;
}

void Render::SetFOVInternal(float _fov_radian)
{
    // Base call does this.
    UpdateBoxSize();

    m_XInvScale = 1 / m_Scale;
    m_YInvScale = 1 / m_Scale;
    m_TX = m_XInvScale * 0.5f * (m_BoxWidth - 1);
    m_TY = m_YInvScale * 0.5f * (m_BoxHeight - 1);
    m_VDst = m_TX / tan(_fov_radian * 0.5);

    // Then SW style does this.
    // W_TO_Z_SCALE    = m_VDst / m_ZFar;
    // WI_SCALE        = 1 / m_VDst;

    // D3D style does this:
    float yScale = 1 / tan(_fov_radian / 2); // 1 / tan() = cotangent

    ProjMatrix._11 = 1.0;
    ProjMatrix._12 = 0.0;
    ProjMatrix._13 = 0.0;
    ProjMatrix._14 = 0.0;
    
    ProjMatrix._21 = 0.0;
    ProjMatrix._22 = m_BoxWidth / m_BoxHeight;
    ProjMatrix._23 = 0.0;
    ProjMatrix._24 = 0.0;

    ProjMatrix._31 = 0.0;
    ProjMatrix._32 = 0.0;
    ProjMatrix._33 = m_ZFar / (m_ZFar - m_VDst);
    ProjMatrix._34 = 1 / yScale;

    ProjMatrix._41 = 0.0;
    ProjMatrix._42 = 0.0;
    ProjMatrix._43 = -yScale;
    ProjMatrix._44 = 0.0;

    SetProjMatrix(&ProjMatrix);

    // D3DXMATRIX Ortho;
    // D3DXMatrixOrthoOffCenterLH(&Ortho, -40, 40, -40, 40, m_VDst, m_ZFar);
    // SetProjMatrix(&Ortho);
}

void Render::SetObjectScale(Vector *pScale)
{
    ObjectScaleVec = *pScale;

    // Largest axis is scale.
    ObjectScale = ObjectScaleVec.x;

    if (ObjectScaleVec.y > ObjectScale)
        ObjectScale = ObjectScaleVec.y;

    if (ObjectScaleVec.z > ObjectScale)
        ObjectScale = ObjectScaleVec.z;
}

void Render::CalcObjectMatrix()
{
    ObjectMatrix._11 = ObjectScaleVec.x * FrameCurrent->m_Position.m_Frame.m[0][0];
    ObjectMatrix._21 = ObjectScaleVec.y * FrameCurrent->m_Position.m_Frame.m[1][0];
    ObjectMatrix._31 = ObjectScaleVec.z * FrameCurrent->m_Position.m_Frame.m[2][0];
    ObjectMatrix._41 = FrameCurrent->m_WorldOrigin.x;

    ObjectMatrix._12 = ObjectScaleVec.x * FrameCurrent->m_Position.m_Frame.m[0][2];
    ObjectMatrix._22 = ObjectScaleVec.y * FrameCurrent->m_Position.m_Frame.m[1][2];
    ObjectMatrix._32 = ObjectScaleVec.z * FrameCurrent->m_Position.m_Frame.m[2][2];
    ObjectMatrix._42 = FrameCurrent->m_WorldOrigin.z;

    ObjectMatrix._13 = ObjectScaleVec.x * FrameCurrent->m_Position.m_Frame.m[0][1];
    ObjectMatrix._23 = ObjectScaleVec.y * FrameCurrent->m_Position.m_Frame.m[1][1];
    ObjectMatrix._33 = ObjectScaleVec.z * FrameCurrent->m_Position.m_Frame.m[2][1];
    ObjectMatrix._43 = FrameCurrent->m_WorldOrigin.y;    

    ObjectMatrix._14 = 0.0;
    ObjectMatrix._24 = 0.0;
    ObjectMatrix._34 = 0.0;
    ObjectMatrix._44 = 1.0;    

    SetWorldMatrix(&ObjectMatrix);
}

void RenderDevice::UpdateObjCell(CObjCell *pCell)
{
    /* THIS IS NOT THE REAL FUNCTION - Fix me later.
    for (DWORD i = 0; i < pCell->m_ShadowObjCount; i++)
    {
        if (pCell->m_ShadowObjs.array_data[i])
            pCell->m_ShadowObjs.array_data[i]->m_PhysObj->UpdateViewerDistance();
    }
    */

    for (DWORD i = 0; i < pCell->m_ObjectCount; i++)
    {
        if (pCell->m_Objects.array_data[i])
            pCell->m_Objects.array_data[i]->UpdateViewerDistance();
    }
}

void RenderDevice::DrawPartCell(CPartCell *pCell, DWORD ClipPlaneIndex)
{
    // curr_detail_surface = 0;

    for (DWORD i = 0; i < pCell->m_ShadowPartCount; i++)
    {
        CShadowPart *pPart = pCell->m_ShadowParts.array_data[i];

        if (pPart)
            pPart->draw(ClipPlaneIndex);
    }
}

void RenderDevice::DrawBlock(CLandBlock *pLandBlock)
{
    Render::ObjectScaleVec = Vector(1.0, 1.0, 1.0);
    Render::ObjectScale = 1.0;

    Position Pos;
    Pos.m_LandCell = pLandBlock->m_Key;
    Render::positionPush(3, &Pos);

    // PrimSWRender::LandLightCache = pLandBlock->m_LightCache;
    int CellWidth = pLandBlock->m_LBStruct.m_CellWidth;

    for (int i = 0; i < (CellWidth * CellWidth); i++)
    {
        // Insert all the objects as shadow parts
    }

    BOOL fully_detailed = (LandDefs::lblock_side == CellWidth) ? TRUE : FALSE;

    for (int i = 0; i < (CellWidth * CellWidth); i++)
    {
        Render::SetMaterial(NULL);

        // Set the texture (by the region) here.

        Render::ObjectScaleVec = Vector(1.0, 1.0, 1.0);
        Render::ObjectScale = 1.0;

        // Use 0x108 instead for draw ordering

        if (fully_detailed)
        {
            // Render::curr_detail_surface = Render::landscape_detail_surface;
            // Render::curr_detail_tiling = Render::landscape_detail_tiling;
            Render::CurrSrcBlend = D3DBLEND_SRCALPHA;
            Render::CurrDestBlend = D3DBLEND_INVSRCALPHA;
        }
        
        RenderDevice::DrawLandCell(&pLandBlock->m_LBStruct.m_LandCells[i]);

        // Draw sort cell here.
    }

    Render::positionPop();
}

void RenderDevice::DrawLandCell(CLandCell *pLandCell)
{
    Render::NormalZFuncMode = D3DCMP_ALWAYS;
    Render::NormalZWriteMode = FALSE;

    Render::landPolysDraw(pLandCell->m_Polys, LandDefs::polys_per_landcell);

    Render::NormalZFuncMode = D3DCMP_LESSEQUAL;
    Render::NormalZWriteMode = TRUE;
}

void Render::landPolysDraw(CPolygon **Polys, int PolyCount)
{
    // Temp fix
    SetSurfaceArray(NULL);

    for (int i = 0; i < PolyCount; i++)
        landPolyDraw(Polys[i]);
}

void Render::landPolyDraw(CPolygon *Poly)
{
    SetSurface(Poly, 0);

    DrawTrianglePrim(Poly->m_pVertexObjects);
    FramePolyCount += Poly->m_iVertexCount - 2;
}

void Render::SetDegradeLevelInternal(float Mult)
{
    const float IDEAL_OBJECT_SORT_DISTANCE = 25.0f;
    const float IDEAL_PARTICLE_SORT_DISTANCE = 16.0f;
    const float MIN_OBJECT_SORT_DISTANCE = 8.0f;
    const float MIN_PARTICLE_SORT_DISTANCE = 8.0f;
    const float MAX_OBJECT_SORT_DISTANCE = 50.0f;
    const float MAX_PARTICLE_SORT_DISTANCE = 25.0f;

    m_DegMul = Mult;

    if (Mult < 0)
    {
        object_distance_2dsq = IDEAL_OBJECT_SORT_DISTANCE + ((IDEAL_OBJECT_SORT_DISTANCE - MIN_OBJECT_SORT_DISTANCE) * Mult);
        object_distance_2dsq *= object_distance_2dsq;

        particle_distance_2dsq = IDEAL_PARTICLE_SORT_DISTANCE + ((IDEAL_PARTICLE_SORT_DISTANCE - MIN_PARTICLE_SORT_DISTANCE) * Mult);
        particle_distance_2dsq *= particle_distance_2dsq;

        // max_static_lights = 20.0f + (Mult * 10.0f);
        // max_dynamic_lights = 7.0f + (Mult * 2.0f);
    }
    else
    {
        object_distance_2dsq = IDEAL_OBJECT_SORT_DISTANCE - ((IDEAL_OBJECT_SORT_DISTANCE - MAX_OBJECT_SORT_DISTANCE) * Mult);
        object_distance_2dsq *= object_distance_2dsq;

        particle_distance_2dsq = IDEAL_PARTICLE_SORT_DISTANCE - ((IDEAL_PARTICLE_SORT_DISTANCE - MAX_PARTICLE_SORT_DISTANCE) * Mult);
        particle_distance_2dsq *= particle_distance_2dsq;

        // max_static_lights = 20.0f - (Mult * -10.0f);
        // max_dynamic_lights = 7.0f - (Mult * -3.0f);
    }
}







