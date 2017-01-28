
#include "StdAfx.h"
#include "Polygon.h"
#include "BSPData.h"
#include "Environment.h"

CEnvironment::CEnvironment() : DBObj(ObjCaches::Environments)
{
    m_StructCount = 0;
    m_Structs = NULL;
}

CEnvironment::~CEnvironment()
{
    Destroy();
}

DBObj* CEnvironment::Allocator()
{
    return((DBObj *)new CEnvironment());
}

void CEnvironment::Destroyer(DBObj* pEnvironment)
{
    delete ((CEnvironment *)pEnvironment);
}

CEnvironment *CEnvironment::Get(DWORD ID)
{
    return (CEnvironment *)ObjCaches::Environments->Get(ID);
}

void CEnvironment::Release(CEnvironment *pEnvironment)
{
    if (pEnvironment)
        ObjCaches::Environments->Release(pEnvironment->m_Key);
}

void CEnvironment::Destroy()
{
    if (m_Structs)
    {
        delete [] m_Structs;
        m_Structs = NULL;
    }

    m_StructCount = 0;
}

BOOL CEnvironment::UnPack(BYTE **ppData, ULONG iSize)
{
    UNPACK(DWORD, m_Key);
    UNPACK(DWORD, m_StructCount);

    m_Structs = new CCellStruct[ m_StructCount ];

    for (DWORD i = 0; i < m_StructCount; i++)
        UNPACK_OBJ(m_Structs[i]);

    return TRUE;
}

CCellStruct *CEnvironment::get_cellstruct(DWORD index)
{
    return &m_Structs[index];
}

CCellStruct::CCellStruct()
{
    m_Index = 0;

    m_14 = 0;
    m_18 = NULL;
    m_1C = 0;
    m_20 = NULL;
    m_24 = 0;
    m_28 = NULL;
    m_2C = NULL;
    m_30 = 0;
    m_34 = NULL;
    m_38 = NULL;
    m_3C = NULL;
}

CCellStruct::~CCellStruct()
{
    Destroy();
}

void CCellStruct::Destroy()
{
    if (m_3C)
    {
        delete m_3C;
        m_3C = NULL;
    }

    if (m_18)
    {
        delete [] m_18;
        m_18 = NULL;
    }
    m_14 = 0;

    if (m_38)
    {
        delete m_38;
        m_38 = NULL;
    }

    if (m_34)
    {
        delete [] m_34;
        m_34 = NULL;
    }
    m_30 = 0;

    if (m_2C)
    {
        delete m_2C;
        m_2C = NULL;
    }

    if (m_28)
    {
        delete [] m_28;
        m_28 = NULL;
    }
    m_24 = 0;

    if (m_20)
    {
        delete [] m_20;
        m_20 = NULL;
    }
    m_1C = 0;

    m_VertexArray.DestroyVertex();
}

CPolygon *CCellStruct::get_portal(WORD index)
{
    for (DWORD i = 0; i < m_14; i++)
    {
        if (index == m_18[i]->m_iPolyIndex)
            return m_18[i];
    }

    return NULL;
}

BOOL CCellStruct::point_in_cell(const Vector& point)
{
    return m_3C->point_inside_cell_bsp(point);
}

BOOL CCellStruct::UnPack(BYTE **ppData, ULONG iSize)
{
    UNPACK(DWORD, m_Index);

    UNPACK(DWORD, m_24);
    UNPACK(DWORD, m_30);
    UNPACK(DWORD, m_14);

    if (!UNPACK_OBJ(m_VertexArray))
        return FALSE;

    CPolygon::SetPackVerts(&m_VertexArray);

    m_28 = new CPolygon[ m_24 ];
    for (DWORD i = 0; i < m_24; i++)
        UNPACK_OBJ(m_28[i]);

    m_18 = new CPolygon*[ m_14 ];
    for (DWORD i = 0; i < m_14; i++)
    {
        DWORD Index;
        UNPACK(WORD, Index);

        m_18[i] = &m_28[Index];
    }

// #ifdef PRE_TOD
    PACK_ALIGN();
// #endif

    BSPNODE::pack_poly = m_28;
    BSPNODE::pack_tree_type = 2;
    m_3C = new BSPTREE;
    UNPACK_POBJ(m_3C);

    m_34 = new CPolygon[ m_30 ];
    for (DWORD i = 0; i < m_30; i++)
        UNPACK_OBJ(m_34[i]);

    BSPNODE::pack_poly = m_34;
    BSPNODE::pack_tree_type = 1;
    m_38 = new BSPTREE;
    UNPACK_POBJ(m_38);
    BSPNODE::pack_poly = m_28;
    
    BOOL LastField;
    UNPACK(DWORD, LastField);

    if (LastField)
    {
        BSPNODE::pack_tree_type = 0;
        m_2C = new BSPTREE;
        UNPACK_POBJ(m_2C);
    }
    
// #ifdef PRE_TOD
    PACK_ALIGN();
// #endif

    return TRUE;
}















