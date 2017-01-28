
#include "StdAfx.h"
#include "Movement.h"

#ifndef I_DONT_WORK_YET_SO_DONT_COMPILE_ME

MovementManager::MovementManager()
{
    m_MInterp = NULL;
    m_MoveToManager = NULL;
    m_PhysicsObj = NULL;
    m_WeenieObj = NULL;
}

MovementManager::~MovementManager()
{
    Destroy();
}

MovementManager* MovementManager::Create(CPhysicsObj *pPObject, CWeenieObject *pWObject)
{
    MovementManager *pManager;

    pManager->SetPhysicsObject(pPObject);
    pManager->SetWeenieObject(pWObject);

    return pManager;
}

void MovementManager::Destroy()
{
    if (m_MInterp)
        delete m_MInterp;

    m_MInterp = NULL;

    if (m_MoveToManager)
        delete m_MoveToManager;

    m_MoveToManager = NULL;
}

void MovementManager::MakeMoveToManager()
{
    if (!m_MoveToManager)
        m_MoveToManager = MoveToManager::Create(m_PhysicsObj, m_WeenieObj);
}

void MovementManager::EnterDefaultState()
{
    if (!m_PhysicsObj)
        return;

    get_minterp()->enter_default_state();
}

BOOL MovementManager::IsMovingTo()
{
    if (!m_MoveToManager)
        return FALSE;

    if (!m_MoveToManager->is_moving_to())
        return FALSE;

    return TRUE;
}

void MovementManager::CancelMoveTo(DWORD Unknown)
{
    if (m_MoveToManager)
        m_MoveToManager->CancelMoveTo(Unknown);
}

CMotionInterp *MovementManager::get_minterp()
{
    if (!m_MInterp)
    {
        m_MInterp = CMotionInterp::Create(m_PhysicsObj, m_WeenieObj);

        if (m_PhysicsObj)
            m_MInterp->enter_default_state();
    }

    return m_MInterp;
}

BOOL MovementManager::motions_pending()
{
    if (!m_MInterp)
        return FALSE;
    if (!m_MInterp->motions_pending())
        return FALSE;

    return TRUE;
}

void MovementManager::SetPhysicsObject(CPhysicsObj *pObject)
{
    m_PhysicsObj = pObject;

    if (m_MInterp)
        m_MInterp->SetPhysicsObject(pObject);

    if (m_MoveToManager)
        m_MoveToManager->SetPhysicsObject(pObject);
}

void MovementManager::SetWeenieObject(CWeenieObject *pObject)
{
    m_WeenieObj = pObject;

    if (m_MInterp)
        m_MInterp->SetWeenieObject(pObject);

    if (m_MoveToManager)
        m_MoveToManager->SetWeenieObject(pObject);
}

BOOL MovementManager::unpack_movement(BYTE **ppData, ULONG iSize)
{
    if (!m_MInterp || !m_PhysicsObj)
        return FALSE;

    m_PhysicsObj->interrupt_current_movement();
    m_PhysicsObj->unstick_from_object();

    Position                MovePos;
    MovementParameters        MoveParams;
    InterpretedMotionState    MotionState;

    DWORD ValOne;
    DWORD StateIndex;

    UNPACK(WORD, ValOne);
    UNPACK(WORD, StateIndex);

    DWORD MotionID = MotionIDs[ StateIndex ];

    if (MotionID != m_MInterp->GetMotionStateID())
        m_MInterp->DoMotion(MotionID, &MoveParams);

    switch (ValOne & 0xFF)
    {
    case 0:
        UNPACK_OBJ(MotionState);

        DWORD TargetObj;

        if (ValOne & 0x100)
            UNPACK(DWORD, TargetObj);
        else
            TargetObj = 0;

        move_to_interpreted_state(&MotionState);

        if (TargetObj)
            m_PhysicsObj->stick_to_object(TargetObj);

        m_MInterp->m_70 = ValOne + 0x200;
        return TRUE;
    case 6:
    case 7:
    case 8:
    case 9:
        // Not done.
        __asm int 3;
    default:
        break;
    }
}

CMotionInterp::CMotionInterp(CPhysicsObj *pPObject, CWeenieObject *pWObject)
{
    m_6C = 1.0;
    m_7C = 1.0;

    m_70 = 0;
    m_74 = 0;
    m_78 = 0;

    m_80 = 0;
    m_84 = 0;

    SetWeenieObject(pWObject);
    SetPhysicsObject(pPObject);
}

CMotionInterp::~CMotionInterp()
{
    Destroy();
}

CMotionInterp* CMotionInterp::Create(CPhysicsObj *pPObject, CWeenieObject *pWObject)
{
    CMotionInterp *pInterp = new CMotionInterp(pPObject, pWObject);

    if (!pInterp)
        return NULL;

    return pInterp;
}

DWORD CMotionInterp::DoMotion(DWORD MotionID, const MovementParameters& Params)
{
    if (!m_PhysicsObj)
        return 0x08;

    MoveParameters    CurrentParams = Params;
    DWORD            CurrentMotion = MotionID;

    if (Params.m_04 & 0x8000)
        m_PhysicsObj->interrupt_current_movement();

    if (Params.m_04 & 0x800)
        SetHoldKey(Params.m_24, FBitSet(Params.m_04, 15));

    adjust_motion(&CurrentMotion, &CurrentParams.m_14, Params.m_24);

    if (m_InterpState.m_04 != 0x8000003D)
    {
        switch (MotionID)
        {
        case 0x41000012:
            return 0x3F;
        case 0x41000013:
            return 0x40;
        case 0x41000014:
            return 0x41;
        }

        if (MotionID & 0x2000000)
            return 0x42;
    }

    if (MotionID & 0x10000000)
    {
        if (m_InterpState.GetNumActions >= 6)
            return 0x45;
    }

    DWORD NewMotion = DoInterpretedMotion(CurrentMotion, CurrentParams);

    if (!NewMotion && (Params.m_04 & 0x2000)) 
        m_RawState.ApplyMotion(MotionID, Params);

    return NewMotion;
}

void CMotionInterp::apply_current_movement(BOOL Unk1, DWORD Unk2)
{
    if (!m_PhysicsObj)
        return;
    if (m_00 == 0)
        return;

    // FINISH ME
    // if ((m_WeenieObj && !m_WeenieObj->virtual_0x14()) || (.....))
    if (!m_PhysicsObj->movement_is_autonomous())
        apply_interpreted_movement(Unk1, Unk2);
    else
        apply_raw_movement(Unk1, Unk2);
}

void CMotionInterp::adjust_motion(BOOL Unk1, DWORD Unk2)
{
    if (!m_PhysicsObj)
        return;
    if (m_00 == 0)
        return;

    // FINISH ME
    // if ((m_WeenieObj && !m_WeenieObj->virtual_0x14()) || (.....))
    if (m_WeenieObj)
        __asm int 3;

    if (!m_PhysicsObj->movement_is_autonomous())
        apply_interpreted_movement(Unk1, Unk2);
    else
        apply_raw_movement(Unk1, Unk2);
}

void CMotionInterp::enter_default_state(void)
{
    m_RawState        = RawMotionState(); 
    m_InterpState    = InterpretedMotionState();

    m_PhysicsObj->InitializeMotionTables();

    // Finish me.
    __asm int 3;
}

float CMotionInterp::get_max_speed(void)
{
    float speed;

    if (m_WeenieObj)
    {
        // Finish me.
        __asm int 3;
    }
    else
        speed = 1.0f;
    
    return(speed * run_anim_speed);
}

int CMotionInterp::jump(int arg_0, int& arg_4)
{
    if (!m_PhysicsObj)
        return 8;

    m_PhysicsObj->interrupt_current_movement();

    int allowed = jump_is_allowed(arg_0, arg_4);

    if (allowed)
        m_InterpState.m_2C = 0;
    else
    {
        m_InterpState.m_30 = arg_0;
        m_PhysicsObj->set_on_walkable(allowed);
    }

    return allowed;
}

void CMotionInterp::SetPhysicsObject(CPhysicsObj *pObject)
{
    m_PhysicsObj = pObject;

    apply_current_movement(TRUE, 0);
}

void CMotionInterp::SetWeenieObject(CWeenieObject *pObject)
{
    m_WeenieObj = pObject;

    apply_current_movement(TRUE, 0);
}

InterpretedMotionState::InterpretedMotionState()
{
    // Finish me.
    __asm int 3;
}

InterpretedMotionState::~InterpretedMotionState()
{
    Destroy();
}

void InterpretedMotionState::Destroy()
{
    // Finish me.
    __asm int 3;
}

RawMotionState::RawMotionState()
{
    m_bRunning = TRUE;
    m_10 = 0x8000003D;
    m_14 = 0x41000003;
    m_18 = 0;

    m_1C = 1.0;
    m_20 = 0;
    m_24 = 0;
    m_28 = 1.0;
    m_2C = 0;
    m_30 = 0;
    m_34 = 1.0;
}

RawMotionState::~RawMotionState()
{
    Destroy();
}

void RawMotionState::Destroy()
{
    if (!m_Actions.Empty())
        m_Actions.DestroyContents();
}

BOOL RawMotionState::UnPack(BYTE **ppData, ULONG iSize)
{
    Destroy();

    // The flags determine the pack fields.
    DWORD Flags;

    if (iSize >= sizeof(DWORD))
        UNPACK(DWORD, Flags);
    else
        Flags = iSize; // Huh?

    if (FBitSet(Flags, 0))
        UNPACK(DWORD, m_IsRunning);
    else
        m_IsRunning = TRUE;

    if (FBitSet(Flags, 1))
        UNPACK(DWORD, m_10);
    else
        m_10 = 0x8000003D;

    if (FBitSet(Flags, 2))
        UNPACK(DWORD, m_14);
    else
        m_14 = 0x41000003;

    if (FBitSet(Flags, 3))
        UNPACK(DWORD, m_18);
    else
        m_18 = 0;

    if (FBitSet(Flags, 4))
        UNPACK(float, m_1C);
    else
        m_1C = 1.0;

    if (FBitSet(Flags, 5))
        UNPACK(float, m_20);
    else
        m_20 = 0;

    if (FBitSet(Flags, 6))
        UNPACK(float, m_24);
    else
        m_24 = 0;

    if (FBitSet(Flags, 7))
        UNPACK(float, m_28);
    else
        m_28 = 1.0;

    if (FBitSet(Flags, 8))
        UNPACK(float, m_2C);
    else
        m_2C = 0;

    if (FBitSet(Flags, 9))
        UNPACK(float, m_30);
    else
        m_30 = 0;

    if (FBitSet(Flags, 10))
        UNPACK(float, m_34);
    else
        m_34 = 1.0;

    DWORD NumActions = (Flags >> 11) & 0x1F;

    for (DWORD i = 0; i < NumActions; i++)
    {
        MotionAction* pAction = new MotionAction;

        UNPACK_POBJ(pAction);
        m_Actions.InsertAtEnd(pAction);
    }

    return TRUE;
}

void RawMotionState::ApplyMotion(DWORD MotionID, MovementParameters *Params)
{
    // Finish me
    __asm int 3;
}

MotionAction::MotionAction()
{
    m_StateID        = 0;
    m_PlaySpeed        = 1.0;
    m_Order            = 0;
    m_OrderFlag        = FALSE;
}

ULONG MotionAction::pack_size() 
{
    return(sizeof(WORD) + sizeof(WORD) + sizeof(float));
}

BOOL MotionAction::UnPack(BYTE **ppData, ULONG iSize)
{
    if (iSize < pack_size())
        return false;

    DWORD StateIndex;
    DWORD Order;

    UNPACK(WORD, StateIndex);
    m_StateID = RawStateIDs[ StateIndex ];

    UNPACK(WORD, Order);
    UNPACK(float, m_PlaySpeed);

    m_Order        = Order & 0x7FFF;
    m_OrderFlag    = FBitSet(Order, 15);

    return TRUE;
}

MoveToManager::MoveToManager()
{
    m_14C = 0;
    m_150 = 0;
    m_154 = 0;
    m_158 = 0;

    InitializeLocalVariables();
}

MoveToManager::~MoveToManager()
{
    Destroy();
}

void MoveToManager::Destroy()
{
    // Finish me.
}

void MoveToManager::CleanUp()
{
    MovementParameters Params;

    Params.m_24 = m_MoveParams.m_24;
    Params.m_04 &= ~((DWORD)0x8000);

    if (m_PhysicsObj)
    {
        if (m_13C)
            _StopMotion(m_13C, &Params);
        
        if (m_140)
            _StopMotion(m_140, &Params);

        if (m_130 && m_00)
            m_PhysicsObj->clear_target();
    }

    InitializeLocalVariables();
}

MoveToManager* MoveToManager::Create(CPhysicsObj *pPhysicsObj, CWeenieObject *pWeenieObj)
{
    MoveToManager *pManager = new MoveToManager();

    if (!pManager)
        return NULL;

    pManager->SetPhysicsObject(pPhysicsObj);
    pManager->SetWeenieObject(pWeenieObj);

    return pManager;
}

void MoveToManager::SetPhysicsObject(CPhysicsObj *pPhysicsObj)
{
    m_PhysicsObj = pPhysicsObj;
}

void MoveToManager::SetWeenieObject(CWeenieObject *pWeenieObj)
{
    m_WeenieObj = pWeenieObj;
}

void MoveToManager::InitializeLocalVariables()
{
    m_00 = 0;

    m_MoveParams->m_04 = 0;
    m_MoveParams->m_20 = 0;
    m_MoveParams->m_30 = FLT_MAX;
    m_MoveParams->m_34 = Time::GetTimeCurrent(); // Timer::m_timeCurrent
    m_MoveParams->m_3C = FLT_MAX;
    m_MoveParams->m_44 = Time::GetTimeCurrent(); // Timer::m_timeCurrent
    m_MoveParams->m_2C = 0;
    m_MoveParams->m_4C = 0;
    m_MoveParams->m_60 = 0;
    m_MoveParams->m_64 = 0;
    m_MoveParams->m_68 = 0;
    m_MoveParams->m_6C = 0;

    m_Pos04 = Position();
    m_Pos4C = Position();

    m_MoveParams->m_50 = 0;
    m_MoveParams->m_54 = 0;
    m_MoveParams->m_58 = 0;
    m_MoveParams->m_5C = 0;
}

void MoveToManager::CancelMoveTo(DWORD Unknown)
{
    if (!m_00)
        return;

    if (!m_Nodes.Empty())
        m_Nodes.DestroyContents();

    CleanUpAndCallWeenie(Unknown);
}

void MoveToManager::CleanUpAndCallWeenie(DWORD Unknown)
{
    CleanUp();

    if (m_PhysicsObj)
        m_PhysicsObj->StopCompletely(0);
}

MovementParameters::MovementParameters()
{
    m_0C = 0;            // value given by a CONST?
    m_08 = 0.6;            // value given by a CONST?
    m_18 = FLT_MAX;        // value given by a CONST?
    m_10 = 0;
    m_14 = 1.0;
    m_1C = 150.0;        // value given by a CONST?
    m_20 = 0;
    m_24 = 0;
    m_28 = 0;
    
    // --MISSING CODE HERE--
    // SUBSTITUTED WITH THIS
    DEBUGOUT("Warning CODE @ line " __LINE__ " of " __FILE__ " is evil!\r\n");
    m_04 = 0x1EE0F;
}

ULONG MovementParameters::pack_size()
{
    return 0x28;
}

BOOL MovementParameters::UnPack(BYTE **ppData, ULONG iSize)
{
    if (iSize < pack_size())
        return FALSE;

    UNPACK(DWORD, m_04);
    UNPACK(float, m_08);
    UNPACK(DWORD, m_0C);
    UNPACK(float, m_18);
    UNPACK(DWORD, m_10);
    UNPACK(float, m_14);
    UNPACK(float, m_1C);
    UNPACK(DWORD, m_20);
    UNPACK(DWORD, m_24);
    UNPACK(DWORD, m_28);

    return TRUE;
}

BOOL MovementParameters::UnPackNet(DWORD Type, BYTE **ppData, ULONG iSize)
{
    ULONG PackSize;

    switch (Type)
    {
    case 6:
    case 7:
        PackSize = 0x1C;
        break;
    case 8:
    case 9:
        PackSize = 0x0C;
        break;
    default:
        return FALSE;
    }

    if (iSize < PackSize)
        return FALSE;

    switch (Type)
    {
    case 6:
    case 7:
        UNPACK(DWORD, m_04);
        UNPACK(float, m_08);
        UNPACK(DWORD, m_0C);
        UNPACK(float, m_18);
        UNPACK(float, m_14);
        UNPACK(float, m_1C);
        UNPACK(DWORD, m_10);
        break;
    case 8:
    case 9:
        UNPACK(DWORD, m_04);
        UNPACK(float, m_14);
        UNPACK(DWORD, m_10);
        break;
    default:
        return FALSE;
    }

    return TRUE;
}

#endif
