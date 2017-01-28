
#pragma once

#include "PackObj.h"
#include "MathLib.h"
#include "Frame.h"

class CPhysicsObj;

class CAnimHook
{
public:
    CAnimHook();
    virtual    ~CAnimHook();

    static CAnimHook* UnPackHook(BYTE** ppData, ULONG iSize);

    virtual void    Execute(CPhysicsObj *pOwner) = 0;
    virtual int        GetType() = 0; 
    virtual void    GetDataRecursion(LPVOID);
    virtual ULONG    pack_size() = 0;
    virtual ULONG    Pack(BYTE** ppData, ULONG iSize) = 0;
    virtual BOOL    UnPack(BYTE** ppData, ULONG iSize) = 0;

    // Hooks will form a list.
    void add_to_list(CAnimHook** pList);

    CAnimHook*        m_pNext; // 0x04
    DWORD            m_Key; // 0x08
};

class AnimFrame
{
public:
    AnimFrame();
    ~AnimFrame();

    void Destroy();
    BOOL UnPack(DWORD ObjCount, BYTE **ppData, ULONG iSize);

    AFrame*                m_pFrames;
    DWORD                m_HookCount;
    CAnimHook*            m_pHooks;    // linked-list of hookable events
    DWORD                m_FrameCount;
};

class AnimPartChange : public PackObj
{
public:
    AnimPartChange();

    BOOL UnPack(BYTE **ppData, ULONG iSize);
    BOOL replaces(AnimPartChange *pChange);

    DWORD m_PartIndex;
    DWORD m_GfxObjID;
    DWORD m_dw0C;
    DWORD m_dw10;
};

class NOOPHook : public CAnimHook
{
public:
    NOOPHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType() ; 
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);
};

class NoDrawHook : public CAnimHook
{
public:
    NoDrawHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType() ; 
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    BOOL    m_bDraw;
};

class DefaultScriptHook : public CAnimHook
{
public:
    DefaultScriptHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType(); 
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);
};

class DefaultScriptPartHook : public CAnimHook
{
public:
    DefaultScriptPartHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType(); 
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    DWORD    m_dw0C;
};

class SoundHook : public CAnimHook
{
public:
    SoundHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType();
    void    GetDataRecursion(LPVOID);
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    DWORD    m_dwSoundID;
};

class SoundTableHook : public CAnimHook
{
public:
    SoundTableHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType();
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    DWORD    m_dwSoundID;
};

class ReplaceObjectHook : public CAnimHook
{
public:
    ReplaceObjectHook();
    ~ReplaceObjectHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType();
    void    GetDataRecursion(LPVOID);
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    // This should get the job done.
    AnimPartChange    m_Replacement;
};

class TransparentHook : public CAnimHook
{
public:
    TransparentHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType();
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    float m_f0C;
    float m_f10;
    float m_f14;
};

class TransparentPartHook : public CAnimHook
{
public:
    TransparentPartHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType();
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    DWORD m_dwPart;
    float m_f10;
    float m_f14;
    float m_f18;
};

class LuminousPartHook : public CAnimHook
{
public:
    LuminousPartHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType();
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    DWORD m_dwPart;
    float m_f10;
    float m_f14;
    float m_f18;
};

class LuminousHook : public CAnimHook
{
public:
    LuminousHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType();
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    float m_f0C;
    float m_f10;
    float m_f14;
};

class DiffusePartHook : public CAnimHook
{
public:
    DiffusePartHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType();
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    DWORD m_dwPart;
    float m_f10;
    float m_f14;
    float m_f18;
};

class DiffuseHook : public CAnimHook
{
public:
    DiffuseHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType();
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    float m_f0C;
    float m_f10;
    float m_f14;
};

class ScaleHook : public CAnimHook
{
public:
    ScaleHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType();
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    float m_f0C;
    float m_f10;
};

class CreateParticleHook : public CAnimHook
{
public:
    CreateParticleHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType();
    void    GetDataRecursion(LPVOID);
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    DWORD    m_dw0C;
    DWORD    m_dw10;
    Frame    m_Position;
    DWORD    m_dw54;
};

class CreateBlockingParticleHook : public CreateParticleHook
{
public:
    CreateBlockingParticleHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType();
};

class DestroyParticleHook : public CAnimHook
{
public:
    DestroyParticleHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType();
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    DWORD    m_dw0C;
};

class StopParticleHook : public CAnimHook
{
public:
    StopParticleHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType();
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    DWORD    m_dw0C;
};

class CallPESHook : public CAnimHook
{
public:
    CallPESHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType();
    void    GetDataRecursion(LPVOID);
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    DWORD    m_dw0C;
    float    m_f10;
};

class SoundTweakedHook : public CAnimHook
{
public:
    SoundTweakedHook();
    ~SoundTweakedHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType();
    void    GetDataRecursion(LPVOID);
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    DWORD    m_dw0C;
    float    m_f10;
    float    m_f14;
    float    m_f18;
};

class SetOmegaHook : public CAnimHook
{
public:
    SetOmegaHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType();
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    DWORD    m_dw0C;
    float    m_f10;
    float    m_f14;
};

class TextureVelocityHook : public CAnimHook
{
public:
    TextureVelocityHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType();
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    float    m_f0C;
    float    m_f10;
};

class TextureVelocityPartHook : public CAnimHook
{
public:
    TextureVelocityPartHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType();
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    DWORD    m_dw0C;
    float    m_f10;
    float    m_f14;
};

class SetLightHook : public CAnimHook
{
public:
    SetLightHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType();
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    DWORD    m_dw0C;
};

// Need to be moved elsewhere!
class AttackCone
{
public:
    AttackCone();

    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    DWORD    m_dw00;
    Vec2D    m_Vec1;
    Vec2D    m_Vec2;
    float    m_f14;
    float    m_f18;
};

class AttackHook : public CAnimHook
{
public:
    AttackHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType();
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    AttackCone    m_cone;
};

class EtherealHook : public CAnimHook
{
public:
    EtherealHook();

    void    Execute(CPhysicsObj *pOwner);
    int        GetType();
    ULONG    pack_size();
    ULONG    Pack(BYTE** ppData, ULONG iSize);
    BOOL    UnPack(BYTE** ppData, ULONG iSize);

    DWORD    m_dw0C;
};
