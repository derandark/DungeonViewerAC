
#include "StdAfx.h"
#include "DATDisk.h"
#include "ObjCache.h"

#include "RegionDesc.h"
#include "LandBlock.h"
#include "LandBlockInfo.h"
#include "EnvCell.h"
#include "Environment.h"
#include "Setup.h"
#include "GfxObj.h"
#include "Surface.h"
#include "Texture.h"
#include "Palette.h"
#include "Animation.h"
#include "Scripts.h"
#include "Particles.h"

ObjCache *ObjCaches::RegionDescs = NULL;
ObjCache *ObjCaches::LandBlocks = NULL;
ObjCache *ObjCaches::LandBlockInfos = NULL;
ObjCache *ObjCaches::EnvCells = NULL;
ObjCache *ObjCaches::Environments = NULL;
ObjCache *ObjCaches::Setups = NULL;
ObjCache *ObjCaches::GfxObjs = NULL;
ObjCache *ObjCaches::GfxObjDegradeInfos = NULL;
ObjCache *ObjCaches::Surfaces = NULL;
ObjCache *ObjCaches::Textures = NULL;
ObjCache *ObjCaches::Palettes = NULL;
ObjCache *ObjCaches::Animations = NULL;
ObjCache *ObjCaches::PhysicsScripts = NULL;
ObjCache *ObjCaches::PhysicsScriptTables = NULL;
ObjCache *ObjCaches::ParticleEmitterInfos = NULL;
ObjCache *ObjCaches::ImgColors = NULL;

void ObjCaches::OutputCacheInfo()
{
    DEBUGOUT(
        "Total Regions: %u\r\n"
        "Total LandBlocks: %u\r\n"
        "Total LandBlockInfos: %u\r\n"
        "Total EnvironmentCells: %u\r\n"
        "Total Environments: %u\r\n"
        "Total Setups: %u\r\n"
        "Total GraphicsObjects: %u\r\n"
        "Total GraphicsDegrades: %u\r\n"
        "Total Surfaces: %u\r\n"
        "Total Textures: %u\r\n"
        "Total Palettes: %u\r\n"
        "Total Animations: %u\r\n"
        "Total PhysicsScripts: %u\r\n"
        "Total PhysicsScriptTables: %u\r\n"
        "Total ParticleEmitterInfos: %u\r\n",
        RegionDescs->GetCachedCount(),
        LandBlocks->GetCachedCount(),
        LandBlockInfos->GetCachedCount(),
        EnvCells->GetCachedCount(),
        Environments->GetCachedCount(),
        Setups->GetCachedCount(),
        GfxObjs->GetCachedCount(),
        GfxObjDegradeInfos->GetCachedCount(),
        Surfaces->GetCachedCount(),
        Textures->GetCachedCount(),
        Palettes->GetCachedCount(),
        Animations->GetCachedCount(),
        PhysicsScripts->GetCachedCount(),
        PhysicsScriptTables->GetCachedCount(),
        ParticleEmitterInfos->GetCachedCount()
        );

}

void ObjCaches::InitCaches()
{
    RegionDescs = new ObjCache(
        DATDisk::pPortal,
        &CRegionDesc::Allocator,
        &CRegionDesc::Destroyer);

    LandBlocks = new ObjCache(
        DATDisk::pCell,
        &CLandBlock::Allocator,
        &CLandBlock::Destroyer);

    LandBlockInfos = new ObjCache(
        DATDisk::pCell,
        &CLandBlockInfo::Allocator,
        &CLandBlockInfo::Destroyer);

    EnvCells = new ObjCache(
        DATDisk::pCell,
        &CEnvCell::Allocator,
        &CEnvCell::Destroyer);

    Environments = new ObjCache(
        DATDisk::pPortal,
        &CEnvironment::Allocator,
        &CEnvironment::Destroyer);

    Setups = new ObjCache(
        DATDisk::pPortal,
        &CSetup::Allocator,
        &CSetup::Destroyer);

    GfxObjs = new ObjCache(
        DATDisk::pPortal,
        &CGfxObj::Allocator,
        &CGfxObj::Destroyer);

    GfxObjDegradeInfos = new ObjCache(
        DATDisk::pPortal,
        &GfxObjDegradeInfo::Allocator,
        &GfxObjDegradeInfo::Destroyer);

    Palettes = new ObjCache(
        DATDisk::pPortal,
        &Palette::Allocator,
        &Palette::Destroyer);

    Textures = new ObjCache(
        DATDisk::pPortal,
        &ImgTex::Allocator,
        &ImgTex::Destroyer);

    Surfaces = new ObjCache(
        DATDisk::pPortal,
        &CSurface::Allocator,
        &CSurface::Destroyer);

    Animations = new ObjCache(
        DATDisk::pPortal,
        &CAnimation::Allocator,
        &CAnimation::Destroyer);

    PhysicsScripts = new ObjCache(
        DATDisk::pPortal,
        &PhysicsScript::Allocator,
        &PhysicsScript::Destroyer);

    PhysicsScriptTables = new ObjCache(
        DATDisk::pPortal,
        &PhysicsScriptTable::Allocator,
        &PhysicsScriptTable::Destroyer);

    ParticleEmitterInfos = new ObjCache(
        DATDisk::pPortal,
        &ParticleEmitterInfo::Allocator,
        &ParticleEmitterInfo::Destroyer);

   ImgColors = new ObjCache(
      DATDisk::pPortal,
      &ImgColor::Allocator,
      &ImgColor::Destroyer);
}

void ObjCaches::DestroyCaches()
{
    if (RegionDescs)
    {
        delete RegionDescs;
        RegionDescs = NULL;
    }

    if (LandBlocks)
    {
        delete LandBlocks;
        LandBlocks = NULL;
    }

    if (LandBlockInfos)
    {
        delete LandBlockInfos;
        LandBlockInfos = NULL;
    }

    if (EnvCells)
    {
        delete EnvCells;
        EnvCells = NULL;
    }

    if (Environments)
    {
        delete Environments;
        Environments = NULL;
    }

    if (Setups)
    {
        delete Setups;
        Setups = NULL;
    }
    
    if (GfxObjs)
    {
        delete GfxObjs;
        GfxObjs = NULL;
    }

    if (GfxObjDegradeInfos)
    {
        delete GfxObjDegradeInfos;
        GfxObjDegradeInfos = NULL;
    }

    if (Surfaces)
    {
        delete Surfaces;
        Surfaces = NULL;
    }

    if (Textures)
    {
        delete Textures;
        Textures = NULL;
    }

    if (Palettes)
    {
        delete Palettes;
        Palettes = NULL;
    }

    if (Animations)
    {
        delete Animations;
        Animations = NULL;
    }

    if (PhysicsScripts)
    {
        delete PhysicsScripts;
        PhysicsScripts = NULL;
    }

    if (PhysicsScriptTables)
    {
        delete PhysicsScriptTables;
        PhysicsScriptTables = NULL;
    }

    if (ParticleEmitterInfos)
    {
        delete ParticleEmitterInfos;
        ParticleEmitterInfos = NULL;
    }

   if (ImgColors)
    {
        delete ImgColors;
        ImgColors = NULL;
    }
}

DBObj::DBObj(ObjCache* pCache)
    : m_pCache(pCache), m_lLinks(1)
{
}

DBObj::~DBObj()
{
}

long DBObj::Link()
{
    return(++m_lLinks);
}

long DBObj::Unlink() 
{
    return(--m_lLinks);
}

ObjCache::ObjCache(DATDisk *pDisk, DBObj *(*pfnAllocator)(), void (*pfnDestroyer)(DBObj *)) : m_Objects(8)
{
    m_pDisk = pDisk;
    m_pfnAllocator = pfnAllocator;
    m_pfnDestroyer = pfnDestroyer;
}

ObjCache::~ObjCache() 
{
    m_Objects.destroy_contents();
}

DBObj *ObjCache::Get(DWORD ID)
{
    DBObj *pObject = m_Objects.lookup(ID);

    if (!pObject)
    {
        if (!m_pDisk)
            return NULL;

        DATEntry File;

        if (m_pDisk->GetData(ID, &File))
        {
            DEBUGOUT("DATDisk::Get(0x%08X) Success\r\n", ID);
        } else {
            DEBUGOUT("DATDisk::Get(0x%08X) Failure\r\n", ID);
            return NULL;
        }

        pObject = m_pfnAllocator();

        if (pObject)
        {
            BYTE *PackData = File.Data + sizeof(DWORD);

            if (pObject->UnPack(&PackData, File.Length))
            {
				pObject->m_Key = ID;
                // DEBUGOUT("Added object %08X(Memory@%08X) to cache.\r\n", ID, pObject);

                m_Objects.add(pObject);
            }
            else
            {
                delete pObject;
                pObject = NULL;
            }
        }

        delete [] File.Data;
        return pObject;
    }
    else
        pObject->Link();

    return pObject;
}

DWORD ObjCache::GetCachedCount()
{
    HashBaseIter<unsigned long> Iter(&m_Objects);

    DWORD Count = 0;
    while (!Iter.EndReached()) {
        Count++;
        Iter.Next();
    }

    return Count;
}

void ObjCache::Release(DWORD ID)
{
    DBObj *pObject = m_Objects.lookup(ID);

    if (pObject)
    {
        if (pObject->Unlink() <= 0)
        {
            m_Objects.remove(pObject->m_Key);
            m_pfnDestroyer(pObject);
        }
    }
}






