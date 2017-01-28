
#pragma once
#include "Frame.h"

class CPhysicsObj;
class CGfxObj;
class CSurface;
class CMaterial;
class BBox;
class GfxObjDegradeInfo;

#define PHYSICSPART_NODRAW (0x00000001UL)

class CPhysicsPart
{
public:
    CPhysicsPart();
    ~CPhysicsPart();

    // Static Functions.
    static CPhysicsPart *makePhysicsPart(DWORD ID);
    static BOOL LoadGfxObjArray(DWORD ID, GfxObjDegradeInfo*& DegradeInfo, CGfxObj**& Objects);
    static void ReleaseGfxObjArray(GfxObjDegradeInfo*& DegradeInfo, CGfxObj**& Objects);

    // Static Variables.
    static DWORD player_iid;

    // Member Functions
    BOOL CopySurfaces();
    void DetermineBasePal();
    void Draw(BOOL bIsBuilding);
    BBox *GetBoundingBox() const;
    float GetMaxDegradeDistance();
    DWORD GetObjectIID() const; // Probably inlined.
    void RestoreSurfaces();
    void SetGfxObjArray(GfxObjDegradeInfo* DegradeInfo, CGfxObj** Objects);
    void SetNoDraw(BOOL NoDraw);
    BOOL SetPart(DWORD ID);
    void SetTranslucency(float Amount);
    BOOL UsePalette(Palette *pPalette);
    void UpdateViewerDistance(void);
    void UpdateViewerDistance(float Distance, const Vector& Angle);

    void calc_draw_frame();

    // Member Variables
    float                m_ViewerDist;    // 0x00
    Vector                m_ViewerAngle;    // 0x04
    GfxObjDegradeInfo*    m_DegradeInfo;    // 0x10
    DWORD                m_dw14;            // 0x14
    DWORD                m_dw18;            // 0x18
    DWORD                m_dw1C;            // 0x1C

    CGfxObj**            m_GfxObjs;        // 0x20
    Vector                m_Scale;        // 0x24

    Position            m_Pos30;        // 0x30
    Position            m_Pos78;        // 0x78

    CMaterial*            m_Material;        // 0xC0
    CSurface**            m_Textures;        // 0xC4
    DWORD                m_BasePalID;    // 0xC8
    Palette*            m_BasePal;        // 0xCC
    CPhysicsObj*        m_PhysicsObj;    // 0xD0
    long                m_PartIndex;    // 0xD4

    // 0xDC is unknown
};




