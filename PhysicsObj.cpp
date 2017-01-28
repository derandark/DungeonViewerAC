
#include "StdAfx.h"
#include "Physics.h"
#include "PhysicsObj.h"
#include "PartArray.h"
#include "PhysicsPart.h"
#include "Particles.h"
#include "Animation.h"
#include "Scripts.h"
#include "Setup.h"
#include "ObjCell.h"
#include "Movement.h"
#include "LandDefs.h"
#include "Render.h"

#pragma warning(disable : 4150)

double CPhysicsObj::max_update_fps = 30.0;
double CPhysicsObj::min_update_delay = 1.0 / CPhysicsObj::max_update_fps;

CPhysicsObj::CPhysicsObj()
{
    m_04 = 0;
    m_IID = 0;
    m_0C = 0;
    m_PartArray = NULL;
    m_PartGroup = NULL;
    m_18 = 0;
    m_1C = 0;
    m_20 = 1.0f;
    m_SoundTable = NULL;
    m_ScriptManager = NULL;
    m_PhysicsScriptTable = NULL;
    m_38 = 0;
    m_3C = 0;
    m_Parent = NULL;
    m_Children = NULL;
    m_24 = FLT_MAX;
    m_ViewerDist = FLT_MAX;

    m_ObjCell = NULL;
    m_94 = 0;

    // m_ShadowObjs = new CShadowObj[ 100 ];
    m_ShadowObjs = NULL;
    // DEBUGOUT("Unfinished code in construction of Physics Object\r\n");

    m_A0 = 0;
    m_A4 = 4;
    m_9C = 4;

    m_fElasticity = DEFAULT_ELASTICITY;
    m_AC = 1; // dungeonviewer had as 0;
    m_fTranslucency = DEFAULT_TRANSLUCENCY;
    m_fFriction = DEFAULT_FRICTION;

    m_MovementManager = NULL;
    m_PositionManager = NULL;
    m_CC = 0;
    m_D0 = 0;
    
    m_A8 = 0x400808; // dungeonviewer had it as 0x400C08;
    m_fMinTranslucency = 0.0f;

    m_fMass = 1.0f / DEFAULT_MASS;

    m_E0 = 0;
    m_E4 = 0;
    m_E8 = 0;
    m_Acceleration = Vector(0, 0, 0);
    m_F8 = Vector(0, 0, 0);
    m_104 = NULL;
    m_108 = 0;
    m_10C = 0;
    m_110 = 0;
    m_118 = 0;
    m_DetectionManager = NULL;
    m_AttackManager = NULL;
    m_TargetManager = NULL;
    m_ParticleManager = NULL;
    m_WeenieObject = NULL;
    m_140 = 0;

    m_114 = 1.0f;

    m_144 = 0;
    m_148 = 0;
    m_14C = 0;
    m_150 = 0;
    m_154 = 0;
    m_158 = 0;
    m_15C = 0;
    m_160 = 0;
    m_164 = 0;
    m_168 = 0;
    m_16C = 0;
    m_170 = 0;
    m_174 = 0;
}

CPhysicsObj::~CPhysicsObj()
{
    Destroy();

    // delete [] m_108;

    if (m_ShadowObjs)
    {
        UNFINISHED("CPhysicsObj::~CPhysicsObj - delete [] m_ShadowObjs");
        delete [] m_ShadowObjs;
    }
    m_ShadowObjs = NULL;
}

void CPhysicsObj::Destroy()
{
    if (m_MovementManager)
    {
        delete m_MovementManager;
        m_MovementManager = NULL;
    }

    if (m_PositionManager)
    {
        delete m_PositionManager;
        m_PositionManager = NULL;
    }

    if (m_ParticleManager)
    {
        delete m_ParticleManager;
        m_ParticleManager = NULL;
    }

    if (m_ScriptManager)
    {
        delete m_ScriptManager;
        m_ScriptManager = NULL;
    }

    // FINISH CODE HERE
    // Destroy physicshooks..
    m_104 = NULL; 

    if ((m_A8 & 1) && (m_A8 & 0xC0000))
        CPhysics::RemoveStaticAnimatingObject(this);

    if (m_PhysicsScriptTable)
    {
        PhysicsScriptTable::Release(m_PhysicsScriptTable);
        m_PhysicsScriptTable = NULL;
    }

    // FINISH CODE HERE TOO

    if (m_SoundTable)
    {
        UNFINISHED("CPhysicsObj::Destroy - CSoundTable::Release(m_SoundTable)");
        // CSoundTable::Release(m_SoundTable);
        m_SoundTable = NULL;
    }

    // FINISH CODE HERE TOO

    if (m_PartArray)
    {
        delete m_PartArray;
        m_PartArray = NULL;
    }

    if (m_PartGroup)
    {
        delete m_PartGroup;
        m_PartGroup = NULL;
    }

    m_164 = 0;
    m_168 = 0;
    m_16C = 0;
    m_170 = 0;
    m_174 = 0;

    m_WeenieObject = NULL;

    if (m_15C)
    {
        UNFINISHED("CPhysicsObj::Destroy - delete m_15C");
        // delete m_15C;
        m_15C = NULL;
    }

    if (m_DetectionManager)
    {
        delete m_DetectionManager;
        m_DetectionManager = NULL;
    }

    if (m_AttackManager)
    {
        delete m_AttackManager;
        m_AttackManager = NULL;
    }

    if (m_TargetManager)
    {
        delete m_TargetManager;
        m_TargetManager = NULL;
    }

    if (m_Children)
    {
        delete m_Children;
        m_Children = NULL;
    }

    m_AC = 0;
    m_A8 = 0x400C08;
}

void CPhysicsObj::InitDefaults(CSetup *pSetup)
{
    if (pSetup->m_DefaultScript)
    {
        // DEBUGOUT("Playing default script %08X\r\n", pSetup->m_DefaultScript);
         play_script_internal(pSetup->m_DefaultScript);
    }
    
    if (pSetup->m_DefaultMotionTable)
        SetMotionTableID(pSetup->m_DefaultMotionTable);

    if (pSetup->m_DefaultSoundTable)
        set_stable_id(pSetup->m_DefaultSoundTable);

    if (pSetup->m_DefaultScriptTable)
        set_phstable_id(pSetup->m_DefaultScriptTable);

    if (m_A8 & 1)
    {
        if (pSetup->m_DefaultAnim)
            m_A8 |= 0x40000;

        if (pSetup->m_DefaultScript)
            m_A8 |= 0x80000;

        if (m_A8 & 0xC0000)
            CPhysics::AddStaticAnimatingObject(this);
    }
}

BOOL CPhysicsObj::InitObjectBegin(DWORD IID, BOOL bUnknown)
{
    m_IID = IID;

    if (bUnknown)
        m_A8 &= ~(0x00000001UL);
    else
        m_A8 |=  (0x00000001UL);

    m_AC &= ~(0x00000080UL);
    m_D8 = Time::GetTimeElapsed();

    return TRUE;
}

BOOL CPhysicsObj::InitPartArrayObject(DWORD ModelID, BOOL bUnknown)
{
    if (!ModelID)
        return FALSE;

    DWORD ModelType = ModelID & 0xFF000000;
    BOOL MakeMeTranslucent = FALSE;

    if (ModelType == 0x01000000)
    {
        m_PartArray = CPartArray::CreateMesh(this, ModelID);

        if (!m_PartArray)
            return FALSE;

    }
    else if (ModelType == 0x02000000)
    {
        m_PartArray = CPartArray::CreateSetup(this, ModelID, bUnknown);

        if (!m_PartArray)
            return FALSE;
    }
    else
    {
        if (ModelType)
            return FALSE;

        MakeMeTranslucent = TRUE;

        if (!makeAnimObject(0x02000000 | ModelID, bUnknown))
            return FALSE;
    }

    CacheHasPhysicsBSP();

    if (MakeMeTranslucent)
    {
        m_A8 |= 0x00000004UL;
        m_AC &= ~(0x00000100UL);

        SetTranslucencyInternal(0.25f);
        m_A8 |= 0x00000010UL;
    }

    return TRUE;
}

BOOL CPhysicsObj::CacheHasPhysicsBSP()
{
    if (m_PartArray)
    {
        if (m_PartArray->CacheHasPhysicsBSP())
        {
            m_A8 |= (0x00010000UL);
            return TRUE;
        }
    }
    else
    if (m_PartGroup)
    {
        if (m_PartGroup->CacheHasPhysicsBSP())
        {
            m_A8 |= (0x00010000UL);
            return TRUE;
        }
    }

    m_A8 &= ~(0x00010000UL);
    return FALSE;
}

BOOL CPhysicsObj::makeAnimObject(DWORD ModelID, BOOL bUnknown)
{
    m_PartArray = CPartArray::CreateSetup(this, ModelID, bUnknown);

    return(m_PartArray ? TRUE : FALSE);
}

void CPhysicsObj::SetTranslucencyInternal(float Amount)
{
    if (Amount < m_fMinTranslucency)
        Amount = m_fMinTranslucency;
    
    m_fTranslucency = Amount;

    if (m_PartArray)
        m_PartArray->SetTranslucencyInternal(Amount);
    else
    if (m_PartGroup)
        m_PartGroup->SetTranslucencyInternal(Amount);
}

BOOL CPhysicsObj::SetPlacementFrameInternal(DWORD PlacementID)
{
    BOOL R = FALSE;

    if (m_PartArray)
        R = m_PartArray->SetPlacementFrame(PlacementID);
    else
    if (m_PartGroup)
        R = m_PartGroup->SetPlacementFrame(PlacementID);

    if (!(m_A8 & 0x1000))
    {
        if (m_PartArray)
            m_PartArray->SetFrame(&m_Position.m_Frame);
        else
        if (m_PartGroup)
            m_PartGroup->SetFrame(&m_Position.m_Frame);
    }

    return R;
}

CPhysicsObj *CPhysicsObj::makeObject(DWORD ModelID, DWORD GUID, BOOL bUnknown)
{
    CPhysicsObj *pObject = new CPhysicsObj();

    if (!pObject)
        return NULL;

    if (!pObject->InitObjectBegin(GUID, bUnknown))
        goto bad_object;

    if (!pObject->InitPartArrayObject(ModelID, TRUE))
        goto bad_object;

    pObject->SetPlacementFrameInternal(0x65);

    return pObject;

bad_object:

    delete pObject;
    return NULL;
}

CPhysicsObj *CPhysicsObj::makeParticleObject(DWORD ObjCount, Vector *Unused)
{
    CPhysicsObj *pObject = new CPhysicsObj();

    if (!pObject)
        return NULL;

    pObject->m_A8 |= 0x1001;
    pObject->m_IID = 0;

    pObject->m_PartArray = CPartArray::CreateParticle(pObject, ObjCount, Unused);

    if (!pObject->m_PartArray)
    {
        delete pObject;
        return NULL;
    }

    return pObject;
}

void CPhysicsObj::animate_static_object()
{
    if (!m_ObjCell)
        return;

    double CurrentTime = Time::GetTimeElapsed(); // Timer__m_timeCurrent

    // Update physics timer.
    PhysicsTimer::curr_time = CurrentTime;

    // Time between last update.
    double FrameTime = CurrentTime - m_D8;

    if (FrameTime <= F_EPSILON)
    {
        m_D8 = CurrentTime;
    }
    else
    if (FrameTime < min_update_delay)
    {
        // Wait to update.. not quite there yet
    }
    else
    if (FrameTime > HUGE_QUANTUM)
    {
        m_D8 = CurrentTime;
    }
    else
    {
        if (m_PartArray)
        {
            if (m_A8 & 0x40000)
            {
                m_PartArray->Update(FrameTime, NULL);
                m_Position.m_Frame.grotate(m_F8);
                UpdatePartsInternal();
                UpdateChildrenInternal();
            }

            if (m_A8 & 0x80000)
            {
                if (m_ScriptManager)
                    m_ScriptManager->UpdateScripts();
            }

            if (m_ParticleManager)
                m_ParticleManager->UpdateParticles();

            process_hooks();
        }

        /*
        Frame NewPosition;
        UpdatePositionInternal(FrameTime, &NewPosition);

        set_initial_frame(&NewPosition);

        if (m_ParticleManager)
            m_ParticleManager->UpdateParticles();

        if (m_ScriptManager)
            m_ScriptManager->UpdateScripts();
            */

        m_D8 = CurrentTime;
    }
}

void CPhysicsObj::update_position()
{
    if (m_Parent)
        return;

    double CurrentTime = Time::GetTimeElapsed(); // Timer__m_timeCurrent

    // Update physics timer.
    PhysicsTimer::curr_time = CurrentTime;

    // Time between last update.
    double FrameTime = CurrentTime - m_D8;

    if (FrameTime < F_EPSILON)
    {
        m_D8 = CurrentTime;
    }
    else
    if (FrameTime < min_update_delay)
    {
        // Wait to update.. not quite there yet
    }
    else
    if (FrameTime > HUGE_QUANTUM)
    {
        m_D8 = CurrentTime;
    }
    else
    {
        Frame NewPosition;
        UpdatePositionInternal(FrameTime, &NewPosition);

        set_initial_frame(&NewPosition);

        if (m_ParticleManager)
            m_ParticleManager->UpdateParticles();

        if (m_ScriptManager)
            m_ScriptManager->UpdateScripts();

        m_D8 = CurrentTime;
    }
}

void CPhysicsObj::set_initial_frame(Frame *Pos)
{
    m_Position.m_Frame = *Pos;

    if (!(m_A8 & 0x1000))
    {
        if (m_PartArray)
            m_PartArray->SetFrame(&m_Position.m_Frame);
        else
        if (m_PartGroup)
            m_PartGroup->SetFrame(&m_Position.m_Frame);
    }

    UpdateChildrenInternal();
}

void CPhysicsObj::UpdateChild(CPhysicsObj *pChild, DWORD b, Frame *c)
{
    Frame LocalFrame;

    if (b < m_PartArray->m_iPartCount)
        LocalFrame.combine(&m_PartArray->m_pPartObjs[b]->m_Pos30.m_Frame, c);
    else
        LocalFrame.combine(&m_Position.m_Frame, c);

    pChild->m_Position.m_Frame = LocalFrame;
    pChild->UpdatePartsInternal();
    pChild->UpdateChildrenInternal();

    if (pChild->m_ParticleManager)
        pChild->m_ParticleManager->UpdateParticles();

    if (pChild->m_ScriptManager)
        pChild->m_ScriptManager->UpdateScripts();
}

void CPhysicsObj::UpdateChildrenInternal()
{
    if (m_PartArray && m_Children)
    {
        for (DWORD i = 0; i < m_Children->m_ChildCount; i++)
            UpdateChild(m_Children->m_Objects.array_data[i], m_Children->m_14.array_data[i], &m_Children->m_Frames.array_data[i]);
    }
}

void CPhysicsObj::UpdatePartsInternal()
{
    if (!(m_A8 & 0x1000))
    {
        if (m_PartArray)
            m_PartArray->SetFrame(&m_Position.m_Frame);
        else
        if (m_PartGroup)
            m_PartGroup->SetFrame(&m_Position.m_Frame);
    }
}

void CPhysicsObj::UpdatePositionInternal(float FrameTime, Frame *NewPosition)
{
    Frame TempFrame;

    if (!(m_A8 & 0x4000))
    {
        if (m_PartArray)
            m_PartArray->Update(FrameTime, &TempFrame);
        else
        if (m_PartGroup)
            m_PartGroup->Update(FrameTime, &TempFrame);

        if (m_AC & 2)
            TempFrame.m_origin *= m_114;
        else
            TempFrame.m_origin *= 0.0f;
    }

    if (m_PositionManager)
    {
        UNFINISHED("m_PositionManager->adjust_offset call");
        // m_PositionManager->adjust_offset(&TempFrame, (double)FrameTime);
    }

    NewPosition->combine(&m_Position.m_Frame, &TempFrame);

    if (!(m_A8 & 0x4000))
    {
        UpdatePhysicsInternal(FrameTime, NewPosition);
    }

    process_hooks();
}

void CPhysicsObj::UpdatePhysicsInternal(float FrameTime, Frame *NewPosition)
{
    // UNFINISHED
}

void CPhysicsObj::UpdateViewerDistance(void)
{
    // const float particle_distance_2dsq = FLT_MAX; // Render::*
    // const float object_distance_2dsq = FLT_MAX;    // Render::*

    // The distance at which we should be degraded.
    float degrade_dist = (m_A8 & 0x1000)? Render::particle_distance_2dsq : Render::object_distance_2dsq;

    // Our offset relative to the viewer.
    Vector offset = Render::ViewerPos.get_offset(m_Position);

    // 3D distance
    float distance3D = m_ViewerDist = offset.magnitude();

    // 2D distance (squared)
    float distance2Dsq = offset.x*offset.x + offset.y*offset.y;

    if (distance2Dsq < degrade_dist) // !! never degrade (substitutes for "if (distance2Dsq < degrade_dist)")
    {
        if (m_PartArray)
            m_PartArray->UpdateViewerDistance();
        else
        if (m_PartGroup)
            m_PartGroup->UpdateViewerDistance();
    }
    else
    {
        Vector ViewAngle;

        if (distance3D > F_EPSILON)
            ViewAngle = offset.normalize();
        else
            ViewAngle = Vector(0, 0, 1);

        if (m_PartArray)
            m_PartArray->UpdateViewerDistance(distance3D, ViewAngle);
        else
        if (m_PartGroup)
            m_PartGroup->UpdateViewerDistance(distance3D, ViewAngle);
    }
}

void CPhysicsObj::SetNoDraw(BOOL NoDraw)
{
    if (m_PartArray)
        m_PartArray->SetNoDrawInternal(NoDraw);
    else
    if (m_PartGroup)
        m_PartGroup->SetNoDrawInternal(NoDraw);
}

void CPhysicsObj::add_anim_hook(CAnimHook *pHook)
{
    // UNFINISHED
}

void CPhysicsObj::process_hooks()
{
    // UNFINISHED
}

void CPhysicsObj::create_particle_emitter(DWORD a, long b, Frame *c, DWORD d)
{
    if (!m_ParticleManager)
        m_ParticleManager = new ParticleManager;
    
    // 0x3200002F = FUBAR - FIX ME
    // 0x32000141 = steam over water (works fine?)
    // 0x320001AF = big torch fire (works fine?)
    // 0x32000227 = sprinkles over fountain

    m_ParticleManager->CreateParticleEmitter(this, a, b, c, d);
}

BOOL CPhysicsObj::play_script_internal(DWORD ScriptID)
{
    if (!ScriptID)
        return FALSE;

    if (!m_ScriptManager)
        m_ScriptManager = new ScriptManager(this);
    
    if (m_ScriptManager)
        return m_ScriptManager->AddScript(ScriptID);
    else
        return FALSE;
}

BOOL CPhysicsObj::play_script(DWORD Effect, float Mod)
{
    // Remove this check?
    if (m_ObjCell)
    {
        if (!m_PhysicsScriptTable)
            return FALSE;

        DWORD ScriptID = m_PhysicsScriptTable->GetScript(Effect, Mod);

        return play_script_internal(ScriptID);
    }

    return TRUE;
}

DWORD CPhysicsObj::SetMotionTableID(DWORD ID)
{
    // DEBUGOUT("Omitted motion table code here\r\n");

    if (m_PartArray)
    {
        //if (!m_PartArray->SetMotionTableID(ID))
        //     return FALSE;

        // MISSING CODE HERE
        return TRUE;
    }

    return FALSE;
}

void CPhysicsObj::set_stable_id(DWORD ID)
{
    // DEBUGOUT("Omitted sound table code here\r\n");
    // CSoundTable::Release(m_PhysicsScriptTable);
    // m_SoundTable = CSoundTable::Get(ID);
}

void CPhysicsObj::set_phstable_id(DWORD ID)
{
    PhysicsScriptTable::Release(m_PhysicsScriptTable);

    m_PhysicsScriptTable = PhysicsScriptTable::Get(ID);
}

void CPhysicsObj::set_nodraw(BOOL NoDraw, BOOL Unused)
{
    if (NoDraw)
    {
        m_A8 |= ~(0x00000020UL);

        if (m_PartArray)
            m_PartArray->SetNoDrawInternal(1);
        else
        if (m_PartGroup)
            m_PartGroup->SetNoDrawInternal(1);
    }
    else
    {
        m_A8 &= ~(0x00000020UL);

        if (m_PartArray)
            m_PartArray->SetNoDrawInternal(0);
        else
        if (m_PartGroup)
            m_PartGroup->SetNoDrawInternal(0);
    }
}

void CPhysicsObj::set_heading(float Heading, BOOL Unused)
{
    Frame NewFrame;
    
    NewFrame = m_Position.m_Frame;
    NewFrame.set_heading(Heading);

    set_initial_frame(&NewFrame);
}

void CPhysicsObj::DrawRecursive()
{
    if (m_PartArray)
        m_PartArray->Draw(&m_Position);
    else
    if (m_PartGroup)
        m_PartGroup->Draw(&m_Position);

    if (m_Children)
    {
        for (DWORD i = 0; i < m_Children->m_ChildCount; i++)
            m_Children->m_Objects.array_data[i]->DrawRecursive();
    }
}

void CPhysicsObj::SetTranslucencyHierarchical(float Amount)
{
    float MyAmount = Amount;

    if (MyAmount < m_fMinTranslucency)
        MyAmount = m_fMinTranslucency;

    if (m_PartArray)
        m_PartArray->SetTranslucencyInternal(MyAmount);
    else
    if (m_PartGroup)
        m_PartGroup->SetTranslucencyInternal(MyAmount);

    if (m_Children)
    {
        for (DWORD i = 0; i < m_Children->m_ChildCount; i++)
            m_Children->m_Objects.array_data[i]->SetTranslucencyHierarchical(Amount);
    }
}

void CPhysicsObj::set_sequence_animation(
    DWORD AnimationID, BOOL ClearAnimations, long StartFrame, float FrameRate)
{
    if (!m_PartArray)
        return;

    AnimData adata;

    adata.m_AnimID        = AnimationID;
    adata.m_LowFrame    = StartFrame;
    adata.m_PlaySpeed    = FrameRate;

    if (ClearAnimations)
        m_PartArray->m_Sequence.clear();

    m_PartArray->m_Sequence.append_animation(&adata); 
}

BOOL CPhysicsObj::set_parent(CPhysicsObj *Parent, DWORD b, Frame *c)
{
    if (!Parent)
        return FALSE;

    if (!Parent->add_child(this, b, c))
        return FALSE;

    unset_parent();
    leave_world();

    m_Parent = Parent;

    if (Parent->m_ObjCell)
    {
        change_cell(Parent->m_ObjCell);
        Parent->UpdateChild(this, b, c);
        recalc_cross_cells();
    }

    return TRUE;
}

void CPhysicsObj::change_cell(CObjCell *pObjCell)
{
    if (m_ObjCell)
        leave_cell(TRUE);
    
    if (pObjCell)
        enter_cell(pObjCell);
    else
    {
        set_cell_id(0);
        m_ObjCell = NULL;
    }
}

BOOL CPhysicsObj::is_completely_visible()
{
    if (!m_ObjCell)
        return FALSE;
    if (!m_94)
        return FALSE;

    for (DWORD i = 0; i < m_94; i++)
    {
        // m_ShadowObjs
    }

    return TRUE;
}

BOOL CPhysicsObj::is_valid_walkable(const Vector& point)
{
    if (point.z >= PhysicsGlobals::floor_z)
        return TRUE;

    return FALSE;
}

float CPhysicsObj::get_heading()
{
    return m_Position.m_Frame.get_heading();
}

BOOL CPhysicsObj::get_landscape_coord(long& X, long& Y)
{
    return LandDefs::gid_to_lcoord(m_Position.m_LandCell, X, Y);
}

float CPhysicsObj::get_walkable_z()
{
    return PhysicsGlobals::floor_z;
}

void CPhysicsObj::add_obj_to_cell(CObjCell *pCell, Frame *pFrame)
{
    enter_cell(pCell);

    set_initial_frame(pFrame);

    calc_cross_cells_static();
}

void CPhysicsObj::set_cell_id(DWORD CellID)
{
    m_Position.m_LandCell = CellID;

    if (!(m_A8 & 0x1000))
    {
        if (m_PartArray)
            m_PartArray->SetCellID(CellID);
        else
        if (m_PartGroup)
            m_PartGroup->SetCellID(CellID);
    }
}

void CPhysicsObj::leave_cell(BOOL Unknown)
{
    if (!m_ObjCell)
        return;

    m_ObjCell->remove_object(this);

    if (m_Children)
    {
        for (DWORD i = 0; i < m_Children->m_ChildCount; i++)
            m_Children->m_Objects.array_data[i]->leave_cell(Unknown);
    }

    if (m_PartArray)
        m_PartArray->RemoveLightsFromCell(m_ObjCell);
    else
    if (m_PartGroup)
        m_PartGroup->RemoveLightsFromCell(m_ObjCell);

    m_ObjCell = NULL;
}

void CPhysicsObj::enter_cell(CObjCell *pCell)
{
    if (!m_PartArray && !m_PartGroup)
        return;

    pCell->add_object(this);

    if (m_Children)
    {
        for (DWORD i = 0; i < m_Children->m_ChildCount; i++)
            m_Children->m_Objects.array_data[i]->enter_cell(pCell);
    }

    m_Position.m_LandCell = pCell->m_Key;
    set_cell_id(pCell->m_Key);

    m_ObjCell = pCell;

    if (m_PartArray)
        m_PartArray->AddLightsToCell(pCell);
    else
    if (m_PartGroup)
        m_PartGroup->AddLightsToCell(pCell);
}

void CPhysicsObj::calc_cross_cells_static()
{
    // UNFINISHED
    // ...
}

void CPhysicsObj::leave_world()
{
    report_collision_end(TRUE);

    /* Missing 
    if (obj_maint)
    {
        obj_maint->RemoveFromLostCell();
        obj_maint->RemoveObjectToBeDestroyed(m_IID);
    }
    */

    m_AC &= ~((DWORD)0x80);
    remove_shadows_from_cells();
    leave_cell(FALSE);
    set_cell_id(0);

    m_AC &= ~((DWORD)0x1);
    calc_acceleration();

    m_AC &= ~((DWORD)0x8);

    BOOL IsSet = m_AC & 2;
    m_AC &= ~((DWORD)0x2);

    if (IsSet && m_MovementManager)
    {
        // m_MovementManager->LeaveGround();
    }

    calc_acceleration();

    m_AC &= ~((DWORD)0x1F4);
}

void CPhysicsObj::calc_acceleration()
{
    if (!(m_AC & 1) && !(m_AC & 2) && !(m_A8 & 0x800000))
    {
        m_Acceleration = Vector(0, 0, 0);
        m_F8 = Vector(0, 0, 0);
    }
    else
    if (m_A8 & 0x400)
    {
        m_Acceleration = Vector(0, 0, PhysicsGlobals::gravity);
    }
    else
    {
        m_Acceleration = Vector(0, 0, 0);
    }
}

void CPhysicsObj::remove_shadows_from_cells()
{
    // UNFINISHED
    // ...
}

void CPhysicsObj::report_collision_end(BOOL Unknown)
{
    // UNFINISHED
    // ...
}

void CPhysicsObj::clear_transient_states()
{
    // UNFINISHED
    // ...
}

void CPhysicsObj::recalc_cross_cells()
{
    // UNFINISHED
    // ...
}

BOOL CPhysicsObj::add_child(CPhysicsObj *Child, DWORD b, Frame *c)
{
    if (Child == this)
        return FALSE;

    if ((b != (DWORD)-1) && (b >= m_PartArray->m_iPartCount))
        return FALSE;


    if (!m_Children)
    {
        m_Children = new CHILDLIST();
    }

    m_Children->add_child(Child, c, b, 0); 

    return TRUE;
}

void CPhysicsObj::unset_parent()
{
    if (!m_Parent)
        return;

    if (m_Parent->m_Children)
        m_Parent->m_Children->remove_child(this);

    if (m_Parent->m_A8 & 0x4000)
    {
        m_A8 &= ~(0x00000020UL);

        if (m_PartArray)
            m_PartArray->SetNoDrawInternal(0);
        else
        if (m_PartGroup)
            m_PartGroup->SetNoDrawInternal(0);
    }

    m_Parent = NULL;
    m_D8 = Time::GetTimeCurrent(); // Timer::m_timeCurrent

    clear_transient_states();
}

CHILDLIST::CHILDLIST()
{
    m_ChildCount = 0;
}

CHILDLIST::~CHILDLIST()
{
}

BOOL CHILDLIST::FindChildIndex(CPhysicsObj *pObj, WORD *Index)
{
    for (DWORD i = 0; i < m_ChildCount; i++)
    {
        if (m_Objects.array_data[i] == pObj)
        {
            *Index = i;
            return TRUE;
        }
    }

    return FALSE;
}

void CHILDLIST::add_child(CPhysicsObj *pChild, Frame *pFrame, DWORD unk1, DWORD unk2)
{
    if (m_ChildCount >= m_Objects.array_size)
    {
        DWORD new_size = m_Objects.array_size + 4;

        m_Objects.grow(new_size);
        m_Frames.grow(new_size);
        m_14.grow(new_size);
        m_1C.grow(new_size);
    }

    m_Objects.array_data[ m_ChildCount ] = pChild;
    m_Frames.array_data[ m_ChildCount ] = *pFrame;
    m_14.array_data[ m_ChildCount ] = unk1;
    m_1C.array_data[ m_ChildCount ] = unk2;

    m_ChildCount++;
}

void CHILDLIST::remove_child(CPhysicsObj *pChild)
{
    DWORD i;
    for (i = 0; i < m_ChildCount; i++)
    {
        if (m_Objects.array_data[i] == pChild)
            break;
    }

    m_ChildCount--;

    // Shift all children.
    for (; i < m_ChildCount; i++)
    {
        DWORD j = i + 1;

        m_Objects.array_data[i] = m_Objects.array_data[j];
        m_Frames.array_data[i] = m_Frames.array_data[j];
        m_14.array_data[i] = m_14.array_data[j];
        m_1C.array_data[i] = m_1C.array_data[j];
    }
}

void CPhysicsObj::AddPartToShadowCells(CPhysicsPart *pPart)
{
    if (m_ObjCell)
        pPart->m_Pos30.m_LandCell = m_ObjCell->m_Key;

    // Finish me
}

void CPhysicsObj::RemovePartFromShadowCells(CPhysicsPart *pPart)
{
    if (m_ObjCell)
        pPart->m_Pos30.m_LandCell = m_ObjCell->m_Key;

    // Finish me
}

void CPhysicsObj::interrupt_current_movement()
{
#ifndef I_DONT_WORK_YET_SO_DONT_COMPILE_ME
    if (m_MovementManager)
        m_MovementManager->CancelMoveTo(0x36);
#endif
}

void CPhysicsObj::unstick_from_object()
{
    // Finish me
}

void CPhysicsObj::stick_to_object(DWORD ObjectID)
{
    // Finish me
}

DWORD CPhysicsObj::GetDataID(void)
{
    if (m_PartArray)
        return m_PartArray->GetDataID();
    else
        return 0;
}





