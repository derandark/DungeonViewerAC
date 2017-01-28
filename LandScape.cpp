
#include "StdAfx.h"
#include "LandScape.h"
#include "LandBlock.h"
#include "LandDefs.h"
#include "GameSky.h"
#include "GfxObj.h"
#include "Render.h"

LPVOID block_interval = NULL;

LScape::LScape()
{
    m_LandBlocks = NULL;
    m_0C = NULL;
    m_LoadPointBlock = 0;
    m_14 = 0;
    m_18 = 0;
    m_1C = 0;
    m_DetailMesh = NULL;
    m_2C = 1;
    m_30 = 0;
    m_34 = 0;
    m_38 = 0;
    m_3C = 0;

    if (/*RenderOptions::bSingleSurfaceLScape*/ FALSE)
    {
        m_MidRadius = 1;
        m_MaxRadius = 3;
    }
    else
    {
        m_MidRadius = 5;
        m_MaxRadius = 11;
    }

    // ambient_level = 0.4;
    // sunlight = 1.2;
    // dword_5F15E0 = 0;
    // dword_5F15E4 = 0.5;

    CLandBlock::init();

    // Finish me first.
    m_GameSky = NULL; // new GameSky;
}

LScape::~LScape()
{
    release_all();

    if (block_interval)
    {
        delete [] block_interval;
        block_interval = NULL;
    }

    if (m_GameSky)
    {
        delete m_GameSky;
        m_GameSky = NULL;
    }

    if (m_DetailMesh)
    {
        delete m_DetailMesh;
        m_DetailMesh = NULL;
    }

    CleanupDetailSurfaces();
}

void LScape::CleanupDetailSurfaces(void)
{
    // Finish me
}

void LScape::ChangeRegion(void)
{
    if (!GenerateDetailMesh())
    {
        if (m_DetailMesh)
            delete m_DetailMesh;

        m_DetailMesh = NULL;
    }
}

BOOL LScape::GenerateDetailMesh(void)
{
    // Finish me.
    return FALSE;
}

void LScape::draw(void)
{
    if (m_GameSky)
        UNFINISHED("m_GameSky->Draw(0);");

    UNFINISHED("Missing landscape draw code");

    for (int i = (m_MaxRadius * m_MaxRadius) - 1; i > 0; i--)
    {

    }

    if (m_GameSky && m_2C)
        UNFINISHED("m_GameSky->Draw(1);");
}

void LScape::pea_draw(void)
{
    if (m_LandBlocks)
    {
        for (int i = (m_MaxRadius * m_MaxRadius) - 1; i > 0; i--)
        {
            if (m_LandBlocks[i])
            {
                RenderDevice::DrawBlock(m_LandBlocks[i]);
            }
        }
    }
}

CLandBlock* LScape::get_all(DWORD LandBlock)
{
    CLandBlock *pLandBlock;

    if (pLandBlock = CLandBlock::Get(LandBlock))
    {
        long LCoordX, LCoordY;

        LandDefs::blockid_to_lcoord(
            LandBlock, LCoordX, LCoordY);

        pLandBlock->m_LBStruct.m_LCoordX = LCoordX;
        pLandBlock->m_LBStruct.m_LCoordY = LCoordY;
    }
    else
        DEBUGOUT("Missing LandBlock %08X\r\n", LandBlock);

    return pLandBlock;
}

BOOL LScape::get_block_shift(DWORD Block1, DWORD Block2, DWORD* XOut, DWORD* YOut)
{
    if (!Block1)
        return FALSE;

    long X2, Y2, X1, Y1;
    LandDefs::blockid_to_lcoord(Block2, X2, Y2);
    LandDefs::blockid_to_lcoord(Block1, X1, Y1);

    *XOut = (X2 - X1) / LandDefs::lblock_side;
    *YOut = (Y2 - Y1) / LandDefs::lblock_side;

    return TRUE;
}

void LScape::update_loadpoint(DWORD LandBlock)
{
    if (!LandBlock)
    {
        release_all();
        return;
    }

    DWORD ShiftX, ShiftY;
    BOOL Unknown;
    Unknown = !get_block_shift(m_LoadPointBlock, LandBlock, &ShiftX, &ShiftY);

    BOOL MissingBlock = FALSE;

    if (m_LandBlocks)
    {
        for (int x = 0; x < m_MaxRadius; x++) {
            for (int y = 0; y < m_MaxRadius; y++) {
                if (!m_LandBlocks[x*m_MaxRadius + y])
                {
                    MissingBlock = FALSE;
                    break;
                }
            }
        }
    }

    if (ShiftX || ShiftY || !m_LandBlocks || Unknown || MissingBlock)
    {
        update_block(LandBlock, ShiftX, ShiftY, Unknown);

        if ((m_LoadPointBlock >> LandDefs::block_part_shift) != (LandBlock >> LandDefs::block_part_shift))
            Unknown = TRUE;

        m_LoadPointBlock = LandBlock;
        calc_draw_order(m_14, Unknown);
    }
    else
        m_LoadPointBlock = LandBlock;
}

void LScape::calc_draw_order(DWORD, BOOL)
{
    // Not done.
}

void LScape::update_block(DWORD LandBlock, long ShiftX, long ShiftY, BOOL Unknown)
{
    long GidX, GidY;
    LandDefs::gid_to_lcoord(
        LandBlock, GidX, GidY);

    GidX -= m_MidRadius * LandDefs::lblock_side;
    GidY -= m_MidRadius * LandDefs::lblock_side;

    if (m_LandBlocks && !Unknown &&
        (ShiftX <  m_MaxRadius)    && (ShiftY <  m_MaxRadius) &&
        (ShiftX > -m_MaxRadius) && (ShiftY > -m_MaxRadius))
    {
        if (ShiftX != 0 || ShiftY != 0)
        {
            if (ShiftX < 0)
            {
                if (ShiftY < 0)
                {
                    for (int x = m_MaxRadius - 1, gid_x = (x * LandDefs::lblock_side) + GidX; x >= 0; x--, gid_x -= LandDefs::lblock_side) {
                        for (int y = m_MaxRadius - 1, gid_y = (y * LandDefs::lblock_side) + GidY; y >= 0; y--, gid_y -= LandDefs::lblock_side) {

                            if (((m_MaxRadius + ShiftX) >= x) || ((m_MaxRadius + ShiftY) >= y))
                            {
                                if (m_LandBlocks[x*m_MaxRadius + y])
                                    m_LandBlocks[x*m_MaxRadius + y]->release_all();
                            }

                            if (((ShiftX + x) >= 0) && ((ShiftY + y) >= m_MaxRadius))
                                m_LandBlocks[m_MaxRadius*x + y] = m_LandBlocks[(m_MaxRadius*(ShiftX + x)) + (ShiftY + y)];
                            else
                            {
                                if (LandDefs::in_bounds(gid_x, gid_y))
                                    m_LandBlocks[x*m_MaxRadius + y] = get_all(LandDefs::get_block_gid(gid_x, gid_y));
                                else
                                    m_LandBlocks[x*m_MaxRadius + y] = NULL;
                            }
                        }
                    }
                }
                else
                {
                    for (int x = m_MaxRadius - 1, gid_x = (x * LandDefs::lblock_side) + GidX; x >= 0; x--, gid_x -= LandDefs::lblock_side) {
                        for (int y = 0, gid_y = GidY; y < m_MaxRadius; y++, gid_y += LandDefs::lblock_side) {

                            if ((x >= (m_MaxRadius + ShiftX)) || (ShiftY > y))
                            {
                                if (m_LandBlocks[x*m_MaxRadius + y])
                                    m_LandBlocks[x*m_MaxRadius + y]->release_all();
                            }

                            if (((ShiftX + x) >= 0) && ((ShiftY + y) < m_MaxRadius))
                                m_LandBlocks[m_MaxRadius*x + y] = m_LandBlocks[(m_MaxRadius*(ShiftX + x)) + (ShiftY + y)];
                            else
                            {
                                if (LandDefs::in_bounds(gid_x, gid_y))
                                    m_LandBlocks[x*m_MaxRadius + y] = get_all(LandDefs::get_block_gid(gid_x, gid_y));
                                else
                                    m_LandBlocks[x*m_MaxRadius + y] = NULL;
                            }
                        }
                    }
                }
            }
            else
            if (ShiftY < 0)
            {
                for (int x = 0, gid_x = GidX; x < m_MaxRadius; x++, gid_x += LandDefs::lblock_side) {
                    for (int y = m_MaxRadius - 1, gid_y = (y * LandDefs::lblock_side) + GidY; y >= 0; y--, gid_y -= LandDefs::lblock_side) {

                        if ((ShiftX > x) || (y >= (m_MaxRadius + ShiftY)))
                        {
                            if (m_LandBlocks[x*m_MaxRadius + y])
                                m_LandBlocks[x*m_MaxRadius + y]->release_all();
                        }

                        if (((ShiftX + x) < m_MaxRadius) && ((ShiftY + y) >= 0))
                            m_LandBlocks[m_MaxRadius*x + y] = m_LandBlocks[(m_MaxRadius*(ShiftX + x)) + (ShiftY + y)];
                        else
                        {
                            if (LandDefs::in_bounds(gid_x, gid_y))
                                m_LandBlocks[x*m_MaxRadius + y] = get_all(LandDefs::get_block_gid(gid_x, gid_y));
                            else
                                m_LandBlocks[x*m_MaxRadius + y] = NULL;
                        }
                    }
                }
            }
            else
            {
                for (int x = 0, gid_x = GidX; x < m_MaxRadius; x++, gid_x += LandDefs::lblock_side) {
                    for (int y = 0, gid_y = GidY; y < m_MaxRadius; y++, gid_y += LandDefs::lblock_side) {

                        if (x < ShiftX || y < ShiftY) {
                            if (m_LandBlocks[x*m_MaxRadius + y])
                                m_LandBlocks[x*m_MaxRadius + y]->release_all();
                        }

                        long curr_x = ShiftX + x;
                        long curr_y = ShiftY + y;

                        if ((curr_x < m_MaxRadius) && (curr_y < m_MaxRadius))
                            m_LandBlocks[x*m_MaxRadius + y] = m_LandBlocks[curr_x*m_MaxRadius + curr_y];
                        else
                        {
                            if (LandDefs::in_bounds(gid_x, gid_y))
                                m_LandBlocks[x*m_MaxRadius + y] = get_all(LandDefs::get_block_gid(gid_x, gid_y));
                            else
                                m_LandBlocks[x*m_MaxRadius + y] = NULL;
                        }
                    }
                }
            }
        }
        else
        {
            for (int x = 0, gid_x = GidX; x < m_MaxRadius; x++, gid_x += LandDefs::lblock_side) {
                for (int y = 0, gid_y = GidY; y < m_MaxRadius; y++, gid_y += LandDefs::lblock_side) {
                    DWORD index = x*m_MaxRadius + y;

                    if (!m_LandBlocks[index]) {
                        if (LandDefs::in_bounds(gid_x, gid_y))
                            m_LandBlocks[index] = get_all(LandDefs::get_block_gid(gid_x, gid_y));
                        else
                            m_LandBlocks[index] = NULL;
                    }
                }
            }
        }
    }
    else
    {
        release_all();

        if (!m_LandBlocks)
            m_LandBlocks = new CLandBlock*[ m_MaxRadius * m_MaxRadius ];

        for (int x = 0, gid_x = GidX; x < m_MaxRadius; x++, gid_x += LandDefs::lblock_side) {
            for (int y = 0, gid_y = GidY; y < m_MaxRadius; y++, gid_y += LandDefs::lblock_side) {
                DWORD index = (x*m_MaxRadius) + y;

                if (LandDefs::in_bounds(gid_x, gid_y))
                    m_LandBlocks[index] = get_all(LandDefs::get_block_gid(gid_x, gid_y));
                else
                    m_LandBlocks[index] = NULL;
            }
        }
    }

    for (int x = 0; x < m_MaxRadius; x++) {
        for (int y = 0; y < m_MaxRadius; y++) {

            CLandBlock *pLB = m_LandBlocks[x*m_MaxRadius + y];

            if (pLB)
            {
                long Magic1, Magic2;
                get_block_orient(x, y, Magic1, Magic2);

                if ((pLB->m_LBStruct.m_CellWidth == LandDefs::lblock_side) &&
                     (pLB->m_LBStruct.m_CellWidth != (LandDefs::lblock_side / Magic1)))
                    pLB->notify_change_size();
                
                if (pLB->m_LBStruct.generate(pLB->m_Key, Magic1, Magic2))
                {
                    pLB->m_CC = -1;
                    pLB->m_D0 = -1;

                    pLB->init_lcell_ptrs();
                    pLB->m_LBStruct.calc_lighting();
                    pLB->get_land_limits();
                }
            }
        }
    }
}

BOOL LScape::SetMidRadius(long MidRadius)
{
    if (MidRadius < 1)
        return FALSE;

    // if (RenderOptions::bSingleSurfaceLScape && MidRadius != 1)
    //      return FALSE;

    if (m_LandBlocks)
        return FALSE;

    m_MidRadius = MidRadius;
    m_MaxRadius = MidRadius*2 + 1;
    return TRUE;
}

void LScape::get_block_orient(long x, long y, long& magic1, long& magic2)
{
    long ydist = y - m_MidRadius;
    long ydista = abs(ydist);
    long xdist = x - m_MidRadius;
    long xdista = abs(xdist);

    if (xdista <= ydista)
        xdista = ydista;

    if (xdista <= 1)
        magic1 = 1;
    else
    if (xdista <= 2)
        magic1 = 2;
    else
    if (xdista <= 4)
        magic1 = 4;
    else
    {
        magic1 = 8;
        magic2 = 0;
        return;
    }

    if (xdist != magic1)
    {
        if (xdist != -magic1)
        {
            if (ydist != magic1)
            {
                if (ydist != -magic1)
                    magic2 = 0;
                else
                    magic2 = 2;
            }
            else
                magic2 = 1;
        }
        else
        if (ydist != magic1)
        {
            if (ydist != -magic1)
                magic2 = 4;
            else
                magic2 = 6;
        }
        else
            magic2 = 5;
    }
    else
    if (ydist != magic1)
    {
        if (ydist != -magic1)
            magic2 = 3;
        else
            magic2 = 8;
    }
    else
        magic2 = 7;
}

void LScape::release_all(void)
{
    if (m_LandBlocks)
    {
        for (int x = 0; x < m_MaxRadius; x++) {
            for (int y = 0; y < m_MaxRadius; y++) {

                CLandBlock *pLandBlock = m_LandBlocks[(x * m_MaxRadius) + y];
                
                if (pLandBlock)
                    pLandBlock->release_all();

                m_LandBlocks[(x * m_MaxRadius) + y] = NULL;
            }
        }

        if (m_LandBlocks)
        {
            delete [] m_LandBlocks;
            m_LandBlocks = NULL;
        }

        if (m_0C)
        {
            delete [] m_0C;
            m_0C = NULL;
        }
    }

    if (m_GameSky)
        UNFINISHED("m_GameSky->SetInactive();");

    m_LoadPointBlock = 0;
    m_14 = 0;
}

void LScape::set_sky_position(const Position& Pos)
{
    if (m_LoadPointBlock && m_GameSky)
    {
        UNFINISHED("m_GameSky->UpdatePosition()");
        // m_GameSky->UpdatePosition(Pos);
    }
}







