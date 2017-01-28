
#include "StdAfx.h"
#include "Material.h"
#include "GfxObj.h"
#include "Surface.h"
#include "Palette.h"
#include "Render.h"
#include "PhysicsPart.h"
#include "PhysicsObj.h"

DWORD CPhysicsPart::player_iid = -1;

CPhysicsPart::CPhysicsPart()
{
    m_GfxObjs = NULL;

    m_Scale = Vector(1.0f, 1.0f, 1.0f);

    m_PhysicsObj = NULL;
    m_dw14 = 0;
    m_DegradeInfo = NULL;
    m_PartIndex = -1;
    m_dw18 = 1;
    m_ViewerAngle = Vector(0, 0, 1);
    m_dw1C = 0;
    m_Material    = NULL;
    m_Textures    = NULL;
    m_BasePal    = NULL;
    m_BasePalID = 0;

    m_ViewerDist = FLT_MAX;
}

CPhysicsPart::~CPhysicsPart()
{
    if (m_BasePal)
    {
        Palette::releasePalette(m_BasePal);
        m_BasePal = NULL;
    }

    if (m_GfxObjs && m_GfxObjs[0])
    {
        if (m_Material != m_GfxObjs[0]->m_Material)
        {
            if (m_Material)
                delete m_Material;
        }

        m_Material = NULL;
    }

    RestoreSurfaces();
    ReleaseGfxObjArray(m_DegradeInfo, m_GfxObjs);
}

CPhysicsPart *CPhysicsPart::makePhysicsPart(DWORD ID)
{
    CPhysicsPart *pPart = new CPhysicsPart();

    if (!pPart->SetPart(ID))
    {
        delete pPart;
        pPart = NULL;
    }

    return pPart;
}

BOOL CPhysicsPart::SetPart(DWORD ID)
{
    if (!ID)
        return FALSE;

    GfxObjDegradeInfo* deginfo = NULL;
    CGfxObj** objarray = NULL;

    if (!LoadGfxObjArray(ID, deginfo, objarray))
        return FALSE;

    SetGfxObjArray(deginfo, objarray);
    return TRUE;
}

void CPhysicsPart::RestoreSurfaces()
{
    if (m_GfxObjs)
    {
        if (m_GfxObjs[0])
        {
            if (m_GfxObjs[0]->m_Textures != m_Textures)
            {
                for (DWORD i = 0; i < m_GfxObjs[0]->m_TextureCount; i++)
                    CSurface::releaseCustomSurface(m_Textures[i]);

                delete [] m_Textures;
                m_Textures = m_GfxObjs[0]->m_Textures;
            }
        }
    }
}

void CPhysicsPart::DetermineBasePal()
{
    for (DWORD i = 0; i < m_GfxObjs[0]->m_TextureCount; i++)
    {
        if (m_Textures[i]->GetOriginalPaletteID())
        {
            m_BasePalID = m_Textures[i]->GetOriginalPaletteID();
            return;
        }
    }

    m_BasePalID = 0;
}

BOOL CPhysicsPart::UsePalette(Palette *pPalette)
{
    if (!pPalette)
        return FALSE;

    if (m_Textures == m_GfxObjs[0]->m_Textures && !CopySurfaces())
        return FALSE;

    for (DWORD i = 0; i < m_GfxObjs[0]->m_TextureCount; i++)
        m_Textures[i]->UsePalette(pPalette);

    if (m_BasePal)
    {
        Palette::releasePalette(m_BasePal);
        m_BasePal = NULL;
    }

    m_BasePal = Palette::copyRef(pPalette);
    return TRUE;
}

BOOL CPhysicsPart::CopySurfaces()
{
    if (m_Textures != m_GfxObjs[0]->m_Textures)
        return TRUE;

    m_Textures = new CSurface*[ m_GfxObjs[0]->m_TextureCount ];

    if (!m_Textures)
        return FALSE;

    for (DWORD i = 0; i < m_GfxObjs[0]->m_TextureCount; i++)
    {
        m_Textures[i] = CSurface::makeCustomSurface(m_GfxObjs[0]->m_Textures[i]);

        if (!m_Textures[i])
            return FALSE;
    }

    return TRUE;
}

void CPhysicsPart::SetGfxObjArray(GfxObjDegradeInfo* DegradeInfo, CGfxObj** Objects)
{
    RestoreSurfaces();
    ReleaseGfxObjArray(m_DegradeInfo, m_GfxObjs);

    m_DegradeInfo = DegradeInfo;
    m_GfxObjs = Objects;
    m_Textures = Objects[0]->m_Textures;

    DetermineBasePal();
    
    if (m_BasePal)
        UsePalette(m_BasePal);
}

BOOL CPhysicsPart::LoadGfxObjArray(DWORD ID, GfxObjDegradeInfo*& DegradeInfo, CGfxObj**& Objects)
{
    DegradeInfo = GfxObjDegradeInfo::Get(0x11000000 | (ID & 0xFFFFFF));

    if (DegradeInfo)
    {
        Objects = new CGfxObj*[ DegradeInfo->m_DegradeCount ];

        for (DWORD i = 0; i < DegradeInfo->m_DegradeCount; i++)
            Objects[i] = CGfxObj::Get(ID);
    }
    else
    {
        Objects    = new CGfxObj*[1];

        Objects[0] = CGfxObj::Get(ID);
    }

    if (!Objects[0])
    {
        ReleaseGfxObjArray(DegradeInfo, Objects);
        return FALSE;
    }

    return TRUE;
}

void CPhysicsPart::ReleaseGfxObjArray(GfxObjDegradeInfo*& DegradeInfo, CGfxObj**& Objects)
{
    DWORD ObjCount;

    if (DegradeInfo)
    {
        ObjCount = DegradeInfo->m_DegradeCount;
        GfxObjDegradeInfo::Release(DegradeInfo);
        DegradeInfo = NULL;
    }
    else
        ObjCount = 1;
    
    if (Objects)
    {
        for (DWORD i = 0; i < ObjCount; i++)
        {
            if (Objects[i])
            {
                CGfxObj::Release(Objects[i]);
                Objects[i] = NULL;
            }
        }

        delete [] Objects;
        Objects = NULL;
    }
}

float CPhysicsPart::GetMaxDegradeDistance()
{
    if (m_DegradeInfo)
        return m_DegradeInfo->get_max_degrade_distance();

    return 100.0f;
}

BBox *CPhysicsPart::GetBoundingBox() const
{
    return &(m_GfxObjs[0]->m_BBox);
}

void CPhysicsPart::SetNoDraw(BOOL NoDraw)
{
    if (NoDraw)
        m_dw1C |= PHYSICSPART_NODRAW;
    else
        m_dw1C &= ~PHYSICSPART_NODRAW;
}

void CPhysicsPart::SetTranslucency(float Amount)
{
    if (!m_PhysicsObj || !(m_PhysicsObj->m_A8 & 0x100000))
    {
        if (FALSE /*RenderOptions::bUseMaterials */)
        {
            __asm int 3;
            // if (CopyMaterial())
            //    m_Material->SetTranslucencySimple(Amount);
        }
        else
        {
            if (Amount == 1.0f)
                m_dw1C |= PHYSICSPART_NODRAW;
            else
            {
                m_dw1C &= ~PHYSICSPART_NODRAW;

                if (m_GfxObjs[0]->m_Textures != m_Textures || CopySurfaces())
                {
                    for (DWORD i = 0; i < m_GfxObjs[0]->m_TextureCount; i++)
                        m_Textures[i]->SetTranslucency(Amount);
                }
            }
        }
    }
}

// Probably inlined.
DWORD CPhysicsPart::GetObjectIID(void) const
{
    return(m_PhysicsObj ? m_PhysicsObj->m_IID : 0);
}

void CPhysicsPart::UpdateViewerDistance(void)
{
    Vector offset = Render::ViewerPos.get_offset(m_Pos30, m_GfxObjs[0]->m_AngleVec * m_Scale);
    
    // Cache off distance for later usage.
    m_ViewerDist = offset.magnitude();

    if (m_ViewerDist > F_EPSILON)
        m_ViewerAngle = offset.normalize();
    else
        m_ViewerAngle = Vector(0, 0, 1);

    if (m_DegradeInfo && (GetObjectIID() != player_iid))
    {
        m_DegradeInfo->get_degrade(m_ViewerDist / m_Scale.z, &m_dw14, &m_dw18);
    }
    else
    {
        m_dw14 = 0;
        m_dw18 = 1;
    }

    if (m_GfxObjs[m_dw14])
        calc_draw_frame();
}

void CPhysicsPart::UpdateViewerDistance(float Distance, const Vector& Angle)
{
    m_ViewerDist = Distance;
    m_ViewerAngle = Angle;

    if (m_DegradeInfo && (GetObjectIID() != player_iid)) {
        m_DegradeInfo->get_degrade(m_ViewerDist / m_Scale.z, &m_dw14, &m_dw18);
    }
    else {
        m_dw14 = 0;
        m_dw18 = 1;
    }

    if (m_GfxObjs[m_dw14])
        calc_draw_frame();
}

void CPhysicsPart::calc_draw_frame(void)
{
    m_Pos78 = m_Pos30;

    switch(m_dw18)
    {
    case 1:
        // Do nothing..
        break;
    case 2:
        m_Pos78.m_Frame.set_vector_heading(m_ViewerAngle);
        break;
    case 3:
        m_Pos78.m_Frame.rotate_around_axis_to_vector(0, m_ViewerAngle);
        break;
    case 4:
        m_Pos78.m_Frame.rotate_around_axis_to_vector(1, m_ViewerAngle);
        break;
    case 5:
        m_Pos78.m_Frame.rotate_around_axis_to_vector(2, m_ViewerAngle);
        break;
    }
}

void CPhysicsPart::Draw(BOOL bIsBuilding)
{
    if (m_dw1C & PHYSICSPART_NODRAW)
         return;

    DWORD activeindex = (m_DegradeInfo && (m_dw14 < m_DegradeInfo->m_DegradeCount)) ? m_dw14 : 0;

    if (!m_GfxObjs[activeindex])
        return;

    // UpdateViewerDistance();

    Render::SetMaterial(m_Material);
    Render::SetSurfaceArray(m_Textures);
    Render::SetObjectScale(&m_Scale);

    /*
    Omitted:
    if (!m_PhysicsObj || !m_dw08)
    {
        Render::check_curr_object = FALSE;
        
        if (creature_mode)
            Render::check_curr_object = TRUE;
    }
    else
        Render::check_curr_object = TRUE;
    */

    // ACRenderDevice::DrawMesh
    int iResult = Render::DrawMesh(m_GfxObjs[activeindex], &m_Pos78, bIsBuilding);

    if (iResult == 3)
    {
        if (m_ViewerDist < Render::m_CurrDepth)
            Render::m_CurrDepth = m_ViewerDist;
    }


    /*
    Omitted:
    switch (iResult)
    {
        // These assume 0x08 is m_Key
    case 3:
        selected_object_id = (m_PhysicsObj ? m_PhysicsObj->m_Key : 0);
        selected_object_index = m_PartIndex;

    case 2:            
        
        if (viewcone_check_object_id == (m_PhysicsObj ? m_PhysicsObj->m_Key : 0))
            selected_object_in_view = TRUE;
    }
    */
}









