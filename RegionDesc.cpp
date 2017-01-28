
#include "StdAfx.h"
#include "GameTime.h"
#include "RegionDesc.h"
#include "LandDefs.h"

#pragma warning(disable : 4150)

CRegionDesc *CRegionDesc::current_region = NULL;

CRegionDesc::CRegionDesc() : DBObj(ObjCaches::RegionDescs)
{
    m_28 = NULL;
    m_Name = "";

    m_38 = 0;                
    m_FileNameDesc = NULL;    
    m_SkyDesc = NULL;        
    m_SoundDesc = NULL;        
    m_SceneDesc = NULL;        
    m_TerrainDesc = NULL;    
    m_EncounterDesc = NULL;
    m_RegionMisc = NULL;

    m_Version = -1;
    m_34 = 0;
}

CRegionDesc::~CRegionDesc()
{
    Destroy();
}

DBObj* CRegionDesc::Allocator()
{
    return((DBObj *)new CRegionDesc());
}

void CRegionDesc::Destroyer(DBObj* pRegionDesc)
{
    delete ((CRegionDesc *)pRegionDesc);
}

CRegionDesc *CRegionDesc::Get(DWORD ID)
{
    return (CRegionDesc *)ObjCaches::RegionDescs->Get(ID);
}

void CRegionDesc::Release(CRegionDesc *pRegionDesc)
{
    if (pRegionDesc)
        ObjCaches::RegionDescs->Release(pRegionDesc->m_Key);
}

BOOL CRegionDesc::SetRegion(DWORD Instance, BOOL Hardware)
{
    // Omits the instance checks..
    CRegionDesc *pRegion = CRegionDesc::Get(Hardware? 0x130F0000 : 0x13000000);

    if (!pRegion)
        return FALSE;

    if (current_region)
        CRegionDesc::Release(current_region);

    current_region = pRegion;
    return TRUE;
}

void CRegionDesc::Destroy(void)
{
    m_Version = -1;

    if (m_FileNameDesc)
    {
        UNFINISHED("FileNameDesc");

        delete m_FileNameDesc;
        m_FileNameDesc = NULL;
    }
    
    if (m_SkyDesc)
    {
        delete m_SkyDesc;
        m_SkyDesc = NULL;
    }

    if (m_SceneDesc)
    {
        delete m_SceneDesc;
        m_SceneDesc = NULL;
    }

    if (m_TerrainDesc)
    {
        delete m_TerrainDesc;
        m_TerrainDesc = NULL;
    }

    if (m_RegionMisc)
    {
        delete m_RegionMisc;
        m_RegionMisc = NULL;
    }

    if (m_SoundDesc)
    {
        delete m_SoundDesc;
        m_SoundDesc = NULL;
    }

    if (m_EncounterDesc)
    {
        delete m_EncounterDesc;
        m_EncounterDesc = NULL;
    }
}

BOOL CRegionDesc::UnPack(BYTE **ppData, ULONG iSize)
{
    Destroy();

    if (!UNPACK(DWORD, m_Key))
        goto check_version;
    if (!UNPACK(DWORD, m_28))
        goto check_version;
    if (!UNPACK(DWORD, m_Version))
        goto check_version;

check_version:
    if (m_Version != 3)
    {
        if (m_Version > 3)
        {
            DEBUGOUT("Error: The data files have a more recent version than the executable..\r\n");
            return FALSE;
        }
        else
        {
            DEBUGOUT("Error: The executable has a more recent version than the data files..\r\n");
            return FALSE;
        }
    }

    if (!UNPACK_OBJ(m_Name))
        return FALSE;

    // Close your eyes and pretend this isn't here.
    DWORD NamePackSize = 2 + (((m_Name.m_Length - 1) >= 0xFFFF)?4:0) + (m_Name.m_Length - 1);

    if (NamePackSize & 3)
        NamePackSize += 4 - (NamePackSize & 3);
    
    iSize -= NamePackSize;

#ifdef PRE_TOD
    PACK_ALIGN();
#endif

    // Excludes size checks. They were worthless anyways.

    long    NumBlockLength, NumBlockWidth, LBlockLength, VertexPerCell;
    float    SquareLength, MaxObjHeight, SkyHeight, RoadWidth;

    UNPACK(long,    NumBlockLength);
    UNPACK(long,    NumBlockWidth);
    UNPACK(float,    SquareLength);
    UNPACK(long,    LBlockLength);
    UNPACK(long,    VertexPerCell);
    UNPACK(float,    MaxObjHeight);
    UNPACK(float,    SkyHeight);
    UNPACK(float,    RoadWidth);

    LandDefs::init(
        NumBlockLength, NumBlockWidth, SquareLength, LBlockLength,
        VertexPerCell, MaxObjHeight, SkyHeight, RoadWidth);

    float HeightTable[256];

    for (int i = 0; i < 256; i++)
        UNPACK(float, HeightTable[i]);

    LandDefs::set_height_table(HeightTable);

    GameTime *pGameTime = new GameTime;

    if (pGameTime && UNPACK_POBJ(pGameTime))
    {
        if (GameTime::current_game_time)
            delete GameTime::current_game_time;
        
        GameTime::current_game_time = pGameTime;
    }

    UNPACK(DWORD, m_38); // 0x21F

    /*

    if (FBitSet(m_38, 4)) // 0x10
    {
        m_SkyDesc = new SkyDesc;

        UNPACK_POBJ(m_SkyDesc);
    }

    if (FBitSet(m_38, 0)) // 0x1
    {
        m_SoundDesc = new CSoundDesc;

        UNPACK_POBJ(m_SoundDesc);
    }

    if (FBitSet(m_38, 1)) // 0x2
    {
        m_SceneDesc = new CSceneDesc;
        UNPACK_POBJ(m_SceneDesc);
    }

    if (TRUE)
    {
        m_TerrainDesc = new CTerrainDesc;
        UNPACK_POBJ(m_TerrainDesc);
    }

    m_RegionMisc = new RegionMisc;

    if (FBitSet(m_38, 9)) // 0x200
        UNPACK_POBJ(m_RegionMisc);
    else
        UNFINISHED("RegionMisc alternate..");

    if (TRUE)
        PACK_ALIGN();

    */

    return TRUE;
}








