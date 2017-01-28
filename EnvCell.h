
#pragma once

#include "ObjCache.h"
#include "ObjCell.h"

class CSurface;
class CPhysicsObj;
class CEnvironment;
class CCellStruct;
class CCellPortal;
class CPolygon;

class CEnvCell : public CObjCell
{
public:
    CEnvCell();
    ~CEnvCell();

    static DBObj* Allocator();
    static void Destroyer(DBObj*);
    static CEnvCell* Get(DWORD ID);
    static void Release(CEnvCell *);

    void Destroy();
    BOOL UnPack(BYTE **ppData, ULONG iSize);

    void calc_clip_planes();
    void init_static_objects();
    BOOL point_in_cell(const Vector& point);

    DWORD            m_SurfaceCount;    // 0xC4 / 0xE8
    CSurface**        m_Surfaces;        // 0xC8 / 0xEC
    CCellStruct*    m_CellStruct;        // 0xCC / 0xF0
    CEnvironment*    m_Environment;        // 0xD0 / 0xF4
    DWORD            m_PortalCount;        // 0xD4 / 0xF8
    CCellPortal*    m_Portals;            // 0xD8 / 0xFC
    DWORD            m_StaticObjCount;    // 0xDC / 0x100
    DWORD*            m_StaticObjIDs;    // 0xE0 / 0x104
    Frame*            m_StaticObjFrames; // 0xE4 / 0x108
    CPhysicsObj**   m_StaticObjs;        // 0xE8 / 0x10C
    LPVOID            m_PolyLights;        // 0xEC / 0x110
    DWORD            m_114;                // 0xF0 / 0x114
    DWORD            m_118;                // 0xF4 / 0x118
    LPVOID            m_11C;                // 0xF8 / 0x11C
    DWORD            m_120;                // 0xFC / 0x120
    DWORD            m_124;                // 0x100 / 0x124
    DWORD            m_128;                // 0x104 / 0x128
};

class CCellPortal
{
public:
    CCellPortal();
    ~CCellPortal();

    BOOL UnPack(DWORD LandBlock, WORD *PolyIndex, BYTE **ppData, ULONG iSize);

    DWORD        m_CellID;
    DWORD        m_04;
    CPolygon*    m_PortalPoly; // 0x08
    DWORD        m_0C;
    long        m_10;
    BOOL        m_14;
};





