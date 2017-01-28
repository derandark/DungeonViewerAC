
#include "StdAfx.h"
#include "ObjCache.h"
#include "Vertex.h"

DWORD CVertexArray::vertex_size = 0x20;

CVertexArray::CVertexArray()
{
    m_pAlloc = NULL;
    m_VertexType = 0;
    m_VertexCount = 0;
    m_pVertexBuffer = NULL;
}

CVertexArray::~CVertexArray()
{
}

void CVertexArray::DestroyVertex()
{
    if (m_VertexType == 1)
    {
        for (DWORD i = 0; i < m_VertexCount; i++)
        {
            ((CSWVertex *)((BYTE *)m_pVertexBuffer + i*vertex_size))->Destroy();
        }
    }
    
    free(m_pAlloc);

    m_pAlloc        = NULL;
    m_VertexType    = 0;
    m_VertexCount    = 0;
    m_pVertexBuffer    = NULL;
}

BOOL CVertexArray::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(DWORD, m_VertexType);
    UNPACK(DWORD, m_VertexCount);
    
    if (!AllocateVertex(m_VertexCount, m_VertexType))
        return FALSE;

    switch (m_VertexType)
    {
    case 1:
        for (DWORD i = 0; i < m_VertexCount; i++)
            ((CSWVertex *)((BYTE *)m_pVertexBuffer + i*vertex_size))->UnPack(ppData, iSize);

        break;
    case 2:
    case 3:
        memcpy(m_pVertexBuffer, *ppData, m_VertexCount * 32);
        *ppData = *ppData + (m_VertexCount * 32);

        DEBUGOUT("UnPacking type %i Vertices!\r\n", m_VertexType);

        break;
    }

    return TRUE;
}

BOOL CVertexArray::AllocateVertex(DWORD VertexCount, DWORD VertexType)
{
    LPVOID VertexBuffer;

    m_pAlloc = malloc((VertexCount + 1) * vertex_size);

    if (!m_pAlloc)
        return FALSE;

    VertexBuffer = m_pAlloc;

    if (vertex_size == 0x20)
    {
        // Align to 32-byte boundary.
        size_t Alignment = (size_t)VertexBuffer & 0x1F;

        if (Alignment)
        {
            VertexBuffer = (LPVOID)((size_t)VertexBuffer - Alignment);
            VertexBuffer = (LPVOID)((size_t)VertexBuffer + 0x20);
        }
    }

    m_pVertexBuffer = VertexBuffer;
    m_VertexType = VertexType;
    m_VertexCount = VertexCount;

    if (VertexType == 1)
    {
        for (DWORD i = 0; i < m_VertexCount; i++)
            ((CSWVertex *)((BYTE *)m_pVertexBuffer + i*vertex_size))->Init();
    }

    return TRUE;
}

void CSWVertex::Init()
{
    index        = 0;
    uvcount        = 0;
    uvarray        = NULL;

    if (CVertexArray::vertex_size == 0x28)
    {
        unk20 = 0;
        unk24 = 0;
    }
}

void CSWVertex::Destroy()
{
    if (uvarray)
    {
        delete [] uvarray;
        uvarray = NULL;
    }
}

BOOL CSWVertex::UnPack(BYTE **ppData, ULONG iSize)
{
    Destroy();

    UNPACK(short, index);
    UNPACK(short, uvcount);

    origin.UnPack(ppData, iSize);
    normal.UnPack(ppData, iSize);

    if (uvcount)
    {
        uvarray = new CSWVertexUV[ uvcount ];

        for (int i = 0; i < uvcount; i++)
            uvarray[i].UnPack(ppData, iSize);
    }

#ifdef PRE_TOD
    PACK_ALIGN();
#endif

    return TRUE;
}

CSWVertexUV::CSWVertexUV()
{
}

BOOL CSWVertexUV::UnPack(BYTE **ppData, ULONG iSize)
{
    UNPACK(float, u);
    UNPACK(float, v);

    return TRUE;
}

BBox::BBox()
{
    m_Min = Vector(0, 0, 0);
    m_Max = Vector(0, 0, 0);
}

void BBox::CalcBBox(Vector& Point)
{
    if (Point.x < m_Min.x)
        m_Min.x = Point.x;
    if (Point.x > m_Max.x)
        m_Max.x = Point.x;

    if (Point.y < m_Min.y)
        m_Min.y = Point.y;
    if (Point.y > m_Max.y)
        m_Max.y = Point.y;

    if (Point.z < m_Min.z)
        m_Min.z = Point.z;
    if (Point.z > m_Max.z)
        m_Max.z = Point.z;
}















