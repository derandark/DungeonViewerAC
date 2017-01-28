
#pragma once
#include "ObjCache.h"
#include "Vertex.h"

class CSurfaceTriStrips;
class CPolygon;
class BSPTREE;

class CCellStruct
{
public:
    CCellStruct();
    ~CCellStruct();

    void Destroy();
    BOOL UnPack(BYTE **ppData, ULONG iSize);

    CPolygon*            get_portal(WORD index);
    BOOL                point_in_cell(const Vector& point);

// private:

    DWORD                m_Index;            // 0x00
    CVertexArray        m_VertexArray;        // 0x04
    DWORD                m_14;                // 0x14
    CPolygon**            m_18;                // 0x18

    DWORD                m_1C;                // 0x1C
    CSurfaceTriStrips*    m_20;                // 0x20

    DWORD                m_24;                // 0x24
    CPolygon*            m_28;                // 0x28
    BSPTREE*            m_2C;                // 0x2C - Used for drawing

    DWORD                m_30;                // 0x30
    CPolygon*            m_34;                // 0x34
    BSPTREE*            m_38;                // 0x38
    BSPTREE*            m_3C;                // 0x3C

    // size 0x40
};

class CEnvironment : public DBObj
{
public:
    CEnvironment();
    ~CEnvironment();

    static DBObj* Allocator();
    static void Destroyer(DBObj*);
    static CEnvironment* Get(DWORD ID);
    static void Release(CEnvironment *);

    void Destroy();
    BOOL UnPack(BYTE **ppData, ULONG iSize);

    CCellStruct*        get_cellstruct(DWORD index);

private:

    DWORD                m_StructCount;    // 0x1C / 0x28
    CCellStruct*        m_Structs;        // 0x20 / 0x2C
};




