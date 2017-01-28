
#include "StdAfx.h"
#include "GfxObj.h"
#include "Setup.h"

PlacementType::PlacementType()
{
}

PlacementType::~PlacementType()
{
}

BOOL PlacementType::UnPack(DWORD ObjCount, BYTE **ppData, ULONG iSize)
{
    UNPACK(DWORD, m_Key);
    m_AnimFrame.UnPack(ObjCount, ppData, iSize);

    return TRUE;
}

LocationType::LocationType()
{
    m_LandCell = 0;
}

LocationType::~LocationType()
{
}

BOOL LocationType::UnPack(BYTE **ppData, ULONG iSize)
{
    UNPACK(DWORD, m_Key);
    UNPACK(DWORD, m_LandCell);
    UNPACK_OBJ(m_Frame);

    return TRUE;
}

LightInfo::LightInfo()
{
    m_dw00 = 0;
}

LightInfo::~LightInfo()
{
}

BOOL LightInfo::UnPack(BYTE **ppData, ULONG iSize)
{
    UNPACK(DWORD, m_dw00);

    UNPACK_OBJ(m_Frame);
    UNPACK_OBJ(m_RGBColor);

    UNPACK(float, m_50);
    UNPACK(float, m_54);
    UNPACK(DWORD, m_58);

    return TRUE;
}

CSetup::CSetup() : DBObj(ObjCaches::Setups), m_Placements9C(2)
{
    m_dw18 = 0;
    m_dw1C = 0;
    m_dw20 = 0;
    m_dw24 = 0;

    m_iObjectCount = 0;
    m_pObjectIDs = NULL;
    m_pObjectUnks = NULL;
    m_pObjectScales = NULL;

    m_CylSphereCount = 0;
    m_pCylSpheres = NULL;

    m_SphereCount = 0;
    m_pSpheres = NULL;

    m_bType8 = FALSE;
    m_bType4 = TRUE; 

    m_fHeight = 0.0f;
    m_fRadius = 0.0f;
    m_dw58 = 0;
    m_dw5C = 0;

    m_bounds    = CSphere(Vector(0.0f, 0.0f, 0.0f), 0.0f); // m_bounds = null sphere
    m_bounds2    = CSphere(Vector(0.0f, 0.0f, 0.0f), 0.0f); // m_bounds2 = null sphere

    m_LightCount = 0;
    m_Lights = NULL;

    m_Solid88 = CSolid(Vector(1.0f, 1.0f, 1.0f));

    m_p94 = NULL;
    m_p98 = NULL;
}

CSetup::~CSetup()
{
    Destroy();
}

DBObj* CSetup::Allocator()
{
    return((DBObj *)new CSetup());
}

void CSetup::Destroyer(DBObj *pSetup)
{
    delete ((CSetup *)pSetup);
}

CSetup* CSetup::Get(DWORD ID)
{
    return (CSetup *)ObjCaches::Setups->Get(ID);
}

void CSetup::Release(CSetup *pSetup)
{
    if (pSetup)
        ObjCaches::Setups->Release(pSetup->m_Key);
}

void CSetup::Destroy()
{
    if (m_pObjectScales)
    {
        delete [] m_pObjectScales;
        m_pObjectScales = NULL;
    }

    if (m_pObjectUnks)
    {
        delete [] m_pObjectUnks;
        m_pObjectUnks = NULL;
    }

    if (m_pObjectIDs)
    {
        delete [] m_pObjectIDs;
        m_pObjectIDs = NULL;
    }

    m_iObjectCount = 0;

    if (m_pCylSpheres)
    {
        delete [] m_pCylSpheres;
        m_pCylSpheres = NULL;
    }

    m_CylSphereCount = 0;

    if (m_pSpheres)
    {
        delete [] m_pSpheres;
        m_pSpheres = NULL;
    }

    m_SphereCount = 0;

    if (m_Lights)
    {
        delete [] m_Lights;
        m_Lights = NULL;
    }

    m_LightCount = 0;

    if (m_p94)
    {
        m_p94->destroy_contents();
        delete m_p94;
        m_p94 = NULL;
    }

    if (m_p98)
    {
        m_p98->destroy_contents();
        delete m_p98;
        m_p98 = NULL;
    }

    m_Placements9C.destroy_contents();

    m_DefaultAnim = 0;
    m_DefaultScript = 0;
    m_DefaultMotionTable = 0;
    m_DefaultSoundTable = 0;
    m_DefaultScriptTable = 0;
}

CSetup *CSetup::makeParticleSetup(DWORD ObjCount, Vector *Unused)
{
    CSetup *pSetup = new CSetup();

    if (!pSetup)
        return NULL;

    pSetup->m_Key = 0;
    pSetup->m_lLinks = 0;
    pSetup->m_iObjectCount = ObjCount;
    pSetup->m_pObjectIDs = NULL;

    return pSetup;
}

CSetup *CSetup::makeSimpleSetup(DWORD GfxObjID)
{
    CSetup *pSetup = new CSetup();

    if (!pSetup)
        return NULL;

    pSetup->m_Key = 0;
    pSetup->m_lLinks = 0;

    if (!(pSetup->m_pObjectIDs = new DWORD[1]))
    {
        delete pSetup;
        return NULL;
    }

    pSetup->m_iObjectCount = 1;
    pSetup->m_pObjectIDs[0] = GfxObjID;

    CGfxObj *pGfxObj = CGfxObj::Get(GfxObjID);

    if (pGfxObj)
    {
        // Use collision sphere?
    
        if (pGfxObj->m_ColSphere )
            pSetup->m_bounds = *pGfxObj->m_ColSphere;
        else if (pGfxObj->m_RenderSphere)
            pSetup->m_bounds = *pGfxObj->m_RenderSphere;

        CGfxObj::Release(pGfxObj);
    }

    // This isn't right, since hash's aren't declared like this, but we'll use it for now.
    PlacementType* pPT = new PlacementType;

    if (!pPT)
    {
        delete pSetup;
        return NULL;
    }
    
    pPT->m_Key = 0;
    pPT->m_AnimFrame.m_FrameCount = 1;
    pPT->m_AnimFrame.m_pFrames = new AFrame[ 1 ];
    pSetup->m_Placements9C.add(pPT);

    return pSetup;
}

BOOL CSetup::UnPack(BYTE** ppData, ULONG iSize)
{
    Destroy();

    BYTE bTypeID;

    UNPACK(DWORD, m_Key);
    UNPACK(DWORD, bTypeID);

    m_bType4 = (bTypeID & 4)? TRUE : FALSE;
    m_bType8 = (bTypeID & 8)? TRUE : FALSE;

    UNPACK(DWORD, m_iObjectCount);

    if (m_iObjectCount)
    {
        m_pObjectIDs = new DWORD[ m_iObjectCount ];

        for (DWORD i = 0; i < m_iObjectCount; i++)
            UNPACK(DWORD, m_pObjectIDs[i]);
    }

    if (bTypeID & 1)
    {
        m_pObjectUnks = new DWORD[ m_iObjectCount ];

        for (DWORD i = 0; i < m_iObjectCount; i++)
            UNPACK(DWORD, m_pObjectUnks[i]);
    }

    if (bTypeID & 2)
    {
        m_pObjectScales = new Vector[ m_iObjectCount ];

        for (unsigned int i = 0; i < m_iObjectCount; i++)
            UNPACK_OBJ(m_pObjectScales[i]);
    }

    DWORD LT94Count;
    UNPACK(DWORD, LT94Count);

    if (LT94Count > 0)
    {
        m_p94 = new LongHash< LocationType >(4);

        for (DWORD i = 0; i < LT94Count; i++)
        {
            LocationType* pLocation = new LocationType;
            UNPACK_POBJ(pLocation);

            m_p94->add(pLocation);
        }
    }

    DWORD LT98Count;
    UNPACK(DWORD, LT98Count);

    if (LT98Count > 0)
    {
        m_p98 = new LongHash< LocationType >(4);

        for (DWORD i = 0; i < LT98Count; i++)
        {
            LocationType* pLocation = new LocationType;
            UNPACK_POBJ(pLocation);

            m_p98->add(pLocation);
        }
    }

    DWORD PT9CCount;
    UNPACK(DWORD, PT9CCount);

    if (PT9CCount > 0)
    {
        for (DWORD i = 0; i < PT9CCount; i++)
        {
            PlacementType* pPlacement =    new PlacementType;
            pPlacement->UnPack(m_iObjectCount, ppData, iSize);

            m_Placements9C.add(pPlacement);
        }
    }
   
    UNPACK(DWORD, m_CylSphereCount);

    if (m_CylSphereCount > 0)
    {
        m_pCylSpheres = new CCylSphere[ m_CylSphereCount ];

        for (unsigned int i = 0; i < m_CylSphereCount; i++)
            UNPACK_OBJ(m_pCylSpheres[i]);
    }

    UNPACK(DWORD, m_SphereCount);

    if (m_SphereCount > 0)
    {
        m_pSpheres = new CSphere[ m_SphereCount ];

        for (unsigned int i = 0; i < m_SphereCount; i++)
            m_pSpheres[i].UnPack(ppData, iSize);
    }

    UNPACK(float, m_fHeight);
    UNPACK(float, m_fRadius);
    UNPACK(DWORD, m_dw5C);
    UNPACK(DWORD, m_dw58);

    m_bounds.UnPack(ppData, iSize);
    m_bounds2.UnPack(ppData, iSize);

    UNPACK(DWORD, m_LightCount);

    if (m_LightCount > 0)
    {
        m_Lights = new LightInfo[ m_LightCount ];

        for (unsigned int i = 0; i < m_LightCount; i++)
            UNPACK_OBJ(m_Lights[i]);
    }

    UNPACK(DWORD, m_DefaultAnim);
    UNPACK(DWORD, m_DefaultScript);
    UNPACK(DWORD, m_DefaultMotionTable);
    UNPACK(DWORD, m_DefaultSoundTable);
    UNPACK(DWORD, m_DefaultScriptTable);

#ifdef PRE_TOD
    PACK_ALIGN();
#endif

    return TRUE;
}























