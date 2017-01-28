
#include "StdAfx.h"
#include "RenderScene.h"
#include "Physics.h"
#include "LandDefs.h"
#include "LandBlock.h"
#include "LandBlockInfo.h"
#include "EnvCell.h"
#include "Environment.h"
#include "Render.h"
#include "PartArray.h"
#include "LandScape.h"

Vector LandScapeScene::camera_pos;

LandScapeScene::LandScapeScene()
{
    m_LastCamUpdate = 0;

    memset(&m_Keys, 0, sizeof(m_Keys));

    m_LandScape = new LScape;
    m_CurrentGid = 0;
}

LandScapeScene::~LandScapeScene()
{
    Destroy();

    delete m_LandScape;
}

void LandScapeScene::Input_KeyDown(BYTE bKey)
{
    if (!m_Keys[ bKey ])
    {
        // Newly pressed.
    }

    m_Keys[ bKey ] = TRUE;
}

void LandScapeScene::Input_KeyUp(BYTE bKey)
{
    m_Keys[ bKey ] = FALSE;
}

void LandScapeScene::Input_MouseMove(long x, long y)
{
    Render::clear_selection_cursor();
    Render::set_selection_cursor(x, y, TRUE);
}

void LandScapeScene::MoveCameraWithInput(float FrameTime, BOOL *Keys)
{
    float MoveSpeed = 600.0f;
    float TurnSpeed = 90.0;

    Vector MoveDirection(0, 0, 0);
    Vector RotateAngles(0, 0, 0);

    // Move Forward/backward
    if (Keys['W'])        MoveDirection.y += FrameTime * MoveSpeed;
    if (Keys['S'])        MoveDirection.y -= FrameTime * MoveSpeed;

    // Move Left/Right (straf)
    if (Keys['A'])        MoveDirection.x -= FrameTime * MoveSpeed;
    if (Keys['D'])        MoveDirection.x += FrameTime * MoveSpeed;

    // Move Up/Down
    if (Keys['R'])        MoveDirection.z += FrameTime * MoveSpeed;
    if (Keys['F'])        MoveDirection.z -= FrameTime * MoveSpeed;

    // Look Up/Down
    if (Keys[VK_UP])        RotateAngles.x += TurnSpeed * FrameTime;
    if (Keys[VK_DOWN])    RotateAngles.x -= TurnSpeed * FrameTime;

    // Look Left/Right
    if (Keys[VK_LEFT])    RotateAngles.y += TurnSpeed * FrameTime;
    if (Keys[VK_RIGHT])    RotateAngles.y -= TurnSpeed * FrameTime;

    // Adjust Tilt/Roll
    if (Keys['Q'])        RotateAngles.z -= TurnSpeed * FrameTime;
    if (Keys['E'])        RotateAngles.z += TurnSpeed * FrameTime;

    // Apply rotation.
    if (RotateAngles.x || RotateAngles.y || RotateAngles.z)
        m_Camera.Rotate(RotateAngles);

    // Apply movement.
    if (MoveDirection.x || MoveDirection.y || MoveDirection.z)
        m_Camera.Move(MoveDirection);
}

void LandScapeScene::UpdateCamera()
{
    float FrameTime = (float)(Time::GetTimeElapsed() - m_LastCamUpdate);
    MoveCameraWithInput(FrameTime, m_Keys);

    m_LastCamUpdate = Time::GetTimeElapsed();

    extern char debug_status_string[512];
    sprintf(debug_status_string, "hello world!");

    long LCX, LCY;
    LCX = Render::ViewOrigin.x / LandDefs::square_length;
    LCY = Render::ViewOrigin.y / LandDefs::square_length;

    DWORD BlockGid = LandDefs::lcoord_to_gid(LCX, LCY);

    if (BlockGid != m_CurrentGid)
    {
        m_LandScape->update_loadpoint(BlockGid);
        m_CurrentGid = BlockGid;
    }
}

int LandScapeScene::SceneType()
{
    return eLandScapeScene;
}

void LandScapeScene::Destroy()
{
}

BOOL LandScapeScene::Init(DWORD LandBlock)
{
    m_LandScape->SetMidRadius(20);
    m_LandScape->update_loadpoint(LandBlock);

    Frame cameraframe;
    long X, Y;

    if (LandDefs::blockid_to_lcoord(LandBlock, X, Y))
    {
        cameraframe.m_origin =
            Vector(
            X * LandDefs::square_length,
            Y * LandDefs::square_length,
            0);
    }

    m_Camera.SetViewPosition(cameraframe);

    return TRUE;
}

void LandScapeScene::Render(double FrameTime)
{
    UpdateCamera();

    g_pD3DDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_ARGB(51, 51, 51, 51));

    Render::SetZFill();
    Render::SetFogState(FALSE);
    Render::SetFilling(D3DFILL_WIREFRAME);
    Render::SetLighting(TRUE);

    Render::pLightManager->DisableAll();
    Render::pLightManager->EnableAll();

    Render::SetCulling(D3DCULL_NONE);

    // Render::SetProjMatrix(m_Camera.GetProjMatrix());
    // Render::SetViewMatrix(m_Camera.GetViewMatrix());
    // Render::SetViewFrame(m_Camera.GetViewFrame());

#ifdef _DEBUG
    extern Vector TestLightOrigin;
    TestLightOrigin = *m_Camera.GetViewPoint();
#endif

    if (m_LandScape)
        m_LandScape->pea_draw();

    // for (DWORD i = 0; i < Render::pLightManager->m_LightCount; i++)
    //    Render::Draw3DPoint(Render::pLightManager->m_LightData.array_data[i]->m_Origin, 1);
}









