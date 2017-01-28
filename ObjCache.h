
#pragma once

#include "HashData.h"
#include "PackObj.h"

class DATDisk;
class ObjCache;

class DBObj :
    public LongHashData,
    public PackObj
{
public:
    DBObj(ObjCache*);
    virtual ~DBObj();

    long Link();
    long Unlink();

protected:
    ObjCache*    m_pCache;    // 0x10
    long        m_lLinks;    // 0x14
};

class ObjCache
{
public:
    ObjCache(DATDisk *pDisk, DBObj *(*pfnAllocator)(), void (*pfnDestroyer)(DBObj *));
    virtual ~ObjCache();

    DBObj *Get(DWORD ID);
    void Release(DWORD ID);

    DWORD GetCachedCount();

protected:

    DATDisk *m_pDisk;

    DBObj *(*m_pfnAllocator)();
    void (*m_pfnDestroyer)(DBObj *);

    LongHash< DBObj > m_Objects;
};

namespace ObjCaches
{
    extern ObjCache *RegionDescs;
    extern ObjCache *LandBlocks;
    extern ObjCache *LandBlockInfos;
    extern ObjCache *EnvCells;
    extern ObjCache *Environments;
    extern ObjCache *Setups;
    extern ObjCache *GfxObjs;
    extern ObjCache *GfxObjDegradeInfos;
    extern ObjCache *Surfaces;
    extern ObjCache *Textures;
    extern ObjCache *Palettes;
    extern ObjCache *Animations;
    extern ObjCache *PhysicsScripts;
    extern ObjCache *PhysicsScriptTables;
    extern ObjCache *ParticleEmitterInfos;
   extern ObjCache *ImgColors;

    extern void InitCaches();
    extern void DestroyCaches();
    extern void OutputCacheInfo();
};

#define DECLARE_CACHE_FUNCTIONS(classname) \
    static DBObj* Allocator(); \
    static void Destroyer(DBObj *); \
    static classname *Get(DWORD ID); \
    static void Release(classname *);

#define DEFINE_CACHE_FUNCTIONS(cachename, classname) \
    DBObj* classname::Allocator() { \
        return((DBObj *)new classname()); \
    } \
    void classname::Destroyer(DBObj* pObj) { \
        delete ((classname *)pObj); \
    } \
    classname *classname::Get(DWORD ID) { \
        return (classname *)ObjCaches::cachename->Get(ID); \
    } \
    void classname::Release(classname *pObj) { \
        if (pObj) \
            ObjCaches::cachename->Release(pObj->m_Key); \
    }










