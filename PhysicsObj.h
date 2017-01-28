
#pragma once

#include "Frame.h"
#include "SArray.h"

class CObjCell;
class CAnimHook;
class CShadowObj;
class CSetup;
class CPartArray;
class CPartGroup;
class CPhysicsPart;
class CSoundTable;
class CWeenieObject;
class PhysicsScriptTable;
class MovementManager;
class PositionManager;
class AttackManager;
class DetectionManager;
class TargetManager;
class ParticleManager;
class ScriptManager;

class CHILDLIST;

#define DEFAULT_FRICTION (0.95f)
#define DEFAULT_ELASTICITY (0.05f)
#define DEFAULT_TRANSLUCENCY (0.00f)
#define DEFAULT_MASS (1.00f)
#define DEFAULT_ALLOWED_SLIDE (0.05f)

#define INVALID_TIME ((double)-1.0)
#define HUGE_QUANTUM ((double)2.0)

// Physics Flags for m_A8 (define these later)
// HasPhysicsBSP: 0x00010000

class CPhysicsObj
{
public:
    CPhysicsObj();
    virtual ~CPhysicsObj();

    static CPhysicsObj *makeObject(DWORD ModelID, DWORD GUID, BOOL bUnknown);
    static CPhysicsObj *makeParticleObject(DWORD ObjCount, Vector *Unused);

    BOOL makeAnimObject(DWORD ModelID, BOOL bUnknown);

    void add_anim_hook(CAnimHook *pHook);
    BOOL add_child(CPhysicsObj *Child, DWORD b, Frame *c);
    void add_obj_to_cell(CObjCell *pCell, Frame *pFrame);
    void animate_static_object();
    void calc_acceleration();
    void calc_cross_cells_static();
    void change_cell(CObjCell *pObjCell);
    void clear_transient_states();
    void create_particle_emitter(DWORD, long, Frame *, DWORD);
    void enter_cell(CObjCell *pCell);
    float get_heading();
    BOOL get_landscape_coord(long& X, long& Y);
    float get_walkable_z();
    void interrupt_current_movement();
    BOOL is_completely_visible();
    BOOL is_valid_walkable(const Vector& point);
    void leave_cell(BOOL Unknown);
    void leave_world();
    BOOL play_script_internal(DWORD ScriptID);
    BOOL play_script(DWORD Effect, float Scale);
    void process_hooks();
    void recalc_cross_cells();
    void remove_shadows_from_cells();
    void report_collision_end(BOOL Unknown);
    void set_cell_id(DWORD CellID);
    void set_heading(float Heading, BOOL Unused);
    void set_initial_frame(Frame *Pos);
    void set_nodraw(BOOL NoDraw, BOOL Unused);
    BOOL set_parent(CPhysicsObj *pParent, DWORD b, Frame *c);
    void set_phstable_id(DWORD ID);
    void set_stable_id(DWORD ID);
    void set_sequence_animation(DWORD AnimationID, BOOL ClearAnimations, long StartFrame, float FrameRate);
    void stick_to_object(DWORD ObjectID);
    void unset_parent();
    void unstick_from_object();
    void update_position();

    // Public or Protected?
    void AddPartToShadowCells(CPhysicsPart *pPart);
    void DrawRecursive();
    DWORD GetDataID();
    void InitDefaults(CSetup *pSetup);
    void RemovePartFromShadowCells(CPhysicsPart *pPart);
    DWORD SetMotionTableID(DWORD ID);
    void SetNoDraw(BOOL NoDraw);
    void SetTranslucencyHierarchical(float Amount);
    void UpdateViewerDistance();

protected:
    BOOL CacheHasPhysicsBSP();

    void Destroy();

    BOOL InitObjectBegin(DWORD GUID, BOOL bUnknown);
    BOOL InitPartArrayObject(DWORD ModelID, BOOL bUnknown);

    void SetTranslucencyInternal(float Amount);
    BOOL SetPlacementFrameInternal(DWORD PlacementID);

    void UpdateChild(CPhysicsObj *pChild, DWORD b, Frame *c);
    void UpdateChildrenInternal();
    void UpdatePartsInternal();
    void UpdatePhysicsInternal(float FrameTime, Frame *NewPosition);
    void UpdatePositionInternal(float FrameTime, Frame *NewPosition);

public:
    DWORD m_04;
    DWORD m_IID; // 0x08
    DWORD m_0C;
    CPartArray *m_PartArray; // 0x10
    CPartGroup *m_PartGroup; // 0x14
    DWORD m_18;
    DWORD m_1C;
    float m_20;
    float m_24;
    float m_ViewerDist; // 0x28
    CSoundTable *m_SoundTable; // 0x2C
    ScriptManager *m_ScriptManager; // 0x30
    PhysicsScriptTable *m_PhysicsScriptTable; // 0x34
    DWORD m_38;
    DWORD m_3C;
    CPhysicsObj *m_Parent; // 0x40
    CHILDLIST *m_Children; // 0x44

    Position m_Position; // 0x48 (size 0x48)
    CObjCell *m_ObjCell; // 0x90
    DWORD m_94;
    CShadowObj *m_ShadowObjs; // 0x98 

    DWORD m_9C;
    DWORD m_A0; 
    DWORD m_A4;
    DWORD m_A8; // 0xA8
    DWORD m_AC;

    float m_fElasticity; // 0xB0
    float m_fTranslucency; // 0xB4
    float m_fMinTranslucency; // 0xB8
    float m_fFriction; // 0xBC
    float m_fMass; // 0xC0

    MovementManager *m_MovementManager; // 0xC4
    PositionManager *m_PositionManager; // 0xC8
    DWORD m_CC;
    DWORD m_D0;

    double m_D8;
    
    DWORD m_E0;
    DWORD m_E4;
    DWORD m_E8;
    Vector m_Acceleration; // 0xEC
    Vector m_F8;
    LPVOID m_104; // physicshooks..
    DWORD m_108;
    DWORD m_10C;
    DWORD m_110;
    float m_114;
    DWORD m_118;
    DetectionManager *m_DetectionManager; // 0x11C
    AttackManager *m_AttackManager; // 0x120
    TargetManager *m_TargetManager; // 0x124
    ParticleManager *m_ParticleManager; // 0x128
    CWeenieObject *m_WeenieObject; // 0x12C
    DWORD m_140;
    DWORD m_144;
    DWORD m_148;
    DWORD m_14C;
    DWORD m_150;
    DWORD m_154;
    DWORD m_158;
    DWORD m_15C;
    DWORD m_160;
    DWORD m_164;
    DWORD m_168;
    DWORD m_16C;
    DWORD m_170;

    WORD m_174;

    static double max_update_fps;
    static double min_update_delay;
};

class CHILDLIST
{
public:
    CHILDLIST();
    ~CHILDLIST();

    void add_child(CPhysicsObj *pObj, Frame *pFrame, DWORD unk1, DWORD unk2);
    void remove_child(CPhysicsObj *pChild);
    
    BOOL FindChildIndex(CPhysicsObj *pObj, WORD *Index);

    WORD m_ChildCount;
    SArray<CPhysicsObj *> m_Objects;
    SArray<Frame> m_Frames;
    SArray<DWORD> m_14;
    SArray<DWORD> m_1C;
};

