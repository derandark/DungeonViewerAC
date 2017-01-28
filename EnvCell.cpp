
#include "StdAfx.h"
#include "Surface.h"
#include "PhysicsObj.h"
#include "Environment.h"
#include "EnvCell.h"

CEnvCell::CEnvCell()
{
    m_SurfaceCount = 0;
    m_Surfaces = NULL;
    m_CellStruct = NULL;
    m_Environment = NULL;
    m_PortalCount = 0;
    m_Portals = NULL;
    m_StaticObjCount = 0;
    m_StaticObjIDs = NULL;
    m_StaticObjFrames = NULL;
    m_StaticObjs = NULL;

    m_11C = NULL;
    m_128 = 0;
    m_124 = 0;
    m_120 = 0x80;

    m_114 = 0;
    m_118 = 0;
    m_PolyLights = NULL;
}

CEnvCell::~CEnvCell()
{
    Destroy();

    delete [] m_11C;
    m_11C = NULL;
}

DBObj* CEnvCell::Allocator()
{
    return((DBObj *)new CEnvCell());
}

void CEnvCell::Destroyer(DBObj* pEnvCell)
{
    delete ((CEnvCell *)pEnvCell);
}

CEnvCell *CEnvCell::Get(DWORD ID)
{
    return (CEnvCell *)ObjCaches::EnvCells->Get(ID);
}

void CEnvCell::Release(CEnvCell *pEnvCell)
{
    if (pEnvCell)
        ObjCaches::EnvCells->Release(pEnvCell->m_Key);
}

void CEnvCell::Destroy()
{
    if (m_Surfaces)
    {
        for (DWORD i = 0; i < m_SurfaceCount; i++)
            CSurface::Release(m_Surfaces[i]);

        delete [] m_Surfaces;
        m_Surfaces = NULL;
    }

    // Missing code here

    if (m_Environment)
    {
        CEnvironment::Release(m_Environment);
        m_Environment = NULL;
    }

    // Missing code here

    if (m_Portals)
    {
        delete [] m_Portals;
        m_Portals = NULL;
    }
    m_PortalCount = 0;

    if (m_PVCells)
    {
        delete [] m_PVCells;
        m_PVCells = NULL;
    }
    m_PVCellCount = 0;

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

    if (m_StaticObjs)
    {
        for (DWORD i = 0; i < m_StaticObjCount; i++)
        {
            if (m_StaticObjs[i])
            {
                m_StaticObjs[i]->leave_world();
                delete m_StaticObjs[i];
            }
        }

        delete [] m_StaticObjs;
        m_StaticObjs = NULL;
    }
    m_StaticObjCount = 0;

    m_CellStruct = NULL;
    m_D8 = FALSE;
}

BOOL CEnvCell::UnPack(BYTE **ppData, ULONG iSize)
{
#ifdef PRE_TOD
#else
   UNPACK(DWORD, m_Key);
#endif

    DWORD Flags;
    
    UNPACK(DWORD, Flags);
    m_D8 = FBitSet(Flags, 0);

    UNPACK(DWORD, m_Key);

    DWORD LandBlock = m_Key & 0xFFFF0000;

    UNPACK(BYTE, m_SurfaceCount); 
    UNPACK(BYTE, m_PortalCount); 
    UNPACK(WORD, m_PVCellCount);

    if (m_SurfaceCount & 0x80)
    {
       // __asm int 3;
       DEBUGOUT("Warning: m_SurfaceCount & 0x80?\r\n");
    }

    m_Surfaces = new CSurface*[ m_SurfaceCount ];
    for (DWORD i = 0; i < m_SurfaceCount; i++)
    {
        DWORD SurfaceID;
        UNPACK(WORD, SurfaceID);
        SurfaceID |= 0x08000000;

        m_Surfaces[i] = CSurface::Get(SurfaceID);
    }

#ifdef PRE_TOD
    PACK_ALIGN();
#endif

    DWORD EnvironmentID, StructIndex;

    UNPACK(WORD, EnvironmentID);
    EnvironmentID |= 0x0D000000;

    UNPACK(WORD, StructIndex);

    m_Environment = CEnvironment::Get(EnvironmentID);

    if (m_Environment)
        m_CellStruct = m_Environment->get_cellstruct(StructIndex);

    m_Position.m_LandCell = m_Key;
    UNPACK_OBJ(m_Position.m_Frame);

    if (m_PortalCount > 0)
    {
        m_Portals = new CCellPortal[ m_PortalCount ];

        for (DWORD i = 0; i < m_PortalCount; i++)
        {
            // This is the index of the portal polygon.
            WORD PolyIndex;

            m_Portals[i].UnPack(LandBlock, &PolyIndex, ppData, iSize);
            m_Portals[i].m_PortalPoly = m_CellStruct->get_portal(PolyIndex);
        }
    }
#ifdef PRE_TOD
    PACK_ALIGN();
#endif

    if (m_PVCellCount > 0)
    {
        m_PVCells = new DWORD[ m_PVCellCount ];

        for (DWORD i = 0; i < m_PVCellCount; i++)
        {
            DWORD CellID;
            UNPACK(WORD, CellID);

            m_PVCells[i] = LandBlock | CellID;
        }
    }
#ifdef PRE_TOD
    PACK_ALIGN();
#endif

    if (Flags & 2)
    {
        UNPACK(DWORD, m_StaticObjCount);

        if (m_StaticObjCount > 0)
        {
            m_StaticObjIDs        = new DWORD[ m_StaticObjCount ];
            m_StaticObjFrames    = new Frame[ m_StaticObjCount ];

            for (DWORD i = 0; i < m_StaticObjCount; i++)
            {
                UNPACK(DWORD, m_StaticObjIDs[i]);
                UNPACK_OBJ(m_StaticObjFrames[i]);
            }
        }
    }
#ifdef PRE_TOD
    PACK_ALIGN();
#endif

    if (Flags & 8)
        UNPACK(DWORD, m_C8);
    else
        m_C8 = 0;

    // PolyLightCache..
    // m_PolyLights = new Vector[ m_CellStruct->m_VertexArray.m_VertexCount ];

    calc_clip_planes();

    return TRUE;
}

void CEnvCell::calc_clip_planes()
{
    // DEBUGOUT("Unfinished calc clipplanes!\r\n");
}

void CEnvCell::init_static_objects()
{
    if (m_StaticObjs)
    {
        // Not done.
        for (DWORD i = 0; i < m_StaticObjCount; i++)
        {
            if (m_StaticObjs[i])
            {
                if (!m_StaticObjs[i]->is_completely_visible())
                    m_StaticObjs[i]->calc_cross_cells_static();
            }
        }
    }
    else
    {
        if (m_StaticObjCount > 0)
        {
            m_StaticObjs = new CPhysicsObj*[ m_StaticObjCount ];

            for (DWORD i = 0; i < m_StaticObjCount; i++)
            {
                if (m_StaticObjIDs[i])
                {
                    DEBUGOUT("Making static object 0x%08X\r\n", m_StaticObjIDs[i]);
                    m_StaticObjs[i] = CPhysicsObj::makeObject(m_StaticObjIDs[i], 0, FALSE);
                }
                else
                    m_StaticObjs[i] = NULL;

                if (m_StaticObjs[i])
                    m_StaticObjs[i]->add_obj_to_cell(this, &m_StaticObjFrames[i]);
            }
        }
    }
}

CCellPortal::CCellPortal()
{
    m_CellID = 0;
    m_04 = 0;
    m_PortalPoly = NULL;
    m_0C = 0;
    m_10 = -1;
    m_14 = -1;
}

CCellPortal::~CCellPortal()
{
}

BOOL CCellPortal::UnPack(DWORD LandBlock, WORD *PolyIndex, BYTE **ppData, ULONG iSize)
{
    BYTE Flags;
    UNPACK(WORD, Flags);

    m_14 = FBitSet(Flags, 0);
    m_0C = FBitSet(~Flags, 1);

    UNPACK(WORD, *PolyIndex);

    WORD CellID;
    UNPACK(WORD, CellID);

    if (Flags & 4)
        m_CellID = -1;
    else
        m_CellID = LandBlock | CellID;

    UNPACK(short, m_10);

    return TRUE;
}

BOOL CEnvCell::point_in_cell(const Vector& point)
{
    if (!m_Portals)
        return FALSE;

    Vector local_point = m_Position.m_Frame.globaltolocal(point);

    return m_CellStruct->point_in_cell(local_point);
}























