
#pragma once

#include "ObjCache.h"
#include "HashData.h"
#include "MathLib.h"
#include "Frame.h"
#include "BSPData.h"
#include "AnimHooks.h"
#include "Palette.h"

class CGfxObj;

class PlacementType : public LongHashData
{
public:
    PlacementType();
    virtual ~PlacementType();

    BOOL UnPack(DWORD ObjCount, BYTE **ppData, ULONG iSize);

    AnimFrame m_AnimFrame;
};

class LocationType : public LongHashData
{
public:
    LocationType();
    ~LocationType();

    BOOL UnPack(BYTE **ppData, ULONG iSize);

    DWORD m_LandCell; // 0x0C
    Frame m_Frame; // 0x10
};

class LightInfo
{
public:
    LightInfo();
    ~LightInfo();

    BOOL UnPack(BYTE **ppData, ULONG iSize);

    DWORD m_dw00;
    Frame m_Frame; // 0x04
    RGBColor m_RGBColor; // 0x44
    float m_50;
    float m_54;
    DWORD m_58;
};

class CSetup : public DBObj
{
public:
    CSetup();
    ~CSetup();

    static CSetup *makeSimpleSetup(DWORD GfxObjID);
    static CSetup *makeParticleSetup(DWORD ObjCount, Vector *Unused);

    static DBObj* Allocator();
    static void Destroyer(DBObj*);
    static CSetup* Get(DWORD ID);
    static void Release(CSetup *);

    void Destroy();    
    BOOL UnPack(BYTE** ppData, ULONG iSize);

    DWORD            m_dw18;            // 0x0C / 0x18
    DWORD            m_dw1C;
    DWORD            m_dw20;
    DWORD            m_dw24;

    DWORD            m_iObjectCount;        // 0x1C / 0x28
    DWORD*            m_pObjectIDs;        // 0x20 / 0x2C
    DWORD*            m_pObjectUnks;        // 0x24 / 0x30
    Vector*            m_pObjectScales;    // 0x28 / 0x34 

    DWORD            m_CylSphereCount;    // 0x2C / 0x38
    CCylSphere*        m_pCylSpheres;        // 0x30 / 0x3C

    DWORD            m_SphereCount;        // 0x34 / 0x40
    CSphere*        m_pSpheres;            // 0x38 / 0x44

    BOOL            m_bType8;            // 0x3C / 0x48
    BOOL            m_bType4;            // 0x40 / 0x4C

    float            m_fHeight;            // 0x44 / 0x50
    float            m_fRadius;            // 0x48 / 0x54
    DWORD            m_dw58;                // 0x4C / 0x58
    DWORD            m_dw5C;                // 0x50 / 0x5C

    CSphere            m_bounds;            // 0x54 / 0x60
    CSphere            m_bounds2;            // 0x64 / 0x70

    DWORD            m_LightCount;        // 0x74 / 0x80
    LightInfo*        m_Lights;            // 0x78 / 0x84

    CSolid            m_Solid88;            // 0x7C / 0x88

    LongHash< LocationType >* m_p94;    // 0x88 / 0x94
    LongHash< LocationType >* m_p98;    // 0x8C / 0x98
    LongHash< PlacementType > m_Placements9C;    // 0x90 / 0x9C

    DWORD            m_DefaultAnim;        // 0xA8 / 0xB4
    DWORD            m_DefaultScript;    // 0xAC / 0xB8
    DWORD            m_DefaultMotionTable; // 0xB0 / 0xBC
    DWORD            m_DefaultSoundTable; // 0xB4 / 0xC0
    DWORD            m_DefaultScriptTable; // 0xB8 / 0xC4
};



