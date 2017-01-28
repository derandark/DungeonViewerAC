
#include "StdAfx.h"
#include "PartArray.h"
#include "Render.h"
#include "RenderScene.h"

ModelScene::ModelScene()
{
    m_LastCamUpdate = 0;

    memset(&m_Keys, 0, sizeof(m_Keys));
}

ModelScene::~ModelScene()
{
}

void ModelScene::Input_KeyDown(BYTE bKey)
{
    m_Keys[ bKey ] = TRUE;
}

void ModelScene::Input_KeyUp(BYTE bKey)
{
    m_Keys[ bKey ] = FALSE;
}

void ModelScene::MoveCameraWithInput(float FrameTime, BOOL *Keys)
{
    float MoveSpeed = 3.0f;
    float TurnSpeed = D3DXToRadian(80.0);

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
    if (Keys[VK_LEFT])    RotateAngles.z += TurnSpeed * FrameTime;
    if (Keys[VK_RIGHT])    RotateAngles.z -= TurnSpeed * FrameTime;

    // Adjust Tilt/Roll
    if (Keys['Q'])        RotateAngles.y -= TurnSpeed * FrameTime;
    if (Keys['E'])        RotateAngles.y += TurnSpeed * FrameTime;

    // Apply rotation.
    if (RotateAngles.x || RotateAngles.y || RotateAngles.z)
        m_Camera.Rotate(RotateAngles);

    // Apply movement.
    if (MoveDirection.x || MoveDirection.y || MoveDirection.z)
        m_Camera.Move(MoveDirection);
}

void ModelScene::UpdateCamera()
{
    float FrameTime = (float)(Time::GetTimeElapsed() - m_LastCamUpdate);
    MoveCameraWithInput(FrameTime, m_Keys);

    m_LastCamUpdate = Time::GetTimeElapsed();
}







