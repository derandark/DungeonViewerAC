
#include "StdAfx.h"
#include "PartArray.h"
#include "Render.h"
#include "Frame.h"
#include "Setup.h"
#include "Animation.h"
#include "PhysicsObj.h"
#include "RenderScene.h"

PhysicsObjScene::PhysicsObjScene()
{
    m_pPhysicsObj = NULL;
}

PhysicsObjScene::~PhysicsObjScene()
{
    Destroy();
}

int PhysicsObjScene::SceneType()
{
    return ePhysicsObjScene;
}

BOOL PhysicsObjScene::Init(DWORD ID)
{
    m_pPhysicsObj = CPhysicsObj::makeObject(ID, 0, TRUE);

    if (!m_pPhysicsObj)
        return FALSE;

    m_pPhysicsObj->set_heading(150.0f, TRUE);

    D3DXMATRIX *pViewMatrix = m_Camera.GetViewMatrix();

    Frame DefaultView;

    DefaultView.euler_set_rotate(Vector(0, 0, 0), 0);
    DefaultView.rotate(Vector(0, 0, 0));
    DefaultView.m_origin = Vector(0.0, -2.2, 1.1);

    m_Camera.SetFOV(90.0f);
    m_Camera.SetViewPosition(DefaultView);
    
    m_pPhysicsObj->set_phstable_id(0x34000004);
    m_pPhysicsObj->set_sequence_animation(0x030000AC, TRUE, 0, 30.0f);

    // Portal storm brewing.
    // 137 = level up?
    m_pPhysicsObj->play_script(140, 1.0f);

    // m_pPhysicsObj->SetTranslucencyHierarchical(0.5f);

    return TRUE;
}

void PhysicsObjScene::set_sequence_animation(DWORD AnimationID, BOOL ClearAnimations, long StartFrame, float FrameRate)
{
    if (!m_pPhysicsObj)
        return;

    m_pPhysicsObj->set_sequence_animation(AnimationID, ClearAnimations, StartFrame, FrameRate);
}

void PhysicsObjScene::Destroy()
{
    if (m_pPhysicsObj)
    {
        delete m_pPhysicsObj;
        m_pPhysicsObj = NULL;
    }
}

void PhysicsObjScene::Render(double FrameTime)
{
    UpdateCamera();

    Render::SetZFill();
    Render::SetFogState(FALSE);
    Render::SetFilling(D3DFILL_SOLID);

    Render::SetLighting(FALSE);
    Render::SetCulling(D3DCULL_NONE);

    // Render::SetProjMatrix(m_Camera.GetProjMatrix());
    // Render::SetViewMatrix(m_Camera.GetViewMatrix());

    if (m_pPhysicsObj)
    {
        m_pPhysicsObj->update_position();
        m_pPhysicsObj->DrawRecursive();
    }
}










