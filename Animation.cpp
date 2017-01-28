
#include "StdAfx.h"
#include "AnimHooks.h"
#include "Animation.h"


AnimData::AnimData()
{
    m_AnimID = 0;
    m_LowFrame = 0;
    m_HighFrame = -1;
    m_PlaySpeed = 30.0f;
}

AnimData::~AnimData()
{
}

ULONG AnimData::pack_size()
{
    return(0x10);
}

BOOL AnimData::UnPack(BYTE **ppData, ULONG iSize)
{
    if (iSize < pack_size())
        return FALSE;

    UNPACK(DWORD, m_AnimID);
    UNPACK(long, m_LowFrame);
    UNPACK(long, m_HighFrame);
    UNPACK(float, m_PlaySpeed);

    return TRUE;
}

AnimSequenceNode::AnimSequenceNode()
{
    m_pAnimation = NULL;
    m_fFrameRate = 30.0f;
    m_LowFrame = -1;
    m_HighFrame = -1;
}

AnimSequenceNode::AnimSequenceNode(AnimData *pAnimData)
{
    m_pAnimation    = NULL;
    m_fFrameRate    = pAnimData->m_PlaySpeed;
    m_LowFrame        = pAnimData->m_LowFrame;
    m_HighFrame        = pAnimData->m_HighFrame;

    set_animation_id(pAnimData->m_AnimID);
}

AnimSequenceNode::~AnimSequenceNode()
{
    if (m_pAnimation)
        CAnimation::Release(m_pAnimation);
}

ULONG AnimSequenceNode::pack_size()
{
    return(0x10);
}

BOOL AnimSequenceNode::UnPack(BYTE **ppData, ULONG iSize)
{
    if (iSize < pack_size())
        return FALSE;

    DWORD AnimID;
    UNPACK(DWORD, AnimID);

    UNPACK(long, m_LowFrame);
    UNPACK(long, m_HighFrame);
    UNPACK(float, m_fFrameRate);

    set_animation_id(AnimID);

    return TRUE;
}

float AnimSequenceNode::get_framerate()
{
    return m_fFrameRate;
}

BOOL AnimSequenceNode::has_anim()
{
    return(m_pAnimation ? TRUE : FALSE);
}

void AnimSequenceNode::multiply_framerate(float fRate)
{
    if (fRate < 0)
        std::swap(m_LowFrame, m_HighFrame);
    
    m_fFrameRate *= fRate;
}

float AnimSequenceNode::get_starting_frame()
{
    if (m_fFrameRate >= 0)
        return (float)(m_LowFrame);
    else
        return (float)(m_HighFrame + 1) - F_EPSILON;
}

float AnimSequenceNode::get_ending_frame()
{
    if (m_fFrameRate >= 0)
        return (float)(m_HighFrame + 1) - F_EPSILON;
    else
        return (float)(m_LowFrame);
}

long AnimSequenceNode::get_low_frame()
{
    return m_LowFrame;
}

long AnimSequenceNode::get_high_frame()
{
    return m_HighFrame;
}

AnimSequenceNode *AnimSequenceNode::GetNext()
{
    return(static_cast<AnimSequenceNode *>(m_pNext));
}

AnimSequenceNode *AnimSequenceNode::GetPrev()
{
    return(static_cast<AnimSequenceNode *>(m_pPrev));
}

AnimFrame *AnimSequenceNode::get_part_frame(long index)
{
    if (!m_pAnimation)
        return NULL;

    if (index < 0 || index >= m_pAnimation->m_FrameCount)
        return NULL;

    return(&m_pAnimation->m_pAnimFrames[ index ]);
}

AFrame *AnimSequenceNode::get_pos_frame(long index)
{
    if (!m_pAnimation)
        return NULL;

    if (index < 0 || index >= m_pAnimation->m_FrameCount)
        return NULL;

    return(&m_pAnimation->m_pAFrames[ index ]);
}

void AnimSequenceNode::set_animation_id(DWORD AnimID)
{
    if (m_pAnimation)
        CAnimation::Release(m_pAnimation);

    m_pAnimation = CAnimation::Get(AnimID);

    if (m_pAnimation)
    {

        if (m_HighFrame < 0)
            m_HighFrame = m_pAnimation->m_FrameCount - 1;
        
        if (((DWORD)m_LowFrame) >= m_pAnimation->m_FrameCount)
            m_LowFrame = m_pAnimation->m_FrameCount - 1;

        if (((DWORD)m_HighFrame) >= m_pAnimation->m_FrameCount)
            m_HighFrame = m_pAnimation->m_FrameCount - 1;

        if (m_LowFrame > m_HighFrame)
            m_HighFrame = m_LowFrame;
    }
}

CAnimation::CAnimation() : DBObj(ObjCaches::Animations)
{
    m_dw18 = 0;
    m_dw1C = 0;
    m_dw20 = 0;
    m_dw24 = 0;

    m_pAFrames = NULL;
    m_pAnimFrames = NULL;
    m_bFlag2 = FALSE;
    m_PartCount = 0;
    m_FrameCount = 0;
}

CAnimation::~CAnimation()
{
    Destroy();
}

DBObj *CAnimation::Allocator()
{
    return((DBObj *)new CAnimation());
}

void CAnimation::Destroyer(DBObj *pAnimation)
{
    delete((CAnimation *)pAnimation);
}

CAnimation *CAnimation::Get(DWORD ID)
{
    return (CAnimation *)ObjCaches::Animations->Get(ID);
}

void CAnimation::Release(CAnimation *pAnimation)
{
    if (pAnimation)
        ObjCaches::Animations->Release(pAnimation->m_Key);
}

void CAnimation::Destroy()
{
    if (m_pAFrames)
    {
        delete [] m_pAFrames;
        m_pAFrames = NULL;
    }

    if (m_pAnimFrames)
    {
        delete [] m_pAnimFrames;
        m_pAnimFrames = NULL;
    }

    m_FrameCount = 0;
    m_PartCount = 0;
}

BOOL CAnimation::UnPack(BYTE **ppData, ULONG iSize)
{
    Destroy();

    UNPACK(DWORD, m_Key);

    DWORD PackFlags;

    UNPACK(DWORD, PackFlags);

    UNPACK(DWORD, m_PartCount);
    UNPACK(DWORD, m_FrameCount);

    if (PackFlags & 1)
    {
        m_pAFrames = new AFrame[ m_FrameCount ];

        for (DWORD i = 0; i < m_FrameCount; i++)
            UNPACK_OBJ(m_pAFrames[i]);
    }

    m_bFlag2 = (PackFlags & 2) ? TRUE : FALSE;

    m_pAnimFrames = new AnimFrame[ m_FrameCount ];

    for (DWORD i = 0; i < m_FrameCount; i++)
        m_pAnimFrames[i].UnPack(m_PartCount, ppData, iSize);

    return TRUE;
}


