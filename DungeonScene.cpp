
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
#include "Preferences.h"

Vector DungeonScene::camera_pos;

DungeonScene::DungeonScene()
{
    m_LastCamUpdate = 0;

    memset(&m_Keys, 0, sizeof(m_Keys));

    m_Physics = new CPhysics(NULL, NULL);
    m_EnvCellCount = 0;
    m_EnvCells = NULL;
    m_SortedCells = NULL;
    m_CurrentCell = 0;
}

DungeonScene::~DungeonScene()
{
    Destroy();

    delete m_Physics;
}

void DungeonScene::Input_KeyDown(BYTE bKey)
{
    m_Keys[ bKey ] = TRUE;
}

void DungeonScene::Input_KeyUp(BYTE bKey)
{
    m_Keys[ bKey ] = FALSE;
}

void DungeonScene::Input_MouseMove(long x, long y)
{
    Render::clear_selection_cursor();
    Render::set_selection_cursor(x, y, TRUE);
}

void DungeonScene::MoveCameraWithInput(float FrameTime, BOOL *Keys)
{
    float MoveSpeed = 10.0f;
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

void DungeonScene::UpdateCamera()
{
    float FrameTime = (float)(Time::GetTimeElapsed() - m_LastCamUpdate);
    MoveCameraWithInput(FrameTime, m_Keys);

    m_LastCamUpdate = Time::GetTimeElapsed();

    extern char debug_status_string[512];

    // sprintf(debug_status_string, "%f %f %f", ViewPos->x, ViewPos->y, ViewPos->z);
    m_CurrentCell = 0;
    sprintf(debug_status_string, "CellID: (none)");

    for (DWORD i = 0; i < m_EnvCellCount; i++)
    {
        if (m_SortedCells[i])
        {
            DWORD block = (m_SortedCells[i]->m_Position.m_LandCell & 0xFFFF0000) >> 16;
            long block_gid_x = ((block & 0xFF00) >> 8) << 3;
            long block_gid_y = ((block & 0x00FF) >> 0) << 3;

            Vector localpoint;
            localpoint.x = Render::ViewOrigin.x - (block_gid_x * LandDefs::square_length);
            localpoint.y = Render::ViewOrigin.y - (block_gid_y * LandDefs::square_length);
            localpoint.z = Render::ViewOrigin.z;

            if (m_SortedCells[i]->point_in_cell(localpoint))
            {
                m_CurrentCell = m_SortedCells[i]->m_Position.m_LandCell;
					 sprintf(debug_status_string, "CellID: 0x%08X", m_CurrentCell);
                break;
            }

            // sprintf(debug_status_string, "%f %f %f", localpoint.x, localpoint.y, localpoint.z);
        }
        else
            break;
    }
}

int DungeonScene::SceneType()
{
    return eDungeonScene;
}

void DungeonScene::Destroy()
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

    if (m_SortedCells)
    {
        delete [] m_SortedCells;
        m_SortedCells = NULL;
    }

    m_EnvCellCount = 0;
}

BOOL DungeonScene::Init(DWORD LandBlock)
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
        DWORD ID = LandBlock | (LandDefs::first_envcell_id + i);

        m_EnvCells[i] = CEnvCell::Get(ID);

        if (m_EnvCells[i])
        {
            if (!m_EnvCells[i]->m_CellStruct || !m_EnvCells[i]->m_CellStruct->m_2C)
            {
                CEnvCell::Release(m_EnvCells[i]);
                m_EnvCells[i] = NULL;

                DEBUGOUT("Missing structure for cell %08X!\r\n", ID);
            }
            else
                m_EnvCells[i]->init_static_objects();
        }
        else
            DEBUGOUT("Missing block environment cell for %08X!\r\n", ID);
    }

    m_SortedCells = new CEnvCell*[ m_EnvCellCount ];
    for (DWORD i = 0; i < m_EnvCellCount; i++)
        m_SortedCells[i] = m_EnvCells[i];

    SortCells();

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

    if ((m_EnvCellCount > 0) && (m_EnvCells[0]))
        m_Camera.SetViewTarget(m_EnvCells[0]->m_Position);

    return TRUE;
}

void DungeonScene::SortCells()
{
    qsort(m_SortedCells, m_EnvCellCount, sizeof(CEnvCell *), &SortCellCompare);
}

int DungeonScene::SortCellCompare(const void *a, const void *b)
{
    CEnvCell *_a = *(CEnvCell **)a;
    CEnvCell *_b = *(CEnvCell **)b;

    if (!_a)
    {
        if (!_b)
            return 0;

        return 1;
    }
    if (!_b)
        return -1;

    float dist1 = (_a->m_Position.localtolocal(Render::ViewerPos, Vector(0, 0, 0))).magnitude();
    float dist2 = (_b->m_Position.localtolocal(Render::ViewerPos, Vector(0, 0, 0))).magnitude();

    // Should use sphere instead?
    if (dist1 > dist2)
        return -1;
    if (dist1 < dist2)
        return 1;
    
    return 0;
}

void DungeonScene::Render(double FrameTime)
{
    m_Physics->UseTime();

    UpdateCamera();
    SortCells();

    g_pD3DDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_ARGB(20, 20, 20, 20)); // D3DCOLOR_ARGB(51, 51, 51, 51));

    // Fix selection ray
    Render::update_viewpoint(*m_Camera.GetViewFrame());

    Render::SetZFill();
    Render::SetFogState(FALSE);
    // Render::SetFilling(D3DFILL_SOLID);

    Render::pLightManager->DisableAll();
    Render::SetCulling(D3DCULL_NONE);


    // Render::SetProjMatrix(m_Camera.GetProjMatrix());
    // Render::SetViewMatrix(m_Camera.GetViewMatrix());
    // Render::SetViewFrame(m_Camera.GetViewFrame());

#ifdef _DEBUG
    extern Vector TestLightOrigin;
    TestLightOrigin = *m_Camera.GetViewPoint();
#endif

    if (m_CurrentCell)
    {
		Render::SetLighting(TRUE);
		Render::FullBrightOverride = false;

        DWORD        CurrentCellIndex = (m_CurrentCell & LandDefs::block_cell_id) - LandDefs::first_envcell_id;
        CEnvCell*    CurrentCell = m_EnvCells[ CurrentCellIndex ];

        if (CurrentCell)
        {
            static CEnvCell *SortedPVCells[512];
            DWORD SortedPVCellCount = 0;

            for (DWORD i = 0; i < CurrentCell->m_PVCellCount; i++)
            {
                DWORD PVCellIndex = (CurrentCell->m_PVCells[i] & LandDefs::block_cell_id) - LandDefs::first_envcell_id;

                if (PVCellIndex < m_EnvCellCount)
                {
                    CEnvCell* PVCell = m_EnvCells[ PVCellIndex ];

                    if (PVCell)
                        SortedPVCells[ SortedPVCellCount++ ] = PVCell;
                }
                else
                    break;
            }

            qsort(SortedPVCells, SortedPVCellCount, sizeof(CEnvCell *), &SortCellCompare);

            for (DWORD i = 0; i < SortedPVCellCount; i++)
                Render::pLightManager->EnableAllInCell(SortedPVCells[i]->m_Position.m_LandCell);

            Render::pLightManager->EnableAllInCell(CurrentCell->m_Position.m_LandCell);

            for (DWORD i = 0; i < SortedPVCellCount; i++)
                Render::DrawEnvCell(SortedPVCells[i]);

            Render::DrawEnvCell(CurrentCell);
        }
    }
    else
    {
        // Render all cells.
		
		// Not rendering all lights because performance is really bad in large dungeons until this is optimized.

		if (RenderPreferences::RenderFullbrightOutsideCells)
		{
			Render::SetLighting(FALSE);
			Render::FullBrightOverride = true;
		}
		else
		{
			Render::pLightManager->EnableAll();
			Render::SetLighting(TRUE);
			Render::FullBrightOverride = false;
		}

        for (DWORD i = 0; i < m_EnvCellCount; i++)
        {
            if (m_SortedCells[i])
            {
                //if (m_SortedCells[i]->m_Position.m_LandCell == 0x0120010E)
                    Render::DrawEnvCell(m_SortedCells[i]);
            }
            else
                break;
        }
    }

    // for (DWORD i = 0; i < Render::pLightManager->m_LightCount; i++)
    //    Render::Draw3DPoint(Render::pLightManager->m_LightData.array_data[i]->m_Origin, 1);
}









