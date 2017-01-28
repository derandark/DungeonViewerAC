
#pragma once

#include "ObjCache.h"
#include "Vertex.h"
#include "BSPData.h"

class CMaterial;
class CPolygon;
class CSurface;

class CGfxObj : public DBObj
{
public:
    CGfxObj();
    ~CGfxObj();

    static DBObj* Allocator();
    static void Destroyer(DBObj*);
    static CGfxObj* Get(DWORD ID);
    static void Release(CGfxObj *);

    void Destroy();

    BOOL UnPack(BYTE** ppData, ULONG iSize);

    void init_end();

    DWORD                m_dw18;                // 0x0C / 0x18
    DWORD                m_dw1C;                // 0x10 / 0x1C
    DWORD                m_dw20;                // 0x14 / 0x20
    DWORD                m_dw24;                // 0x18 / 0x24

    CMaterial*            m_Material;            // 0x1C / 0x28
    DWORD                m_TextureCount;            // 0x20 / 0x2C
    CSurface**            m_Textures;                // 0x24 / 0x30
    CVertexArray        m_VertexArray;            // 0x28 / 0x34 - size: 0x10

    DWORD                m_ColTriangleCount;        // 0x38 / 0x44
    CPolygon*            m_ColTriangles;            // 0x3C / 0x48
    CSphere*            m_ColSphere;            // 0x40 / 0x4C
    BSPTREE*            m_ColTree;                // 0x44 / 0x50

    Vector                m_AngleVec;                // 0x48 / 0x54 - size: 0x0C

    DWORD                m_dw60;                    // 0x54 / 0x60
    DWORD                m_dw64;                    // 0x58 / 0x64
    DWORD                m_RenderTriangleCount;    // 0x5C / 0x68
    CPolygon*            m_RenderTriangles;        // 0x60 / 0x6C
    CSphere*            m_RenderSphere;            // 0x64 / 0x70
    BSPTREE*            m_RenderTree;            // 0x68 / 0x74

    BBox                m_BBox;                // 0x6C / 0x78 - size: 0x18?
};

struct GfxObjDegradeLevel
{
    DWORD m_00;
    DWORD m_04;
    float m_08;
    float m_0C;
    float m_10;
};

class GfxObjDegradeInfo : public DBObj
{
public:
    GfxObjDegradeInfo();
    ~GfxObjDegradeInfo();

    static DBObj* Allocator();
    static void Destroyer(DBObj*);
    static GfxObjDegradeInfo* Get(DWORD ID);
    static void Release(GfxObjDegradeInfo *);

    void Destroy();
    BOOL UnPack(BYTE** ppData, ULONG iSize);

    void get_degrade(float ViewerDist, DWORD *GfxIndex, DWORD *GfxFrameMod) const;
    float get_max_degrade_distance(void) const;

    DWORD                    m_dw18;                // 0x0C / 0x18
    DWORD                    m_dw1C;                // 0x10 / 0x1C
    DWORD                    m_dw20;                // 0x14 / 0x20
    DWORD                    m_dw24;                // 0x18 / 0x24

    DWORD                    m_DegradeCount;        // 0x1C / 0x28
    GfxObjDegradeLevel*        m_DegradeLevels;    // 0x20 / 0x2C
};




