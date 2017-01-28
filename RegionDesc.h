
#pragma once

#include "ObjCache.h"
#include "PString.h"

class CEncounterDesc;
class CSoundDesc;
class CTerrainDesc;
class FileNameDesc;
class CSceneDesc;
class CRegionMisc;
class SkyDesc;

class CRegionDesc : public DBObj
{
public:
    CRegionDesc();
    ~CRegionDesc();

    static DBObj* Allocator();
    static void Destroyer(DBObj*);
    static CRegionDesc* Get(DWORD ID);
    static void Release(CRegionDesc *);

    void Destroy(void);

    BOOL UnPack(BYTE **ppData, ULONG iSize);

    DWORD            m_28;                // 0x1C / 0x28
    PString            m_Name;                // 0x20 / 0x2C
    DWORD            m_Version;            // 0x24 / 0x30
    DWORD            m_34;                // 0x28 / 0x34;
    DWORD            m_38;                // 0x2C / 0x38
    FileNameDesc*    m_FileNameDesc;        // 0x30 / 0x3C
    SkyDesc*        m_SkyDesc;            // 0x34 / 0x40
    CSoundDesc*        m_SoundDesc;        // 0x38 / 0x44
    CSceneDesc*        m_SceneDesc;        // 0x3C / 0x48
    CTerrainDesc*    m_TerrainDesc;        // 0x40 / 0x4C
    CEncounterDesc*    m_EncounterDesc;    // 0x44 / 0x50
    CRegionMisc*    m_RegionMisc;        // 0x58 / 0x64

    static BOOL SetRegion(DWORD Instance, BOOL Hardware);
    static CRegionDesc* current_region;
};




