
#pragma once

#include "PartCell.h"
#include "ObjCache.h"
#include "DArray.h"
#include "Frame.h"

class CLandBlock;
class CPhysicsObj;
class CShadowObj;
class LIGHTOBJ;

class CShadowObj : public LongHashData
{
public:
    CShadowObj();
    virtual ~CShadowObj();

    void set_physobj(CPhysicsObj *pObject);

    CPhysicsObj*    m_PhysObj;        // 0x0C
    DWORD            m_CellID;        // 0x10
    DWORD            m_14;            // 0x14
};

class CObjCell : public CPartCell, public DBObj
{
public:
    CObjCell();
    virtual ~CObjCell();

    void add_object(CPhysicsObj *pObject);
    void add_light(LIGHTOBJ *Light);

    void remove_object(CPhysicsObj *pObject);
    void remove_light(LIGHTOBJ *Light);

    DWORD            m_Water40;            // 0x1C / 0x40
    Position        m_Position;        // 0x20 / 0x44

    DWORD            m_ObjectCount;        // 0x68 / 0x8C
    DArray< CPhysicsObj * >
                    m_Objects;            // 0x6C / 0x90

    DWORD            m_94;                // 0x70 / 0x94
    DWORD            m_98;                // 0x74 / 0x98
    DWORD            m_9C;                // 0x78 / 0x9C

    // Light Information
    DWORD            m_LightCount;        // 0x7C / 0xA0
    DArray< const LIGHTOBJ * >
                    m_Lights;            // 0x80 / 0xA4

    // Shadow Objects, ugh!
    DWORD            m_ShadowObjCount;    // 0x90 / 0xB4
    DArray<CShadowObj *>
                    m_ShadowObjs;        // 0x94 / 0xB8

    DWORD            m_C8;                // 0xA4 / 0xC8
    DWORD            m_CC;                // 0xA8 / 0xCC
    DWORD            m_PVCellCount;        // 0xAC / 0xD0
    DWORD*            m_PVCells;            // 0xB0 / 0xD4
    BOOL            m_D8;                // 0xB4 / 0xD8
    LPVOID            m_DC;                // 0xB8 / 0xDC
    CLandBlock*        m_LandBlock;        // 0xBC / 0xE0

};
