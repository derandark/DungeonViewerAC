
#pragma once

#include "ObjCache.h"
#include "Frame.h"

class BuildInfo;
class CBldPortal;

class CLandBlockInfo : public DBObj
{
public:
    CLandBlockInfo();
    ~CLandBlockInfo();

    static DBObj* Allocator();
    static void Destroyer(DBObj*);
    static CLandBlockInfo* Get(DWORD ID);
    static void Release(CLandBlockInfo *);

    void Destroy();
    BOOL UnPack(BYTE **ppData, ULONG iSize);
    DWORD Unpack_Num_Cells(BYTE *pData);

    DWORD            m_StaticObjCount;    // 0x1C / 0x28
    DWORD*            m_StaticObjIDs;        // 0x20 / 0x2C
    Frame*            m_StaticObjFrames;    // 0x24 / 0x30

    DWORD            m_BuildingCount;    // 0x28 / 0x34
    BuildInfo**        m_Buildings;        // 0x2C / 0x38

    PackableHashTable<DWORD, DWORD>* m_3C; // 0x30 / 0x3C
    LPVOID            m_40;                // 0x34 / 0x40

    DWORD            m_CellCount;        // 0x38 / 0x44
    LPVOID            m_48;                // 0x3C / 0x48
    LPVOID            m_4C;                // 0x40 / 0x4C
};

// What's the real name?
class BuildInfo
{
public:
    BuildInfo();
    ~BuildInfo();

    DWORD m_MeshID;                // 0x00
    Frame m_Frame;                    // 0x04

    DWORD m_44;
    DWORD m_NumPortals;            // 0x48
    CBldPortal** m_Portals;        // 0x4C
};

class CBldPortal
{
public:
    CBldPortal();
    ~CBldPortal();

    BOOL UnPack(DWORD Block, BYTE **ppData, ULONG iSize);

    DWORD m_00;
    DWORD m_Cell;
    DWORD m_08;
    DWORD m_0C;
    DWORD m_VisCellCount; // 0x10
    DWORD* m_VisCellIDs; // 0x14
};







