
#pragma once

#include "ObjCache.h"
#include "MathLib.h"
#include "DLListBase.h"
#include "Frame.h"

class Position;
class CObjCell;
class CPhysicsPart;
class CPhysicsObj;
class CSetup;
class CSingleMush;
class AnimSequenceNode;
class AnimFrame;
class AnimData;
class Palette;
class MotionTableManager;
class LIGHTLIST;

class CSequence : public PackObj
{
public:
    CSequence();
    ~CSequence();

    void        clear();
    void        clear_animations();
    void        clear_physics();

    void        set_object(CPhysicsObj *pPhysicsObj);
    void        set_placement_frame(AnimFrame *PlacementFrame, DWORD ID);
    void        set_velocity(const Vector& Velocity);
    void        set_omega(const Vector& Omega);

    void        combine_physics(const Vector& Velocity, const Vector& Omega);
    void        subtract_physics(const Vector& Velocity, const Vector& Omega);

    AnimFrame*    get_curr_animframe();
    long        get_curr_frame_number();

    void        multiply_cyclic_animation_framerate(float fRate);

    void        remove_cyclic_anims();

    void        append_animation(AnimData *pAnimData);
    void        apply_physics(Frame* pframe, double arg4, double argC);
    void        update(double time_elapsed, Frame* pframe);
    void        update_internal(double time_elapsed, AnimSequenceNode** ppanim, double *pframenum, Frame *pframe); 

    void        apricot();
    void        execute_hooks(AnimFrame *pAnimFrame, long lDir);
    void        advance_to_next_animation(double TimeElapsed, AnimSequenceNode** ppAnim, double *pFrameNum, Frame *pFrame);

    DLListBase m_Animations; // 0x04
    AnimSequenceNode *m_pNodes; // 0x0C
    Vector m_Velocity; // 0x10
    Vector m_Omega; // 0x1C
    CPhysicsObj *m_pPhysicsObj; // 0x28
    DWORD m_dw2C;
    double m_CurrentFrameNum; // 0x30
    AnimSequenceNode *m_CurrentAnim; // 0x38
    AnimFrame *m_PlacementFrame; // 0x3C
    DWORD m_PlacementID; // 0x40
    DWORD m_dw44;
};

class CPartArray
{
public:

    CPartArray();
    ~CPartArray();

    static CPartArray *CreateMesh(CPhysicsObj *pPhysicsObj, DWORD ID);
    static CPartArray *CreateSetup(CPhysicsObj *pPhysicsObj, DWORD ID, BOOL);
    static CPartArray *CreateParticle(CPhysicsObj *pPhysicsObj, DWORD ObjCount, Vector *Unused);

    void Destroy();
    void DestroySetup();
    void DestroyParts();
    void DestroyPals();
    void DestroyLights();
    BOOL InitParts();
    BOOL InitLights();
    void InitDefaults();

    void AddLightsToCell(CObjCell *pCell);
    BOOL CacheHasPhysicsBSP();
    void RemoveLightsFromCell(CObjCell *pCell);
    void SetCellID(DWORD ID);
    BOOL SetMeshID(DWORD ID);
    BOOL SetSetupID(DWORD ID, BOOL Unknown);
    BOOL SetPlacementFrame(DWORD ID);
    void SetFrame(Frame *pFrame);
    void SetNoDrawInternal(BOOL NoDraw);
    void SetTranslucencyInternal(float Amount);
    void UpdateParts(Frame *pFrame);
    void Update(float fTimeElapsed, Frame *pFrame);
    void UpdateViewerDistance(void);
    void UpdateViewerDistance(float Distance, const Vector& Angle);

    BOOL AllowsFreeHeading();
    DWORD GetDataID();
    float GetHeight();
    float GetRadius();

    void Draw(Position *Pos);

    DWORD                m_dw00;
    CPhysicsObj*        m_pPhysicsObj; // 0x04
    CSequence            m_Sequence; // 0x08 -- size 0x48
    MotionTableManager*    m_pMTManager; // 0x50
    CSetup*                m_pPartSetup; // 0x54
    DWORD                m_iPartCount; // 0x58
    CPhysicsPart**        m_pPartObjs; // 0x5C
    Vector                m_Scale; // 0x60
    CSingleMush*        m_pMush; // 0x6C
    Palette**            m_pPalettes; // 0x70
    LIGHTLIST*            m_pLightList; // 0x74
    DWORD                m_dw78;
};

class CPartGroup : public CPartArray 
{
    // Finish me
};

// Light information is obtained from CSetup
class LightInfo;

class LIGHTOBJ // size 0x48
{
public:
    LIGHTOBJ();
    ~LIGHTOBJ();

    LightInfo*    m_LightInfo;        // 0x00
    Frame        m_Frame;            // 0x04
    DWORD        m_Flags;            // 0x44
};

class LIGHTLIST // size 0x8
{
public:
    LIGHTLIST(DWORD LightCount);
    ~LIGHTLIST();

    void set_frame(Frame *pFrame);

    DWORD        m_LightCount;
    LIGHTOBJ*    m_Lights;
};




