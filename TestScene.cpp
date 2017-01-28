

#include "StdAfx.h"
#include "Render.h"
#include "RenderScene.h"
#include "Surface.h"

struct TESTVERTEX
{
    float x, y, z;
    DWORD color;
};

#define TESTVERTEXFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

TestScene::TestScene()
{
    m_pTestVB = NULL;
    m_pSurface = NULL;
}

TestScene::~TestScene()
{
    if (m_pTestVB)
    {
        m_pTestVB->Release();
        m_pTestVB = NULL;
    }

    if (m_pSurface)
    {
        CSurface::Release(m_pSurface);
        m_pSurface = NULL;
    }
}

int TestScene::SceneType()
{
    return eTestScene;
}

BOOL TestScene::Init()
{
    if (!CreateView())
        DEBUGOUT("Failed creating test view.\r\n");
    
    if (!CreatePolygons())
        DEBUGOUT("Failed creating test polygons.\r\n");

    m_pSurface = CSurface::Get(0x08000BEC);

    if (!m_pSurface)
        return FALSE;

    return TRUE;
}

BOOL TestScene::CreateView()
{
    float FOV = D3DXToRadian(90.0f);
    float AspectRatio = 4.0f / 3.0f;
    
    D3DXMatrixPerspectiveFovLH(
        &m_mProj, FOV, AspectRatio, 1.0f, 100.0f);

    D3DXMatrixIdentity(&m_mWorld);

    D3DXMatrixLookAtLH(
        &m_mView,
        &D3DXVECTOR3(0.0f, -1.0f, 1.0f), /* Eye */
        &D3DXVECTOR3(0.0f, 0.0f, 0.0f), /* Target */
        &D3DXVECTOR3(0.0f, 0.0f, 1.0f) /* Up */);

    return TRUE;
}

BOOL TestScene::CreatePolygons()
{
    HRESULT hResult;

    TESTVERTEX Vertices[4] =
    {
        { 0.0f, 0.0f, 0.0f, D3DCOLOR_COLORVALUE(1.0, 0.0, 0.0, 1.0) },
        { 0.0f, 1.0f, 0.0f, D3DCOLOR_COLORVALUE(0.0, 1.0, 0.0, 1.0) },
        { 1.0f, 0.0f, 0.0f, D3DCOLOR_COLORVALUE(0.0, 0.0, 1.0, 1.0) },
        { 1.0f, 1.0f, 0.0f, D3DCOLOR_COLORVALUE(1.0, 0.0, 1.0, 1.0) },
    };

    hResult = g_pD3DDevice->CreateVertexBuffer(
        sizeof(Vertices), /* Buffer Size */
        0, /* Usage */
        TESTVERTEXFVF, /* FVF */
        D3DPOOL_DEFAULT, /* Pool */ 
        &m_pTestVB, /* Output */
        NULL);

    if (FAILED(hResult) || !m_pTestVB)
        return FALSE;

    LPVOID lpMem = NULL;
    hResult = m_pTestVB->Lock(0, sizeof(Vertices), &lpMem, 0);

    if (FAILED(hResult))
        return FALSE;

    memcpy(lpMem, Vertices, sizeof(Vertices));
    
    hResult = m_pTestVB->Unlock();

    if (FAILED(hResult))
        return FALSE;

    return TRUE;
}

void TestScene::Render(double FrameTime)
{
    Render::SetZFill();
    Render::SetFogState(FALSE);
    Render::SetFilling(D3DFILL_WIREFRAME);

    Render::SetLighting(FALSE);
    Render::SetCulling(D3DCULL_NONE);

    Render::SetProjMatrix(&m_mProj);
    Render::SetViewMatrix(&m_mView);
    Render::SetWorldMatrix(&m_mWorld);

    // Render::SetSurface(m_pSurface);

    Render::DrawVBDirect(D3DPT_TRIANGLESTRIP, TESTVERTEXFVF, 2, sizeof(TESTVERTEX), m_pTestVB);
}





