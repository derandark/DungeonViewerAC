
#include "StdAfx.h"
#include "Polygon.h"
#include "Surface.h"
#include "GfxObj.h"
#include "Render.h"

CGfxObj::CGfxObj() : DBObj(ObjCaches::GfxObjs)
{
    m_dw18 = 0;
    m_dw1C = 0;
    m_dw20 = 0;
    m_dw24 = 0;

    m_AngleVec = Vector(0, 0, 0);

    m_Material = NULL;

    m_TextureCount = 0;
    m_Textures = NULL;

    m_ColTriangleCount = 0;
    m_ColTriangles = NULL;
    m_ColSphere = NULL;
    m_ColTree = NULL;

    m_dw60 = 0;
    m_dw64 = 0;

    m_RenderTriangleCount = 0;
    m_RenderTriangles = NULL;
    m_RenderSphere = NULL;
    m_RenderTree = NULL;
}

CGfxObj::~CGfxObj()
{
    Destroy();
}

DBObj* CGfxObj::Allocator()
{
    return((DBObj *)new CGfxObj());
}

void CGfxObj::Destroyer(DBObj* pGfxObj)
{
    delete ((CGfxObj *)pGfxObj);
}

CGfxObj *CGfxObj::Get(DWORD ID)
{
    return (CGfxObj *)ObjCaches::GfxObjs->Get(ID);
}

void CGfxObj::Release(CGfxObj *pGfxObj)
{
    if (pGfxObj)
        ObjCaches::GfxObjs->Release(pGfxObj->m_Key);
}

void CGfxObj::Destroy()
{
    if (m_RenderTree)
    {
        delete m_RenderTree;
        m_RenderTree = NULL;
    }
    m_RenderSphere = NULL;

    if (m_RenderTriangles)
    {
        delete [] m_RenderTriangles;
        m_RenderTriangles = NULL;
    }
    m_RenderTriangleCount = 0;

    if (m_dw64)
    {
        UNFINISHED("CSurfaceTriStrips::Free call");

        // CSurfaceTriStrips::Free(m_dw64);
        m_dw64 = NULL;

        if (m_RenderSphere)
            delete m_RenderSphere;
    }
    m_dw60 = 0;

    if (m_ColTriangles)
    {
        delete [] m_ColTriangles;
        m_ColTriangles = NULL;
    }

    if (m_ColTree)
    {
        delete m_ColTree;
        m_ColTree = NULL;
    }

    if (m_ColSphere)
    {
        // delete m_ColSphere;
        m_ColSphere = NULL;
    }

    m_VertexArray.DestroyVertex();

    if (m_Textures)
    {
        for (DWORD i = 0; i < m_TextureCount; i++)
        {
            CSurface::Release(m_Textures[i]);
        }

        delete [] m_Textures;
        m_Textures = NULL;
    }
    m_TextureCount = 0;
}

BOOL CGfxObj::UnPack(BYTE **ppData, ULONG iSize)
{
    Destroy();

    UNPACK(DWORD, m_Key);

    DWORD Fields;
    UNPACK(DWORD, Fields);

   /*
#ifdef PRE_TOD
   UNPACK(DWORD, m_TextureCount);
#else
   UNPACK(BYTE, m_TextureCount);
#endif
   */

#ifdef PRE_TOD
      UNPACK(DWORD, m_TextureCount);
#else
      UNPACK(BYTE, m_TextureCount);

      if (m_TextureCount & 0x80)
      {
         DWORD lowbyte;
         UNPACK(BYTE, lowbyte);

         m_TextureCount = ((m_TextureCount & 0x7f) << 8) | lowbyte;
      }
#endif

    m_Textures = new CSurface*[ m_TextureCount ];

    for (DWORD i = 0; i < m_TextureCount; i++)
    {
        DWORD TextureID;
        UNPACK(DWORD, TextureID);

        m_Textures[i] = CSurface::Get(TextureID);
    }

    if (!UNPACK_OBJ(m_VertexArray))
        return FALSE;

    CPolygon::SetPackVerts(&m_VertexArray);

    if (Fields & 1)
    {
        // Collision data
#ifdef PRE_TOD
      UNPACK(DWORD, m_ColTriangleCount);
#else
      UNPACK(BYTE, m_ColTriangleCount);

      if (m_ColTriangleCount & 0x80)
      {
         DWORD lowbyte;
         UNPACK(BYTE, lowbyte);

         m_ColTriangleCount = ((m_ColTriangleCount & 0x7f) << 8) | lowbyte;
      }
#endif

        m_ColTriangles = new CPolygon[  m_ColTriangleCount ];

        for (DWORD i = 0; i < m_ColTriangleCount; i++)
            UNPACK_OBJ(m_ColTriangles[i]);

        BSPNODE::pack_poly = m_ColTriangles;
        BSPNODE::pack_tree_type = 1;

        m_ColTree = new BSPTREE();
        UNPACK_POBJ(m_ColTree);

        m_ColSphere = m_ColTree->GetSphere();
    }

    UNPACK_OBJ(m_AngleVec);

    if (Fields & 2)
    {
        // Rendering data.
#ifdef PRE_TOD
      UNPACK(DWORD, m_RenderTriangleCount);
#else
      UNPACK(BYTE, m_RenderTriangleCount);

      if (m_RenderTriangleCount & 0x80)
      {
         DWORD lowbyte;
         UNPACK(BYTE, lowbyte);

         m_RenderTriangleCount = ((m_RenderTriangleCount & 0x7f) << 8) | lowbyte;
      }
#endif

        m_RenderTriangles = new CPolygon[ m_RenderTriangleCount ];

        for (DWORD i = 0; i < m_RenderTriangleCount; i++)
            UNPACK_OBJ(m_RenderTriangles[i]);

        BSPNODE::pack_poly = m_RenderTriangles;
        BSPNODE::pack_tree_type    = 0;

        m_RenderTree = new BSPTREE();
        UNPACK_POBJ(m_RenderTree);

        m_RenderSphere = m_RenderTree->GetSphere();
    }

    if (Fields & 4)
    {
        // Not done.
        __asm int 3;

        // Alternative rendering data (TriStrips)
        m_RenderSphere = new CSphere;
        UNPACK_POBJ(m_RenderSphere);

        // CSurfaceTriStrips::StripsUnPack(&m_dw60, &m_dw64, ppData, iSize);
    }

#ifdef PRE_TOD
    PACK_ALIGN();
#endif
    init_end();

    return TRUE;
}

void CGfxObj::init_end()
{
    if (m_VertexArray.m_pVertexBuffer)
    {
        m_BBox.m_Min = ((CSWVertex *)m_VertexArray.m_pVertexBuffer)[0].origin;
        m_BBox.m_Max = ((CSWVertex *)m_VertexArray.m_pVertexBuffer)[0].origin;

        for (DWORD i = 1; i < m_VertexArray.m_VertexCount; i++)
            m_BBox.CalcBBox(((CSWVertex *)((BYTE *)m_VertexArray.m_pVertexBuffer + i*CVertexArray::vertex_size))->origin);
    }
    else
    {
        m_BBox.m_Min = Vector(0, 0, 0);
        m_BBox.m_Max = Vector(0, 0, 0);
    }
}

GfxObjDegradeInfo::GfxObjDegradeInfo() : DBObj(ObjCaches::GfxObjDegradeInfos)
{
    m_dw18 = 0;
    m_dw1C = 0;
    m_dw20 = 0;
    m_dw24 = 0;

    m_DegradeCount = 0;
    m_DegradeLevels = NULL;
}

GfxObjDegradeInfo::~GfxObjDegradeInfo()
{
    Destroy();
}

DBObj* GfxObjDegradeInfo::Allocator()
{
    return((DBObj *)new GfxObjDegradeInfo());
}

void GfxObjDegradeInfo::Destroyer(DBObj* pGfxObjDegradeInfo)
{
    delete ((GfxObjDegradeInfo *)pGfxObjDegradeInfo);
}

GfxObjDegradeInfo *GfxObjDegradeInfo::Get(DWORD ID)
{
    return (GfxObjDegradeInfo *)ObjCaches::GfxObjDegradeInfos->Get(ID);
}

void GfxObjDegradeInfo::Release(GfxObjDegradeInfo *pGfxObjDegradeInfo)
{
    if (pGfxObjDegradeInfo)
        ObjCaches::GfxObjDegradeInfos->Release(pGfxObjDegradeInfo->m_Key);
}

void GfxObjDegradeInfo::Destroy()
{
    if (m_DegradeLevels)
    {
        delete [] m_DegradeLevels;
        m_DegradeLevels = NULL;
    }
    m_DegradeCount = 0;
}

BOOL GfxObjDegradeInfo::UnPack(BYTE **ppData, ULONG iSize)
{
    Destroy();

    UNPACK(DWORD, m_Key);
    UNPACK(DWORD, m_DegradeCount);

    m_DegradeLevels = new GfxObjDegradeLevel[ m_DegradeCount ];

    for (DWORD i = 0; i < m_DegradeCount; i++)
    {
        UNPACK(DWORD, m_DegradeLevels[i].m_00);
        UNPACK(DWORD, m_DegradeLevels[i].m_04);
        UNPACK(float, m_DegradeLevels[i].m_08);
        UNPACK(float, m_DegradeLevels[i].m_0C);
        UNPACK(float, m_DegradeLevels[i].m_10);
    }

    return TRUE;
}

float GfxObjDegradeInfo::get_max_degrade_distance(void) const
{
    if (m_DegradeCount > 2)
        return m_DegradeLevels[m_DegradeCount-2].m_10;
    else
        return m_DegradeLevels[0].m_10;
}

void GfxObjDegradeInfo::get_degrade(float ViewerDist, DWORD *GfxIndex, DWORD *GfxFrameMod) const
{
    const BOOL degrades_disabled = TRUE; // FALSE;
    const long force_level = -1;

    if (degrades_disabled)
    {
        *GfxIndex = 0;
        *GfxFrameMod = m_DegradeLevels[0].m_04;
    }
    else
    {
        if (force_level == -1)
        {
            if (Render::m_DegMul < 0.0)
            {
                ViewerDist = fabs(ViewerDist);

                for (DWORD i = 0; i < m_DegradeCount; i++)
                {
                    float val = m_DegradeLevels[i].m_0C + ((m_DegradeLevels[i].m_0C - m_DegradeLevels[i].m_08) * Render::m_DegMul);

                    if (ViewerDist < val)
                    {
                        *GfxIndex = i;
                        *GfxFrameMod = m_DegradeLevels[i].m_04;
                        return;
                    }
                }
            }
            else
            {
                ViewerDist = fabs(ViewerDist);

                for (DWORD i = 0; i < m_DegradeCount; i++)
                {
                    float val = m_DegradeLevels[i].m_0C - ((m_DegradeLevels[i].m_0C - m_DegradeLevels[i].m_10) * Render::m_DegMul);

                    if (ViewerDist < val)
                    {
                        *GfxIndex = i;
                        *GfxFrameMod = m_DegradeLevels[i].m_04;
                        return;
                    }
                }
            }

            *GfxIndex = m_DegradeCount - 1;
            *GfxFrameMod = m_DegradeLevels[m_DegradeCount - 1].m_04;
        }
        else
            UNFINISHED("Degrade level forcing");
    }
}







