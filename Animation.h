
#pragma once

#include "ObjCache.h"
#include "DLListBase.h"

class CAnimation;
class AFrame;
class AnimFrame;

class AnimData : public PackObj
{
public:
    AnimData();
    ~AnimData();

    ULONG    pack_size();
    BOOL    UnPack(BYTE **ppData, ULONG iSize);

    DWORD    m_AnimID; // 0x04
    long    m_LowFrame; // 0x08
    long    m_HighFrame; // 0x0C
    float    m_PlaySpeed; // 0x10
};

class AnimSequenceNode : public PackObj, public DLListNode
{
public:
    AnimSequenceNode();
    AnimSequenceNode(AnimData *pAnimData);
    virtual ~AnimSequenceNode();

    ULONG    pack_size();
    BOOL    UnPack(BYTE **ppData, ULONG iSize);

    float    get_framerate();
    void    multiply_framerate(float fRate);

    BOOL    has_anim();
    long    get_low_frame();
    long    get_high_frame();
    float    get_starting_frame();
    float    get_ending_frame();
    void    set_animation_id(DWORD AnimID);

    AnimFrame    *get_part_frame(long index);
    AFrame        *get_pos_frame(long index);

    AnimSequenceNode *GetNext();
    AnimSequenceNode *GetPrev();

//private:
    
    // DLListNode inherited..
    // AnimSequenceNode    *m_pNext;            // 0x04
    // AnimSequenceNode    *m_pPrev;            // 0x08 
    CAnimation            *m_pAnimation;        // 0x0C
    float                m_fFrameRate;        // 0x10
    long                m_LowFrame;        // 0x14
    long                m_HighFrame;        // 0x18
};

class CAnimation : public DBObj
{
public:
    CAnimation();
    virtual ~CAnimation();

    static DBObj *Allocator();
    static void Destroyer(DBObj *pAnimation);
    static CAnimation *Get(DWORD ID);
    static void Release(CAnimation *pAnimation);

    void Destroy();
    BOOL UnPack(BYTE **ppData, ULONG iSize);

    // Inherited from unknown class?
    DWORD        m_dw18;
    DWORD        m_dw1C;
    DWORD        m_dw20;
    DWORD        m_dw24;

    // CAnimation members..
    AFrame        *m_pAFrames;        // 0x1C / 0x28
    AnimFrame    *m_pAnimFrames;    // 0x20 / 0x2C
    BOOL        m_bFlag2;            // 0x24 / 0x30
    DWORD        m_PartCount;        // 0x28 / 0x34
    DWORD        m_FrameCount;        // 0x2C / 0x38

};




