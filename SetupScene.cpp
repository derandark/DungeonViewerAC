
#include "StdAfx.h"
#include "PartArray.h"
#include "Render.h"
#include "Frame.h"
#include "Setup.h"
#include "Animation.h"
#include "RenderScene.h"

SetupScene::SetupScene()
{
    m_pPartArray = NULL;
}

SetupScene::~SetupScene()
{
    Destroy();
}

int SetupScene::SceneType()
{
    return eSetupScene;
}

BOOL SetupScene::Init(DWORD ID)
{
    m_pPartArray = CPartArray::CreateSetup(NULL, ID, TRUE);

    if (!m_pPartArray)
        return FALSE;

    D3DXMATRIX *pViewMatrix = m_Camera.GetViewMatrix();

    Frame DefaultView;

    DefaultView.euler_set_rotate(Vector(0, 0, 0), 0);
    DefaultView.rotate(Vector(D3DXToRadian(-5.0f), 0, 0));
    DefaultView.m_origin = Vector(0.0, -2.2, 1.1);

    m_Camera.SetFOV(90.0f);
    m_Camera.SetViewPosition(DefaultView);
    
    if (m_pPartArray->m_pPartSetup)
    {
        if (m_pPartArray->m_pPartSetup->m_Key == 0x0200099E)
            set_sequence_animation(0x03000001, 1, 0, 30.0f);

        if (m_pPartArray->m_pPartSetup->m_Key == 0x02001255)
            set_sequence_animation(0x030000AC, 1, 0, 30.0f);
    }

    return TRUE;
}

void SetupScene::set_sequence_animation(DWORD AnimID, BOOL ClearAnims, long LowFrame, float FrameRate)
{
    if (!m_pPartArray)
        return;

    AnimData ad;

    ad.m_AnimID = AnimID;
    ad.m_LowFrame = LowFrame;
    ad.m_PlaySpeed = FrameRate;

    if (ClearAnims)
        m_pPartArray->m_Sequence.clear();

    m_pPartArray->m_Sequence.append_animation(&ad); 
}

void SetupScene::Destroy()
{
    if (m_pPartArray)
    {
        delete m_pPartArray;
        m_pPartArray = NULL;
    }
}

void SetupScene::Render(double FrameTime)
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
    {
        static Frame BlankFrame;
        BlankFrame.set_heading(180);
        m_pPartArray->Update(FrameTime, &BlankFrame);
        m_pPartArray->SetFrame(&BlankFrame);
        m_pPartArray->Draw(NULL);
    }
}










