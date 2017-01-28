
#include "StdAfx.h"
#include "RenderScene.h"

RenderScene*    RenderScene::m_pCurrentScene = NULL;
double            RenderScene::m_fLastFrame = 0.0;    

void RenderScene::Cleanup()
{
    if (m_pCurrentScene)
    {
        delete m_pCurrentScene;
        m_pCurrentScene = NULL;
    }
}

BOOL RenderScene::WndProc(UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message)
    {
    case WM_KEYDOWN:

        if (m_pCurrentScene)
            m_pCurrentScene->Input_KeyDown((BYTE)wParam);

        return TRUE;
    case WM_KEYUP:

        if (m_pCurrentScene)
            m_pCurrentScene->Input_KeyUp((BYTE)wParam);

        return TRUE;
    case WM_LBUTTONDOWN:

        return TRUE;
    case WM_LBUTTONUP:

        return TRUE;
    case WM_MOUSEMOVE:

        WORD x = LOWORD(lParam);
        WORD y = HIWORD(lParam);

        if (m_pCurrentScene)
            m_pCurrentScene->Input_MouseMove(x, y);

        return TRUE;
    }

    return FALSE;
}

void RenderScene::CreateSurfaceScene(DWORD SurfaceID)
{
    DEBUGOUT(__FUNCTION__ "(0x%08X)\r\n", SurfaceID);

    SurfaceScene *pScene = new SurfaceScene();

    if (pScene)
    {
        if (!pScene->Init(SurfaceID))
            DEBUGOUT("Init on SurfaceScene FAILED!\r\n");
    }

    SetCurrent(pScene);
}

void RenderScene::CreateGfxObjScene(DWORD GfxObjID)
{
    DEBUGOUT(__FUNCTION__ "(0x%08X)\r\n", GfxObjID);

    GfxObjScene *pScene = new GfxObjScene();

    if (pScene)
    {
        if (!pScene->Init(GfxObjID))
            DEBUGOUT("Init on GfxObjScene FAILED!\r\n");
    }

    SetCurrent(pScene);
}

void RenderScene::CreateSetupScene(DWORD SetupID)
{
    DEBUGOUT(__FUNCTION__ "(0x%08X)\r\n", SetupID);

    SetupScene *pScene = new SetupScene();

    if (pScene)
    {
        if (!pScene->Init(SetupID))
            DEBUGOUT("Init on SetupScene FAILED!\r\n");
    }

    SetCurrent(pScene);
}

void RenderScene::CreatePhysicsObjScene(DWORD ModelID)
{
    DEBUGOUT(__FUNCTION__ "(0x%08X)\r\n", ModelID);

    PhysicsObjScene *pScene = new PhysicsObjScene();

    if (pScene)
    {
        if (!pScene->Init(ModelID))
            DEBUGOUT("Init on ObjectScene FAILED!\r\n");
    }

    SetCurrent(pScene);
}

void RenderScene::CreateDungeonScene(DWORD LandBlock)
{
    DEBUGOUT(__FUNCTION__ "(0x%08X)\r\n", LandBlock);

    DungeonScene *pScene = new DungeonScene();

    if (pScene)
    {
        if (!pScene->Init(LandBlock))
            DEBUGOUT("Init on DungeonScene FAILED!\r\n");
    }

    SetCurrent(pScene);
}

void RenderScene::CreateDungeonMapScene(DWORD LandBlock)
{
    DEBUGOUT(__FUNCTION__ "(0x%08X)\r\n", LandBlock);

    DungeonMapScene *pScene = new DungeonMapScene();

    if (pScene)
    {
        if (!pScene->Init(LandBlock))
            DEBUGOUT("Init on DungeonMapScene FAILED!\r\n");
    }

    SetCurrent(pScene);
}

void RenderScene::CreateLandScapeScene(DWORD LandBlock)
{
    DEBUGOUT(__FUNCTION__ "(0x%08X)\r\n", LandBlock);

    LandScapeScene *pScene = new LandScapeScene();

    if (pScene)
    {
        if (!pScene->Init(LandBlock))
            DEBUGOUT("Init on LandScapeScene FAILED!\r\n");
    }

    SetCurrent(pScene);
}

void RenderScene::CreateTestScene()
{
    DEBUGOUT(__FUNCTION__ "\r\n");

    TestScene *pScene = new TestScene();

    if (pScene)
    {
        if (!pScene->Init())
            DEBUGOUT("Init on TestScene FAILED!\r\n");
    }

    SetCurrent(pScene);
}

RenderScene *RenderScene::GetCurrent()
{
    return m_pCurrentScene;
}

RenderScene *RenderScene::SetCurrent(RenderScene *pScene)
{
    if (m_pCurrentScene)
    {
        DEBUGOUT("Deleting existing RenderScene..\r\n");
        delete m_pCurrentScene;
    }

    m_fLastFrame = Time::GetTimeElapsed();
    DEBUGOUT("New RenderScene set..\r\n");

    return(m_pCurrentScene = pScene);
}

void RenderScene::RenderCurrent()
{
    if (!m_pCurrentScene)
        return;

    m_pCurrentScene->Render(Time::GetTimeElapsed() - m_fLastFrame);
    m_fLastFrame = Time::GetTimeElapsed();
}

RenderScene::~RenderScene()
{
}

void RenderScene::Destroy()
{
}

void RenderScene::Reset()
{
}



