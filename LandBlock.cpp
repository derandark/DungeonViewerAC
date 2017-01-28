
#include "StdAfx.h"
#include "PhysicsObj.h"
#include "LandDefs.h"
#include "LandCell.h"
#include "LandBlock.h"

CLandBlock::CLandBlock() : DBObj(ObjCaches::LandBlocks), m_StaticObjs(0x80)
{
    m_MinHeight = 0.0;
    m_MaxHeight = 0.0;
    m_C0 = 0;
    m_bLBInfo = FALSE;

    m_D4 = 0;
    m_pLBInfo = NULL;
    m_StaticObjCount = 0;
    m_C8 = 9;

    m_BuildingCount = 0;
    m_Buildings = NULL;
    m_F8 = 0;
    m_FC = 0;
    
    m_108 = NULL;
    m_10C = 0;

    m_CC = -1;
    m_D0 = -1;
}

CLandBlock::~CLandBlock()
{
    Destroy();
}

DBObj* CLandBlock::Allocator()
{
    return((DBObj *)new CLandBlock());
}

void CLandBlock::Destroyer(DBObj* pLandBlock)
{
    delete ((CLandBlock *)pLandBlock);
}

CLandBlock *CLandBlock::Get(DWORD ID)
{
    return (CLandBlock *)ObjCaches::LandBlocks->Get(ID);
}

void CLandBlock::Release(CLandBlock *pLandBlock)
{
    if (pLandBlock)
        ObjCaches::LandBlocks->Release(pLandBlock->m_Key);
}

void CLandBlock::add_static_object(CPhysicsObj *pObject)
{
    if (m_StaticObjCount >= m_StaticObjs.alloc_size)
        m_StaticObjs.grow(m_StaticObjs.alloc_size + 40);

    m_StaticObjs.array_data[ m_StaticObjCount++ ] = pObject;
}

void CLandBlock::destroy_static_objects(void)
{
    for (DWORD i = 0; i < m_StaticObjCount; i++)
    {
        if (m_StaticObjs.array_data[i])
        {
            m_StaticObjs.array_data[i]->leave_world();
            delete m_StaticObjs.array_data[i];
        }
    }

    m_StaticObjCount = 0;
}

void CLandBlock::destroy_buildings(void)
{
    
    if (m_Buildings)
    {
        __asm int 3;
        /*
        for (DWORD i = 0; i < m_BuildingCount; i++)
        {
            if (m_Buildings[i])
                m_Buildings[i]->remove();

            if (m_Buildings[i])
                delete m_Buildings[i];
        }
        */

        delete [] m_Buildings;
        m_Buildings = NULL;
    }
    m_BuildingCount = 0;

    if (m_FC)
    {
        delete [] m_FC;
        m_FC = NULL;
        m_F8 = 0;
    }    
}

void CLandBlock::Destroy(void)
{
    destroy_static_objects();
    destroy_buildings();

    if (m_pLBInfo)
    {
        CLandBlockInfo::Release(m_pLBInfo);
        m_pLBInfo = NULL;
    }

    m_CC = -1;
    m_D0 = -1;
    m_bLBInfo = FALSE;

    if (m_108)
    {
        delete [] m_108;
        m_108 = NULL;
    }
}

void CLandBlock::init(void)
{
    CLandBlockStruct::init();
}

void CLandBlock::get_land_limits(void)
{
    int NumHeightVerts = LandDefs::side_vertex_count * LandDefs::side_vertex_count;

    BYTE High, Low;
    High = Low = m_LBStruct.m_HeightMap[0];

    for (int i = 1; i < NumHeightVerts; i++)
    {
        BYTE height = m_LBStruct.m_HeightMap[i];

        if (High < height)
            High = height;
        if (Low > height)
            Low = height;
    }

    m_MinHeight = LandDefs::Land_Height_Table[High] + LandDefs::max_object_height;
    m_MaxHeight = LandDefs::Land_Height_Table[High] - 1.0f;
}

void CLandBlock::init_lcell_ptrs(void)
{
    int NumLandCells = m_LBStruct.m_CellWidth * m_LBStruct.m_CellWidth;

    for (int i = 0; i < NumLandCells; i++)
        m_LBStruct.m_LandCells[i].m_LandBlock = this; 
}

void CLandBlock::notify_change_size(void)
{
    // Not done.
}

void CLandBlock::release_all(void)
{
    // Not done.
}

BOOL CLandBlock::UnPack(BYTE **ppData, ULONG iSize)
{
    Destroy();

    UNPACK(DWORD, m_Key);
    UNPACK(DWORD, m_bLBInfo);

    UNPACK_OBJ(m_LBStruct);

    if (m_bLBInfo)
        m_pLBInfo = CLandBlockInfo::Get((m_Key & LandDefs::blockid_mask) | LandDefs::lbi_cell_id);

    return TRUE;
}








