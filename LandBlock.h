
#pragma once
#include "ObjCache.h"
#include "LandBlockStruct.h"
#include "LandBlockInfo.h"
#include "Frame.h"
#include "DArray.h"

class CPhysicsObj;
class CBuildingObj;

class CLandBlock : public DBObj
{
public:
    CLandBlock();
    ~CLandBlock();

    static DBObj* Allocator();
    static void Destroyer(DBObj*);
    static CLandBlock* Get(DWORD ID);
    static void Release(CLandBlock *);

    static void init(void);
    
    void Destroy();
    BOOL UnPack(BYTE** ppData, ULONG iSize);

    void add_static_object(CPhysicsObj *pObject);
    void destroy_static_objects(void);
    void destroy_buildings(void);
    void get_land_limits(void);
    void init_lcell_ptrs(void);
    void notify_change_size(void);
    void release_all(void);

    CLandBlockStruct    m_LBStruct;        // 0x1C / 0x28
    Frame                m_Frame;            // 0x6C / 0x78
    float                m_MinHeight;        // 0xAC / 0xB8
    float                m_MaxHeight;        // 0xB0 / 0xBC
    DWORD                m_C0;                // 0xB4 / 0xC0
    BOOL                m_bLBInfo;            // 0xB8 / 0xC4
    DWORD                m_C8;                // 0xBC / 0xC8
    DWORD                m_CC;                // 0xC0 / 0xCC
    DWORD                m_D0;                // 0xC4 / 0xD0
    DWORD                m_D4;                // 0xC8 / 0xD4
    CLandBlockInfo*        m_pLBInfo;            // 0xCC / 0xD8

    DWORD                m_StaticObjCount;    // 0xD0 / 0xDC
    DArray<CPhysicsObj *>
                        m_StaticObjs;        // 0xD4 / 0xE0

    DWORD                m_BuildingCount;    // 0xE4 / 0xF0
    CBuildingObj**        m_Buildings;        // 0xE8 / 0xF4
    DWORD                m_F8;                // 0xEC / 0xF8
    LPVOID                m_FC;                // 0xF0 / 0xFC
    LPVOID                m_108;                // 0xFC / 0x108
    DWORD                m_10C;                // 0x100 / 0x10C

    // Size 0x110
};



