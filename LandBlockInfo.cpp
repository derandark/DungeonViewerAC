
#include "StdAfx.h"
#include "LandBlockInfo.h"

CLandBlockInfo::CLandBlockInfo() : DBObj(ObjCaches::LandBlockInfos)
{
    m_StaticObjCount = 0;
    m_StaticObjIDs = NULL;
    m_StaticObjFrames = NULL;

    m_BuildingCount = 0;
    m_Buildings = NULL;

    m_3C = NULL;
    m_40 = NULL;

    m_CellCount = 0;
    m_48 = NULL;
    m_4C = NULL;
}

CLandBlockInfo::~CLandBlockInfo()
{
    Destroy();
}

DBObj* CLandBlockInfo::Allocator()
{
    return((DBObj *)new CLandBlockInfo());
}

void CLandBlockInfo::Destroyer(DBObj* pLandBlockInfo)
{
    delete ((CLandBlockInfo *)pLandBlockInfo);
}

CLandBlockInfo *CLandBlockInfo::Get(DWORD ID)
{
    return (CLandBlockInfo *)ObjCaches::LandBlockInfos->Get(ID);
}

void CLandBlockInfo::Release(CLandBlockInfo *pLandBlockInfo)
{
    if (pLandBlockInfo)
        ObjCaches::LandBlockInfos->Release(pLandBlockInfo->m_Key);
}

void CLandBlockInfo::Destroy()
{
    if (m_StaticObjIDs)
    {
        delete [] m_StaticObjIDs;
        m_StaticObjIDs = NULL;
    }

    if (m_StaticObjFrames)
    {
        delete [] m_StaticObjFrames;
        m_StaticObjFrames = NULL;
    }

    if (m_4C)
    {
        // MISSING CODE HERE!
        DEBUGOUT("Unfinished LandCell release code..\r\n");

        delete [] m_4C;
        m_4C = NULL;
    }

    if (m_48)
    {
        delete [] m_48;
        m_48 = NULL;
    }

    if (m_Buildings)
    {
        for (DWORD i = 0; i < m_BuildingCount; i++)
        {
            if (m_Buildings[i])
                delete m_Buildings[i];
        }

        delete [] m_Buildings;
        m_Buildings = NULL;
    }

    if (m_3C)
    {
        delete m_3C;
        m_3C = NULL;
    }

    if (m_40)
    {
        delete m_40;
        m_40 = NULL;
    }

    m_CellCount = 0;
    m_BuildingCount = 0;
}

DWORD CLandBlockInfo::Unpack_Num_Cells(BYTE *pData)
{
    return *(DWORD *)(pData + sizeof(DWORD));
}

BOOL CLandBlockInfo::UnPack(BYTE **ppData, ULONG iSize)
{
    Destroy();

    UNPACK(DWORD, m_Key);

    UNPACK(DWORD, m_CellCount);
    UNPACK(DWORD, m_StaticObjCount);

    if (m_StaticObjCount > 0)
    {
        m_StaticObjIDs = new DWORD[ m_StaticObjCount ];
        m_StaticObjFrames = new Frame[ m_StaticObjCount ];

        for (DWORD i = 0; i < m_StaticObjCount; i++)
        {
            UNPACK(DWORD, m_StaticObjIDs[i]);
            UNPACK_OBJ(m_StaticObjFrames[i]);
        }
    }

    DWORD LandBlock = m_Key & 0xFFFF0000;

    DWORD BuildingInfo, BuildingCount, BuildingFlags;

    UNPACK(DWORD, BuildingInfo);
    BuildingCount = LOWORD(BuildingInfo);
    BuildingFlags = HIWORD(BuildingInfo);
    
    m_BuildingCount = BuildingCount;

    // Nasty !
    if (BuildingCount > 0)
    {
        m_Buildings = new BuildInfo*[ BuildingCount ];

        for (DWORD i = 0; i < BuildingCount; i++)
        {
            m_Buildings[i] = new BuildInfo;

            UNPACK(DWORD, m_Buildings[i]->m_MeshID);

            UNPACK_OBJ(m_Buildings[i]->m_Frame);

            UNPACK(DWORD, m_Buildings[i]->m_44);
            UNPACK(DWORD, m_Buildings[i]->m_NumPortals);

            m_Buildings[i]->m_Portals = new CBldPortal*[ m_Buildings[i]->m_NumPortals ];

            for (DWORD j = 0; j < m_Buildings[i]->m_NumPortals; j++)
            {
                m_Buildings[i]->m_Portals[j] = new CBldPortal;
                m_Buildings[i]->m_Portals[j]->UnPack(LandBlock, ppData, iSize);
            }
        }
    }

#ifdef PRE_TOD
    PACK_ALIGN();
#endif

    if (BuildingFlags & 1)
    {
        m_3C = new PackableHashTable<DWORD, DWORD>;

        if (!m_3C->UnPack(ppData, iSize))
            return FALSE;
    }

#ifdef PRE_TOD
    PACK_ALIGN();
#endif
    return TRUE;
}

BuildInfo::BuildInfo()
{
    m_MeshID = 0;

    m_44 = 0;
    m_NumPortals = 0;
    m_Portals = NULL;
}

BuildInfo::~BuildInfo()
{
    for (DWORD i = 0; i < m_NumPortals; i++)
    {
        if (m_Portals[i])
            delete m_Portals[i];
    }

    delete [] m_Portals;
    m_Portals = NULL;
    m_MeshID = 0;

    m_NumPortals = 0;
    m_44 = 0;
}

CBldPortal::CBldPortal()
{
    m_00 = -1;
    m_Cell = 0;
    m_08 = -1;
    m_0C = 0;
    m_VisCellCount = 0;
    m_VisCellIDs = NULL;
}

CBldPortal::~CBldPortal()
{
    delete [] m_VisCellIDs;
}

BOOL CBldPortal::UnPack(DWORD Block, BYTE **ppData, ULONG iSize)
{
    BYTE Flags;
    UNPACK(WORD, Flags);

    m_0C = FBitSet(Flags, 0);
    m_00 = FBitSet(~Flags, 1);

    DWORD Cell;
    UNPACK(WORD, Cell);

    m_Cell = Block | Cell;

    UNPACK(WORD, m_08);

    // Unpack the cell IDs visible to this one.
    UNPACK(WORD, m_VisCellCount);
    m_VisCellIDs = new DWORD[ m_VisCellCount ];

    for (DWORD i = 0; i < m_VisCellCount; i++)
    {
        DWORD VisCellID;
        UNPACK(WORD, VisCellID);

        m_VisCellIDs[i] = Block | VisCellID;
    }

#ifdef PRE_TOD
    PACK_ALIGN();
#endif
    return TRUE;
}



















