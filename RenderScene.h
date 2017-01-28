
#pragma once

#include "Vertex.h"
#include "Polygon.h"

class ImgTex;
class Palette;

class SurfaceHUD;
class CSurface;

class CPartArray;

// SceneTypes
enum {
    eSurfaceScene,
    eDungeonScene,
    eDungeonMapScene,
    eGfxObjScene,
    eSetupScene,
    ePhysicsObjScene,
    eLandScapeScene,
    eTestScene,
}; 

class RenderScene
{
public:

    static void Cleanup();
    static void CreateSurfaceScene(DWORD SurfaceID);
    static void CreateGfxObjScene(DWORD GfxObjID);
    static void CreateSetupScene(DWORD SetupID);
    static void CreatePhysicsObjScene(DWORD SetupID);
    static void CreateDungeonScene(DWORD LandBlock);
    static void CreateDungeonMapScene(DWORD LandBlock);
    static void CreateLandScapeScene(DWORD LandBlock);
    static void CreateTestScene();

    static BOOL WndProc(UINT, WPARAM, LPARAM);
    static void RenderCurrent();

    static RenderScene *GetCurrent();
    static RenderScene *SetCurrent(RenderScene *pScene);

    virtual ~RenderScene();
    virtual void Destroy();
    virtual void Reset();
    virtual void Render(double) = 0;
    virtual int SceneType() = 0;

    virtual void Input_KeyDown(BYTE bKey) { }
    virtual void Input_KeyUp(BYTE bKey) { }
    virtual void Input_MouseMove(long x, long y) { }

private:

    static RenderScene *m_pCurrentScene;
    static double        m_fLastFrame;
};

class SurfaceScene :
    public RenderScene
{
public:
    SurfaceScene();
    virtual ~SurfaceScene();
    virtual void Destroy();
    virtual void Reset();

    BOOL Init(DWORD ID);

    virtual int SceneType();
    virtual void Render(double);

    DWORD GetCurrentID(DWORD ID);
    BOOL SetCurrentID(DWORD ID);

    virtual void Input_KeyDown(BYTE bKey);
    virtual void Input_KeyUp(BYTE bKey);

    CSurface *GetCurrentSurface();

private:

    BOOL CreatePolygons();
    BOOL CreateView();

    void GetDimFactors(float *pWidthFactor, float *pHeightFactor);

    D3DXMATRIX m_mWorld; 
    D3DXMATRIX m_mView; 
    D3DXMATRIX m_mProj; 

    LPDIRECT3DVERTEXBUFFER9 m_pSurfaceVB;

    CSurface*        m_pSurface;

    // Polygon for drawing on.
    CVertexArray    m_VertexArray;
    CPolygon        m_Polygon;

    SurfaceHUD*        m_pSurfaceHUD;
};

#include "ModelScene.h"
#include "GfxObjScene.h"
#include "SetupScene.h"
#include "PhysicsObjScene.h"
#include "DungeonScene.h"
#include "DungeonMapScene.h"
#include "LandScapeScene.h"

class TestScene :
    public RenderScene
{
public:
    TestScene();
    virtual ~TestScene();

    BOOL Init();

    BOOL CreatePolygons();
    BOOL CreateView();

    virtual int SceneType();
    virtual void Render(double);

private:

    D3DXMATRIX m_mWorld; 
    D3DXMATRIX m_mView; 
    D3DXMATRIX m_mProj; 

    LPDIRECT3DVERTEXBUFFER9 m_pTestVB;

    class CSurface *m_pSurface;
};












