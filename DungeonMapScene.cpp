

#include "StdAfx.h"
#include "RenderScene.h"
#include "LandDefs.h"
#include "LandBlock.h"
#include "LandBlockInfo.h"
#include "EnvCell.h"
#include "Environment.h"
#include "Render.h"

DungeonMapScene::DungeonMapScene()
{
    m_LastCamUpdate = 0;

    memset(&m_Keys, 0, sizeof(m_Keys));

    m_EnvCellCount = 0;
    m_EnvCells = NULL;
}

DungeonMapScene::~DungeonMapScene()
{
    Destroy();
}

void DungeonMapScene::Input_KeyDown(BYTE bKey)
{
    m_Keys[ bKey ] = TRUE;
}

void DungeonMapScene::Input_KeyUp(BYTE bKey)
{
    m_Keys[ bKey ] = FALSE;
}

void DungeonMapScene::MoveCameraWithInput(float FrameTime, BOOL *Keys)
{
    float movespeed = 10.0f;
    float zspeed = 10.0f;
    float scalespeed = 0.1f;

    Vec2D offset(0, 0);

    float znear, zfar, scaledir;
    znear = zfar = scaledir = 0.0;

    // Move Forward/backward
    if (Keys['W'])        offset.y += FrameTime * movespeed;
    if (Keys['S'])        offset.y -= FrameTime * movespeed;

    // Move Left/Right (straf)
    if (Keys['A'])        offset.x -= FrameTime * movespeed;
    if (Keys['D'])        offset.x += FrameTime * movespeed;

    // Move Up/Down
    if (Keys['R'])        znear += FrameTime * zspeed;
    if (Keys['F'])        znear -= FrameTime * zspeed;

    // Look Up/Down
    if (Keys['T'])        zfar += FrameTime * zspeed;
    if (Keys['G'])        zfar -= FrameTime * zspeed;

    // Zoom In/Out
    if (Keys['Y'])        scaledir += FrameTime * scalespeed;
    if (Keys['H'])        scaledir -= FrameTime * scalespeed;

    if (scaledir)
        m_Camera.AdjustZoom(scaledir);

    if (znear || zfar)
        m_Camera.AdjustZLimits(znear, zfar);

    if (offset.x || offset.y)
        m_Camera.Move(offset);
}

void DungeonMapScene::UpdateCamera()
{
    float FrameTime = (float)(Time::GetTimeElapsed() - m_LastCamUpdate);
    MoveCameraWithInput(FrameTime, m_Keys);

    m_LastCamUpdate = Time::GetTimeElapsed();
}

int DungeonMapScene::SceneType()
{
    return eDungeonMapScene;
}

void DungeonMapScene::Destroy()
{
    if (m_LandBlock)
    {
        CLandBlock::Release(m_LandBlock);
        m_LandBlock = NULL;
    }

    if (m_EnvCells)
    {
        for (DWORD i = 0; i < m_EnvCellCount; i++)
            CEnvCell::Release(m_EnvCells[i]); 

        delete [] m_EnvCells;
        m_EnvCells = NULL;
    }

    m_EnvCellCount = 0;
}

BOOL DungeonMapScene::Init(DWORD LandBlock)
{
    LandBlock &= LandDefs::blockid_mask;

    m_LandBlock = CLandBlock::Get(LandBlock | LandDefs::block_cell_id);

    if (!m_LandBlock)
    {
        DEBUGOUT("Failed finding LandBlock(%08X)\r\n", LandBlock | LandDefs::block_cell_id);
        return FALSE;
    }

    if (m_LandBlock->m_bLBInfo && m_LandBlock->m_pLBInfo)
        m_EnvCellCount = m_LandBlock->m_pLBInfo->m_CellCount;
    else
    {
        DEBUGOUT("LandBlock has no extra info?\r\n");
        m_EnvCellCount = 0;
    }

    m_EnvCells = new CEnvCell*[ m_EnvCellCount ];

    for (DWORD i = 0; i < m_EnvCellCount; i++)
    {
        m_EnvCells[i] = CEnvCell::Get(LandBlock | (LandDefs::first_envcell_id + i));

        if (m_EnvCells[i])
            m_EnvCells[i]->init_static_objects();
    }

    CalcBBox();

    return TRUE;
}

void DungeonMapScene::CalcBBox()
{
    BOOL NeedBox = TRUE;

    for (DWORD i = 0; i < m_EnvCellCount; i++)
    {
        if (!m_EnvCells[i])
            continue;
        if (!m_EnvCells[i]->m_CellStruct)
            continue;

        DWORD VertexCount = m_EnvCells[i]->m_CellStruct->m_VertexArray.m_VertexCount;
        
        if (!(VertexCount > 0))
            continue;

        if (NeedBox)
        {
            m_BBox.m_Min = ((CSWVertex *)m_EnvCells[i]->m_CellStruct->m_VertexArray.m_pVertexBuffer)[0].origin;
            m_BBox.m_Max = ((CSWVertex *)m_EnvCells[i]->m_CellStruct->m_VertexArray.m_pVertexBuffer)[0].origin;

            for (DWORD j = 1; j < VertexCount; j++)
            {
                CSWVertex* Vertex = (CSWVertex *)((BYTE *)m_EnvCells[i]->m_CellStruct->m_VertexArray.m_pVertexBuffer + j*CVertexArray::vertex_size);

                Vector local_point = m_EnvCells[i]->m_Position.m_Frame.globaltolocal(Vertex->origin);
                m_BBox.CalcBBox(local_point);
            }

            NeedBox = FALSE;
        }
        else
        {
            for (DWORD j = 0; j < VertexCount; j++)
            {
                CSWVertex* Vertex = (CSWVertex *)((BYTE *)m_EnvCells[i]->m_CellStruct->m_VertexArray.m_pVertexBuffer + j*CVertexArray::vertex_size);

                Vector Transformed =
                    m_EnvCells[i]->m_Position.m_Frame.localtoglobal(Vertex->origin);

                m_BBox.CalcBBox(Transformed);
            }
        }
    }

    if (NeedBox)
    {
        // No vertices, null bbox at origin
        m_BBox.m_Min = Vector(0.0, 0.0, 0.0);
        m_BBox.m_Max = Vector(0.0, 0.0, 0.0);
    }

    float Max = 1000 - m_BBox.m_Max.z - 1.0;
    float Min = 1000 - m_BBox.m_Min.z + 1.0;
    DEBUGOUT("Map z-planes (%f, %f)\r\n", Max, Min);

    m_Camera.SetZLimits(Max, Min);
}

void DungeonMapScene::Render(double FrameTime)
{
    UpdateCamera();

    Render::SetZFill();
    Render::SetFogState(FALSE);
    Render::SetFilling(D3DFILL_SOLID);

    Render::SetLighting(FALSE);
    Render::SetCulling(D3DCULL_CW);

    Render::SetProjMatrix(m_Camera.GetProjMatrix());
    Render::SetViewMatrix(m_Camera.GetViewMatrix());
    // Render::SetViewFrame(m_Camera.GetViewFrame());

    for (DWORD i = 0; i < m_EnvCellCount; i++)
    {
        if (m_EnvCells[i])
            Render::DrawEnvCell(m_EnvCells[i]);
    }
}









