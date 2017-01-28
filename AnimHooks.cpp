
#include "StdAfx.h"
#include "PhysicsObj.h"
#include "AnimHooks.h"

AnimFrame::AnimFrame()
{
    m_FrameCount = 0;
    m_pFrames = NULL;

    m_HookCount = 0;
    m_pHooks = NULL;
}

AnimFrame::~AnimFrame()
{
    Destroy();
}

void AnimFrame::Destroy()
{
    // Destroy Frames
    if (m_pFrames)
    {
        delete [] m_pFrames;
        m_pFrames = NULL;
    }

    // Destroy Anim Hooks
    while (m_pHooks)
    {
        CAnimHook *pNext = m_pHooks->m_pNext;
        delete m_pHooks;
        m_pHooks = pNext;
    }

    m_pHooks = NULL;
}

BOOL AnimFrame::UnPack(DWORD ObjCount, BYTE **ppData, ULONG iSize)
{
    Destroy();

    m_FrameCount = ObjCount;
    m_pFrames = new AFrame[ ObjCount ];

    for (DWORD i = 0; i < m_FrameCount; i++)
        UNPACK_OBJ(m_pFrames[i]);
    
    UNPACK(DWORD, m_HookCount);

    for (DWORD i = 0; i < m_HookCount; i++)
        CAnimHook::UnPackHook(ppData, iSize)->add_to_list(&m_pHooks);

    return TRUE;
}

AnimPartChange::AnimPartChange()
{
    m_PartIndex = 0;
    m_GfxObjID = 0;
    m_dw0C = 0;
    m_dw10 = 0;
}

BOOL AnimPartChange::UnPack(BYTE **ppData, ULONG iSize)
{
    // Client does this in a redundant way, but this will achieve the same result.
    if (iSize < 4)
        return FALSE;

    UNPACK(BYTE, m_PartIndex);
    UNPACK(WORD, m_GfxObjID);

    return TRUE;
}

BOOL AnimPartChange::replaces(AnimPartChange *pChange)
{
    // Pretty sure this is right.
    if (m_PartIndex == pChange->m_PartIndex)
        return TRUE;

    return FALSE;
}

CAnimHook::CAnimHook()
{
    m_pNext = NULL;
    m_Key    = -2;
}

CAnimHook::~CAnimHook()
{
}

CAnimHook* CAnimHook::UnPackHook(BYTE** ppData, ULONG iSize)
{
    DWORD hook_type, hook_key;

    UNPACK(DWORD, hook_type);
    UNPACK(DWORD, hook_key);

    CAnimHook* pHook = NULL;

    // DEBUGOUT("Unpacking hook %u\r\n", hook_type);

    switch (hook_type)
    {
    case 0:
        pHook = new NOOPHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 1:
        pHook = new SoundHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 2:
        pHook = new SoundTableHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 3:
        pHook = new AttackHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 5:
        pHook = new ReplaceObjectHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 6:
        pHook = new EtherealHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 7:
        pHook = new TransparentPartHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 8:
        pHook = new LuminousHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 9:
        pHook = new LuminousPartHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 10:
        pHook = new DiffuseHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 11:
        pHook = new DiffusePartHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 12:
        pHook = new ScaleHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 13:
        pHook = new CreateParticleHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 14:
        pHook = new DestroyParticleHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 15:
        pHook = new StopParticleHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 16:
        pHook = new NoDrawHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 17:
        pHook = new DefaultScriptHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 18:
        pHook = new DefaultScriptPartHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 19:
        pHook = new CallPESHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 20:
        pHook = new TransparentHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 21:
        pHook = new SoundTweakedHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 22:
        pHook = new SetOmegaHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 23:
        pHook = new TextureVelocityHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 24:
        pHook = new TextureVelocityPartHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 25:
        pHook = new SetLightHook();
        pHook->UnPack(ppData, iSize);
        break;
    case 26:
        pHook = new CreateBlockingParticleHook();
        pHook->UnPack(ppData, iSize);
        break;
    default:
        goto UnknownHook;
    }

    pHook->m_Key = hook_key;

UnknownHook:

#ifdef PRE_TOD
    PACK_ALIGN();
#endif

    return pHook;
}

void CAnimHook::GetDataRecursion(LPVOID lpVoid)
{
}

void CAnimHook::add_to_list(CAnimHook** pList)
{
    CAnimHook *pNode = *pList;

    if (!pNode)
    {
        // We are the first element.
        *pList = this;
        return;
    }

    // Iterate through all the nodes.
    while (pNode->m_pNext)
        pNode = pNode->m_pNext;

    // Append on last element.
    pNode->m_pNext = this;
}

//=============================================================================
// NOOPHook
//=============================================================================

NOOPHook::NOOPHook()
{
}

void NOOPHook::Execute(CPhysicsObj *pOwner)
{
}

int NOOPHook::GetType()
{
    return 0;
}

ULONG NOOPHook::pack_size()
{
    return 0;
}

ULONG NOOPHook::Pack(BYTE** ppData, ULONG iSize)
{
    return 0;
}

BOOL NOOPHook::UnPack(BYTE** ppData, ULONG iSize)
{
    return TRUE;
}

//=============================================================================
// NoDrawHook
//=============================================================================

NoDrawHook::NoDrawHook()
{
}

void NoDrawHook::Execute(CPhysicsObj *pOwner)
{
    pOwner->set_nodraw(m_bDraw, 0);
}

int NoDrawHook::GetType()
{
    return 16;
}

ULONG NoDrawHook::pack_size()
{
    return sizeof(BOOL);
}

ULONG NoDrawHook::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    if (iSize >= PackSize)
    {
        PACK(BOOL, m_bDraw);
    }

    return PackSize;
}

BOOL NoDrawHook::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(BOOL, m_bDraw);
    return TRUE;
}

//=============================================================================
// DefaultScriptHook
//=============================================================================

DefaultScriptHook::DefaultScriptHook()
{
}

void DefaultScriptHook::Execute(CPhysicsObj *pOwner)
{
    UNFINISHED_WARNING("DefaultScriptHook::Execute");
}

int DefaultScriptHook::GetType()
{
    return 17;
}

ULONG DefaultScriptHook::pack_size()
{
    return 0;
}

ULONG DefaultScriptHook::Pack(BYTE** ppData, ULONG iSize)
{
    return 0;
}

BOOL DefaultScriptHook::UnPack(BYTE** ppData, ULONG iSize)
{
    return TRUE;
}

//=============================================================================
// DefaultScriptPartHook
//=============================================================================

DefaultScriptPartHook::DefaultScriptPartHook()
{
}

void DefaultScriptPartHook::Execute(CPhysicsObj *pOwner)
{
    UNFINISHED_WARNING("DefaultScriptPartHook::Execute");
}

int DefaultScriptPartHook::GetType()
{
    return 18;
}

ULONG DefaultScriptPartHook::pack_size()
{
    return sizeof(DWORD);
}

ULONG DefaultScriptPartHook::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    if (iSize >= PackSize)
    {
        PACK(DWORD, m_dw0C);
    }

    return PackSize;
}

BOOL DefaultScriptPartHook::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(DWORD, m_dw0C);
    return TRUE;
}

//=============================================================================
// SoundHook
//=============================================================================

SoundHook::SoundHook()
{
    m_dwSoundID = 0;
}

void SoundHook::Execute(CPhysicsObj *pOwner)
{
    UNFINISHED_WARNING("SoundHook::Execute");
}

int SoundHook::GetType()
{
    return 1;
}

void SoundHook::GetDataRecursion(LPVOID)
{
    UNFINISHED("SoundHook::GetDataRecursion");
}

ULONG SoundHook::pack_size()
{
    return sizeof(DWORD);
}

ULONG SoundHook::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    PACK(DWORD, m_dwSoundID);

    return PackSize;
}

BOOL SoundHook::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(DWORD, m_dwSoundID);

    // SoundManager::CreateSound(m_dwSoundID);
    UNFINISHED_WARNING("SoundManager::CreateSound(m_dwSoundID)");

    return TRUE;
}

//=============================================================================
// SoundTableHook
//=============================================================================

SoundTableHook::SoundTableHook()
{
    m_dwSoundID = 0;
}

void SoundTableHook::Execute(CPhysicsObj *pOwner)
{
    UNFINISHED_WARNING("SoundTableHook::Execute");
}

int SoundTableHook::GetType()
{
    return 2;
}

ULONG SoundTableHook::pack_size()
{
    return sizeof(DWORD);
}

ULONG SoundTableHook::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    PACK(DWORD, m_dwSoundID);

    return PackSize;
}

BOOL SoundTableHook::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(DWORD, m_dwSoundID);

    return TRUE;
}

//=============================================================================
//ReplaceObjectHook
//=============================================================================

ReplaceObjectHook::ReplaceObjectHook()
{
}

ReplaceObjectHook::~ReplaceObjectHook()
{
}

void ReplaceObjectHook::Execute(CPhysicsObj *pOwner)
{
    UNFINISHED_WARNING("ReplaceObjectHook::Execute");
}

int ReplaceObjectHook::GetType()
{
    return 5;
}

void ReplaceObjectHook::GetDataRecursion(LPVOID)
{
    UNFINISHED("ReplaceObjectHook::GetDataRecursion");
}

ULONG ReplaceObjectHook::pack_size()
{
    return m_Replacement.pack_size();
}

ULONG ReplaceObjectHook::Pack(BYTE** ppData, ULONG iSize)
{
    return PACK_OBJ(m_Replacement);
}

BOOL ReplaceObjectHook::UnPack(BYTE** ppData, ULONG iSize)
{
    return UNPACK_OBJ(m_Replacement);
}


//=============================================================================
//TransparentHook
//=============================================================================

TransparentHook::TransparentHook()
{
}

void TransparentHook::Execute(CPhysicsObj *pOwner)
{
    UNFINISHED_WARNING("TransparentHook::Execute");
}

int TransparentHook::GetType()
{
    return 20;
}

ULONG TransparentHook::pack_size()
{
    return(sizeof(float) * 3);
}

ULONG TransparentHook::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    if (iSize >= PackSize)
    {
        PACK(float, m_f0C);
        PACK(float, m_f10);
        PACK(float, m_f14);
    }

    return PackSize;
}

BOOL TransparentHook::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(float, m_f0C);
    UNPACK(float, m_f10);
    UNPACK(float, m_f14);

    return TRUE;
}

//=============================================================================
// TransparentPartHook
//=============================================================================

TransparentPartHook::TransparentPartHook()
{
}

void TransparentPartHook::Execute(CPhysicsObj *pOwner)
{
    UNFINISHED_WARNING("TransparentPartHook::Execute");
}

int TransparentPartHook::GetType()
{
    return 7;
}

ULONG TransparentPartHook::pack_size()
{
    return(sizeof(DWORD) + sizeof(float)*3);
}

ULONG TransparentPartHook::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    if (iSize >= PackSize)
    {
        PACK(DWORD, m_dwPart);
        PACK(float, m_f10);
        PACK(float, m_f14);
        PACK(float, m_f18);
    }

    return PackSize;
}

BOOL TransparentPartHook::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(DWORD, m_dwPart);
    UNPACK(float, m_f10);
    UNPACK(float, m_f14);
    UNPACK(float, m_f18);

    return TRUE;
}

//=============================================================================
// LuminousPartHook
//=============================================================================

LuminousPartHook::LuminousPartHook()
{
}

void LuminousPartHook::Execute(CPhysicsObj *pOwner)
{
    UNFINISHED_WARNING("LuminousPartHook::Execute");
}

int LuminousPartHook::GetType()
{
    return 9;
}

ULONG LuminousPartHook::pack_size()
{
    return(sizeof(DWORD) + sizeof(float)*3);
}

ULONG LuminousPartHook::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    if (iSize >= PackSize)
    {
        PACK(DWORD, m_dwPart);
        PACK(float, m_f10);
        PACK(float, m_f14);
        PACK(float, m_f18);
    }

    return PackSize;
}

BOOL LuminousPartHook::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(DWORD, m_dwPart);
    UNPACK(float, m_f10);
    UNPACK(float, m_f14);
    UNPACK(float, m_f18);

    return TRUE;
}

//=============================================================================
// LuminousHook
//=============================================================================

LuminousHook::LuminousHook()
{
}

void LuminousHook::Execute(CPhysicsObj *pOwner)
{
    UNFINISHED_WARNING("LuminousHook::Execute");
}

int LuminousHook::GetType()
{
    return 8;
}

ULONG LuminousHook::pack_size()
{
    return(sizeof(float)*3);
}

ULONG LuminousHook::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    if (iSize >= PackSize)
    {
        PACK(float, m_f0C);
        PACK(float, m_f10);
        PACK(float, m_f14);
    }

    return PackSize;
}

BOOL LuminousHook::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(float, m_f0C);
    UNPACK(float, m_f10);
    UNPACK(float, m_f14);

    return TRUE;
}

//=============================================================================
// DiffusePartHook
//=============================================================================

DiffusePartHook::DiffusePartHook()
{
}

void DiffusePartHook::Execute(CPhysicsObj *pOwner)
{
    UNFINISHED_WARNING("DiffusePartHook::Execute");
}

int DiffusePartHook::GetType()
{
    return 11;
}

ULONG DiffusePartHook::pack_size()
{
    return(sizeof(DWORD) + sizeof(float)*3);
}

ULONG DiffusePartHook::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    if (iSize >= PackSize)
    {
        PACK(DWORD, m_dwPart);
        PACK(float, m_f10);
        PACK(float, m_f14);
        PACK(float, m_f18);
    }

    return PackSize;
}

BOOL DiffusePartHook::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(DWORD, m_dwPart);
    UNPACK(float, m_f10);
    UNPACK(float, m_f14);
    UNPACK(float, m_f18);

    return TRUE;
}

//=============================================================================
// DiffuseHook
//=============================================================================

DiffuseHook::DiffuseHook()
{
}

void DiffuseHook::Execute(CPhysicsObj *pOwner)
{
    UNFINISHED_WARNING("DiffuseHook::Execute");
}

int DiffuseHook::GetType()
{
    return 10;
}

ULONG DiffuseHook::pack_size()
{
    return(sizeof(float)*3);
}

ULONG DiffuseHook::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    if (iSize >= PackSize)
    {
        PACK(float, m_f0C);
        PACK(float, m_f10);
        PACK(float, m_f14);
    }

    return PackSize;
}

BOOL DiffuseHook::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(float, m_f0C);
    UNPACK(float, m_f10);
    UNPACK(float, m_f14);

    return TRUE;
}

//=============================================================================
// ScaleHook
//=============================================================================

ScaleHook::ScaleHook()
{
}

void ScaleHook::Execute(CPhysicsObj *pOwner)
{
    UNFINISHED_WARNING("ScaleHook::Execute");
}

int ScaleHook::GetType()
{
    return 12;
}

ULONG ScaleHook::pack_size()
{
    return(sizeof(float)*2);
}

ULONG ScaleHook::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    if (iSize >= PackSize)
    {
        PACK(float, m_f0C);
        PACK(float, m_f10);
    }

    return PackSize;
}

BOOL ScaleHook::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(float, m_f0C);
    UNPACK(float, m_f10);

    return TRUE;
}

//=============================================================================
// CreateParticleHook
//=============================================================================

CreateParticleHook::CreateParticleHook()
{
}

void CreateParticleHook::Execute(CPhysicsObj *pOwner)
{
    pOwner->create_particle_emitter(m_dw0C, m_dw10, &m_Position, m_dw54);
}

int CreateParticleHook::GetType()
{
    return 13;
}

void CreateParticleHook::GetDataRecursion(LPVOID)
{
    UNFINISHED("CreateParticleHook::GetDataRecursion");
}

ULONG CreateParticleHook::pack_size()
{
    return(sizeof(DWORD) + sizeof(long) + m_Position.pack_size() + sizeof(DWORD));
}

ULONG CreateParticleHook::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    if (iSize >= PackSize)
    {
        PACK(DWORD, m_dw0C);
        PACK(DWORD, m_dw10);
        PACK_OBJ(m_Position);
        PACK(DWORD, m_dw54);
    }

    return PackSize;
}

BOOL CreateParticleHook::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(DWORD, m_dw0C);
    UNPACK(DWORD, m_dw10);
    UNPACK_OBJ(m_Position);
    UNPACK(DWORD, m_dw54);

    return TRUE;
}

//=============================================================================
// CreateBlockingParticleHook
//=============================================================================

CreateBlockingParticleHook::CreateBlockingParticleHook()
{
}

void CreateBlockingParticleHook::Execute(CPhysicsObj *pOwner)
{
    UNFINISHED_WARNING("CreateBlockingParticleHook::Execute");
}

int CreateBlockingParticleHook::GetType()
{
    return 26;
}

//=============================================================================
// DestroyParticleHook
//=============================================================================

DestroyParticleHook::DestroyParticleHook()
{
}

void DestroyParticleHook::Execute(CPhysicsObj *pOwner)
{
    UNFINISHED_WARNING("DestroyParticleHook::Execute");
}

int DestroyParticleHook::GetType()
{
    return 14;
}

ULONG DestroyParticleHook::pack_size()
{
    return(sizeof(DWORD));
}

ULONG DestroyParticleHook::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    if (iSize >= PackSize)
    {
        PACK(DWORD, m_dw0C);
    }

    return PackSize;
}

BOOL DestroyParticleHook::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(DWORD, m_dw0C);

    return TRUE;
}

//=============================================================================
// StopParticleHook
//=============================================================================

StopParticleHook::StopParticleHook()
{
}

void StopParticleHook::Execute(CPhysicsObj *pOwner)
{
    UNFINISHED_WARNING("StopParticleHook::Execute");
}

int StopParticleHook::GetType()
{
    return 14;
}

ULONG StopParticleHook::pack_size()
{
    return(sizeof(DWORD));
}

ULONG StopParticleHook::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    if (iSize >= PackSize)
    {
        PACK(DWORD, m_dw0C);
    }

    return PackSize;
}

BOOL StopParticleHook::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(DWORD, m_dw0C);

    return TRUE;
}

//=============================================================================
// CallPESHook
//=============================================================================

CallPESHook::CallPESHook()
{
}

void CallPESHook::Execute(CPhysicsObj *pOwner)
{
    UNFINISHED_WARNING("CallPESHook::Execute");
}

int CallPESHook::GetType()
{
    return 19;
}

void CallPESHook::GetDataRecursion(LPVOID)
{
    UNFINISHED("CallPESHook::GetDataRecursion");
}

ULONG CallPESHook::pack_size()
{
    return(sizeof(DWORD) + sizeof(float));
}

ULONG CallPESHook::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    if (iSize >= PackSize)
    {
        PACK(DWORD, m_dw0C);
        PACK(float, m_f10);
    }

    return PackSize;
}

BOOL CallPESHook::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(DWORD, m_dw0C);
    UNPACK(float, m_f10);

    return TRUE;
}

//=============================================================================
// SoundTweakedHook
//=============================================================================

SoundTweakedHook::SoundTweakedHook()
{
    m_dw0C = 0;
    m_f10 = 0.9f;
    m_f14 = 0.0f;
    m_f18 = 0.0f;
}

SoundTweakedHook::~SoundTweakedHook()
{
    // DEBUGOUT("Missing DestroySound call here!\r\n");
    // SoundManager::DestroySound
    // __asm int 3;
}

void SoundTweakedHook::Execute(CPhysicsObj *pOwner)
{
    // MISSING CODE HERE
    // __asm int 3;
    UNFINISHED_WARNING("SoundTweakedHook::Execute");
}

int SoundTweakedHook::GetType()
{
    return 21;
}

void SoundTweakedHook::GetDataRecursion(LPVOID)
{
    UNFINISHED("SoundTweakedHook::GetDataRecursion");
}

ULONG SoundTweakedHook::pack_size()
{
    return(sizeof(DWORD) + sizeof(float)*3);
}

ULONG SoundTweakedHook::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    PACK(DWORD, m_dw0C);
    PACK(float, m_f14);
    PACK(float, m_f10);
    PACK(float, m_f18);

    return PackSize;
}

BOOL SoundTweakedHook::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(DWORD, m_dw0C);
    UNPACK(float, m_f14);
    UNPACK(float, m_f10);
    UNPACK(float, m_f18);

    // DEBUGOUT("Missing CreateSound call here!\r\n");
    // SoundManager::CreateSound
    // __asm int 3;

    return TRUE;
}

//=============================================================================
// SetOmegaHook
//=============================================================================

SetOmegaHook::SetOmegaHook()
{
}

void SetOmegaHook::Execute(CPhysicsObj *pOwner)
{
    UNFINISHED("SetOmegaHook::Execute");
}

int SetOmegaHook::GetType()
{
    return 22;
}

ULONG SetOmegaHook::pack_size()
{
    return(sizeof(DWORD) + sizeof(float)*2);
}

ULONG SetOmegaHook::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    if (iSize >= PackSize)
    {
        // Actually another class?
        PACK(DWORD, m_dw0C);
        PACK(float, m_f10);
        PACK(float, m_f14);
    }

    return PackSize;
}

BOOL SetOmegaHook::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(DWORD, m_dw0C);
    UNPACK(float, m_f10);
    UNPACK(float, m_f14);

    return TRUE;
}

//=============================================================================
// TextureVelocityHook
//=============================================================================

TextureVelocityHook::TextureVelocityHook()
{
    m_f0C = 0.0f;
    m_f10 = 0.0f;
}

void TextureVelocityHook::Execute(CPhysicsObj *pOwner)
{
    UNFINISHED("TextureVelocityHook::Execute");
}

int TextureVelocityHook::GetType()
{
    return 23;
}

ULONG TextureVelocityHook::pack_size()
{
    return(sizeof(float)*2);
}

ULONG TextureVelocityHook::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    if (iSize >= PackSize)
    {
        PACK(float, m_f0C);
        PACK(float, m_f10);
    }

    return PackSize;
}

BOOL TextureVelocityHook::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(float, m_f0C);
    UNPACK(float, m_f10);

    return TRUE;
}

//=============================================================================
// TextureVelocityPartHook
//=============================================================================

TextureVelocityPartHook::TextureVelocityPartHook()
{
    m_dw0C = -1;
    m_f10 = 0.0f;
    m_f14 = 0.0f;
}

void TextureVelocityPartHook::Execute(CPhysicsObj *pOwner)
{
    UNFINISHED("TextureVelocityPartHook::Execute");
}

int TextureVelocityPartHook::GetType()
{
    return 24;
}

ULONG TextureVelocityPartHook::pack_size()
{
    return(sizeof(DWORD) + sizeof(float)*2);
}

ULONG TextureVelocityPartHook::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    if (iSize >= PackSize)
    {
        PACK(DWORD, m_dw0C);
        PACK(float, m_f10);
        PACK(float, m_f14);
    }

    return PackSize;
}

BOOL TextureVelocityPartHook::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(DWORD, m_dw0C);
    UNPACK(float, m_f10);
    UNPACK(float, m_f14);

    return TRUE;
}

//=============================================================================
// SetLightHook
//=============================================================================

SetLightHook::SetLightHook()
{
}

void SetLightHook::Execute(CPhysicsObj *pOwner)
{
    UNFINISHED_WARNING("SetLightHook::Execute");
}

int SetLightHook::GetType()
{
    return 25;
}

ULONG SetLightHook::pack_size()
{
    return(sizeof(DWORD));
}

ULONG SetLightHook::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    if (iSize >= PackSize)
    {
        PACK(DWORD, m_dw0C);
    }

    return PackSize;
}

BOOL SetLightHook::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(DWORD, m_dw0C);

    return TRUE;
}

//=============================================================================
// AttackCone -- Needs to be moved elsewhere
//=============================================================================

AttackCone::AttackCone()
{
}

ULONG AttackCone::pack_size()
{
    return(sizeof(DWORD) + m_Vec1.pack_size() + m_Vec2.pack_size() + sizeof(float)*2);
}

ULONG AttackCone::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    if (iSize >= PackSize)
    {
        PACK(DWORD, m_dw00);
        PACK_OBJ(m_Vec1);
        PACK_OBJ(m_Vec2);
        PACK(float, m_f14);
        PACK(float, m_f18);
    }

    return PackSize;
}

BOOL AttackCone::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(DWORD, m_dw00);
    UNPACK_OBJ(m_Vec1);
    UNPACK_OBJ(m_Vec2);
    UNPACK(float, m_f14);
    UNPACK(float, m_f18);
        
    return TRUE;
}

//=============================================================================
// AttackHook
//=============================================================================

AttackHook::AttackHook()
{
}

void AttackHook::Execute(CPhysicsObj *pOwner)
{
    UNFINISHED_WARNING("AttackHook::Execute");
}

int AttackHook::GetType()
{
    return 3;
}

ULONG AttackHook::pack_size()
{
    return(m_cone.pack_size());
}

ULONG AttackHook::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    if (iSize >= PackSize)
    {
        PACK_OBJ(m_cone);
    }

    return PackSize;
}

BOOL AttackHook::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK_OBJ(m_cone);

    return TRUE;
}

//=============================================================================
// EtherealHook
//=============================================================================

EtherealHook::EtherealHook()
{
}

void EtherealHook::Execute(CPhysicsObj *pOwner)
{
    UNFINISHED_WARNING("EtherealHook::Execute");
}

int EtherealHook::GetType()
{
    return 6;
}

ULONG EtherealHook::pack_size()
{
    return(sizeof(DWORD));
}

ULONG EtherealHook::Pack(BYTE** ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    if (iSize >= PackSize)
    {
        PACK(DWORD, m_dw0C);
    }

    return PackSize;
}

BOOL EtherealHook::UnPack(BYTE** ppData, ULONG iSize)
{
    UNPACK(DWORD, m_dw0C);

    return TRUE;
}