
#include "StdAfx.h"
#include "Frame.h"
#include "AnimHooks.h"
#include "Setup.h"
#include "Palette.h"
#include "PhysicsPart.h"
#include "PartArray.h"
#include "GfxObj.h"
#include "Animation.h"
#include "PhysicsObj.h"
#include "ObjCell.h"

CSequence::CSequence()
{
    m_pNodes = NULL;
    m_Velocity = Vector(0, 0, 0);
    m_Omega = Vector(0, 0, 0);
    m_pPhysicsObj = NULL; // 0x28
    m_CurrentFrameNum = 0.0;
    m_CurrentAnim = NULL;
    m_PlacementFrame = NULL;
    m_PlacementID = 0;
    m_dw44 = 0;
}

CSequence::~CSequence()
{
    if (!m_Animations.Empty())
        m_Animations.DestroyContents();
}

void CSequence::set_object(CPhysicsObj *pPhysicsObj)
{
    m_pPhysicsObj = pPhysicsObj;
}

void CSequence::set_placement_frame(AnimFrame *PlacementFrame, DWORD ID)
{
    m_PlacementFrame = PlacementFrame;
    m_PlacementID = ID;
}

AnimFrame *CSequence::get_curr_animframe()
{
    if (!m_CurrentAnim)
        return m_PlacementFrame;

    return m_CurrentAnim->get_part_frame(get_curr_frame_number());
}

long CSequence::get_curr_frame_number()
{
    return((long)((double)floor(m_CurrentFrameNum)));
}

void CSequence::multiply_cyclic_animation_framerate(float fRate)
{
    AnimSequenceNode *pNode = m_pNodes;

    while (pNode)
    {
        pNode->multiply_framerate(fRate);
        pNode = pNode->GetNext();
    }
}

void CSequence::remove_cyclic_anims()
{
    // Not done.
    __asm int 3;
}

void CSequence::set_velocity(const Vector& Velocity)
{
    m_Velocity = Velocity;
}

void CSequence::set_omega(const Vector& Omega)
{
    m_Omega = Omega;
}

void CSequence::clear()
{
    clear_animations();
    clear_physics();

    m_PlacementFrame = NULL;
    m_PlacementID = 0;
}

void CSequence::clear_animations()
{
    if (!m_Animations.Empty())
        m_Animations.DestroyContents();

    m_pNodes = NULL;
    m_CurrentFrameNum = 0;
    m_CurrentAnim = NULL;
}

void CSequence::apply_physics(Frame* pframe, double arg4, double argC)
{
    if (argC >= 0.0)
        arg4 = fabs(arg4);
    else
        arg4 = -fabs(arg4);

    pframe->m_origin += m_Velocity * arg4;
    pframe->rotate(m_Omega * arg4);
}

void CSequence::apricot()
{
    AnimSequenceNode *pNode = static_cast<AnimSequenceNode *>(m_Animations.m_pListBegin);

    while (pNode != m_CurrentAnim)
    {
        if (pNode == m_pNodes)
            break;
        
        m_Animations.Remove(static_cast<DLListNode *>(pNode));

        if (pNode)
            delete pNode;

        pNode = static_cast<AnimSequenceNode *>(m_Animations.m_pListBegin);
    }
}

void CSequence::update(double time_elapsed, Frame* pframe)
{
    if (!m_Animations.Empty())
    {
        update_internal(time_elapsed, &m_CurrentAnim, &m_CurrentFrameNum, pframe);
        apricot();
    }
    else
    {
        if (pframe)
            apply_physics(pframe, time_elapsed, time_elapsed);
    }
}

void CSequence::update_internal(double time_elapsed, AnimSequenceNode** ppanim, double *pframenum, Frame *pframe)
{
iterate_anim:

    double framerate = (*ppanim)->get_framerate();
    double frametime = framerate * time_elapsed;

    long lastframe = (long)floor(*pframenum);

    double newframenum = (*pframenum) + frametime;
    *pframenum = newframenum;

    double var_10 = 0;
    long var_28 = 0;

    if (frametime > 0.0)
    {
        if (double((*ppanim)->get_high_frame()) < floor(newframenum))
        {
            double whatever = (*pframenum) - (*ppanim)->get_high_frame() - 1.0;

            if (whatever < 0)
                whatever = 0.0;

            if (F_EPSILON < fabs(framerate))
                var_10 = whatever / framerate;
            else
                var_10 = 0;

            var_28 = 1;
            *pframenum = (*ppanim)->get_high_frame();
        }

        while (floor(*pframenum) > lastframe)
        {
            if (pframe)
            {
                if ((*ppanim)->m_pAnimation->m_pAFrames)
                    pframe->combine(pframe, (*ppanim)->get_pos_frame(lastframe));
                
                if (F_EPSILON < fabs(framerate))
                    apply_physics(pframe, 1.0 / framerate, time_elapsed);
            }

            execute_hooks((*ppanim)->get_part_frame(lastframe), 1);

            lastframe++;
        }
    }
    else if (frametime < 0.0)
    {
        if (double((*ppanim)->get_low_frame()) > floor(newframenum))
        {
            double whatever = (*pframenum) - (*ppanim)->get_low_frame();

            if (whatever > 0)
                whatever = 0.0;

            if (F_EPSILON < fabs(framerate))
                var_10 = whatever / framerate;
            else
                var_10 = 0;

            var_28 = 1;
            *pframenum = (*ppanim)->get_low_frame();
        }

        while (floor(*pframenum) < lastframe)
        {
            if (pframe)
            {
                if ((*ppanim)->m_pAnimation->m_pAFrames)
                    pframe->subtract1(pframe, (*ppanim)->get_pos_frame(lastframe));
                
                if (F_EPSILON < fabs(framerate))
                    apply_physics(pframe, 1.0 / framerate, time_elapsed);
            }

            execute_hooks((*ppanim)->get_part_frame(lastframe), -1);

            lastframe--;
        }
    }
    else
    {
        if (pframe)
        {    
            if (F_EPSILON < fabs(time_elapsed))
                apply_physics(pframe, time_elapsed, time_elapsed);
        }
    }

    if (var_28)
    {
        if (m_pPhysicsObj)
        {
            AnimSequenceNode *pNode = static_cast<AnimSequenceNode *>(m_Animations.m_pListBegin);

            if (pNode != m_pNodes) {
                // m_pPhysicsObj->add_anim_hook(anim_done_hook);
            }
        }

        advance_to_next_animation(time_elapsed, ppanim, pframenum, pframe);
        time_elapsed = var_10;

        goto iterate_anim;
    }
}

void CSequence::execute_hooks(AnimFrame *pAnimFrame, long lDir)
{
    if (m_pPhysicsObj)
    {
        // Finish me.
        CAnimHook *pHook = pAnimFrame->m_pHooks;

        while (pHook)
        {
            if (!pHook->m_Key || pHook->m_Key == lDir)
                m_pPhysicsObj->add_anim_hook(pHook);

            pHook = pHook->m_pNext;
        }
    }
}

void CSequence::advance_to_next_animation(double TimeElapsed, AnimSequenceNode** ppAnim, double *pFrameNum, Frame *pFrame)
{
    if (TimeElapsed >= 0.0)
    {
        if ((*ppAnim)->get_framerate() < 0.0f)
        {
            if (pFrame)
            {
                if ((*ppAnim)->m_pAnimation->m_pAFrames)
                    pFrame->subtract1(pFrame, (*ppAnim)->get_pos_frame((long)floor(*pFrameNum)));

                if (F_EPSILON < fabs((*ppAnim)->get_framerate()))
                    apply_physics(pFrame, 1.0 / (*ppAnim)->get_framerate(), TimeElapsed);
            }
        }

        if ((*ppAnim)->GetNext())
            *ppAnim = (*ppAnim)->GetNext();
        else
            *ppAnim = m_pNodes;

        *pFrameNum = (*ppAnim)->get_starting_frame();

        if ((*ppAnim)->get_framerate() > 0.0f)
        {
            if (pFrame)
            {
                if ((*ppAnim)->m_pAnimation->m_pAFrames)
                    pFrame->combine(pFrame, (*ppAnim)->get_pos_frame((long)floor(*pFrameNum)));

                if (F_EPSILON < fabs((*ppAnim)->get_framerate()))
                    apply_physics(pFrame, 1.0 / (*ppAnim)->get_framerate(), TimeElapsed);
            }
        }
    }
    else
    {
        if ((*ppAnim)->get_framerate() >= 0.0f)
        {
            if (pFrame)
            {
                if ((*ppAnim)->m_pAnimation->m_pAFrames)
                    pFrame->subtract1(pFrame, (*ppAnim)->get_pos_frame((long)floor(*pFrameNum)));

                if (F_EPSILON < fabs((*ppAnim)->get_framerate()))
                    apply_physics(pFrame, 1.0 / (*ppAnim)->get_framerate(), TimeElapsed);
            }
        }

        if ((*ppAnim)->GetPrev())
            *ppAnim = (*ppAnim)->GetPrev();
        else
            *ppAnim = static_cast<AnimSequenceNode *>(m_Animations.m_pListEnd);

        *pFrameNum = (*ppAnim)->get_ending_frame();

        if ((*ppAnim)->get_framerate() < 0.0f)
        {
            if (pFrame)
            {
                if ((*ppAnim)->m_pAnimation->m_pAFrames)
                    pFrame->combine(pFrame, (*ppAnim)->get_pos_frame((long)floor(*pFrameNum)));

                if (F_EPSILON < fabs((*ppAnim)->get_framerate()))
                    apply_physics(pFrame, 1.0 / (*ppAnim)->get_framerate(), TimeElapsed);
            }
        }
    }
}

void CSequence::combine_physics(const Vector& Velocity, const Vector& Omega)
{
    m_Velocity += Velocity;
    m_Omega += Omega;
}

void CSequence::subtract_physics(const Vector& Velocity, const Vector& Omega)
{
    m_Velocity -= Velocity;
    m_Omega -= Omega;
}

void CSequence::clear_physics()
{
    m_Velocity    = Vector(0, 0, 0);
    m_Omega        = Vector(0, 0, 0);
}

void CSequence::append_animation(AnimData *pAnimData)
{
    AnimSequenceNode
        *pNode = new AnimSequenceNode(pAnimData);

    if (!pNode->has_anim())
    {
        // Useless null check performed by the client.
        if (pNode)
            delete pNode;

        return;
    }

    DLListNode
        *pListNode = static_cast<DLListNode *>(pNode);

    m_Animations.InsertAfter(pListNode, m_Animations.m_pListEnd);

    m_pNodes = static_cast<AnimSequenceNode *>(m_Animations.m_pListEnd);

    if (!m_CurrentAnim)
    {
        m_CurrentAnim = static_cast<AnimSequenceNode *>(m_Animations.m_pListBegin);
        m_CurrentFrameNum = m_CurrentAnim->get_starting_frame();
    }
}

CPartArray::CPartArray()
{
    m_dw00 = 0;
    m_pPhysicsObj = NULL;
    m_pMTManager = NULL;
    m_pPartSetup = NULL;
    m_iPartCount = 0;
    m_pPartObjs = NULL;
    m_Scale = Vector(1.0f, 1.0f, 1.0f);
    m_pMush = NULL;
    m_pPalettes = NULL;
    m_pLightList = NULL;
    m_dw78 = 0;
}

CPartArray::~CPartArray()
{
    Destroy();
}

void CPartArray::Destroy()
{
    if (m_pMTManager)
    {
        delete m_pMTManager;
        m_pMTManager = NULL;
    }

    DestroyPals();
    DestroyLights();
    DestroyParts();
    DestroySetup();

    m_Sequence.set_object(NULL);

    m_dw00 = 0;
    m_pPhysicsObj = NULL;
}

CPartArray *CPartArray::CreateMesh(CPhysicsObj *pPhysicsObj, DWORD ID)
{
    CPartArray *pPartArray = new CPartArray();

    pPartArray->m_pPhysicsObj = pPhysicsObj;
    pPartArray->m_Sequence.set_object(pPhysicsObj);
    
    if (!pPartArray->SetMeshID(ID))
    {
        delete pPartArray;

        return NULL;
    }    

    pPartArray->SetPlacementFrame(0x65);

    return pPartArray;
}

CPartArray *CPartArray::CreateSetup(CPhysicsObj *pPhysicsObj, DWORD ID, BOOL bUnknown)
{
    CPartArray *pPartArray = new CPartArray();

    pPartArray->m_pPhysicsObj = pPhysicsObj;
    pPartArray->m_Sequence.set_object(pPhysicsObj);

    if (!pPartArray->SetSetupID(ID, bUnknown))
    {
        delete pPartArray;

        return NULL;
    }

    pPartArray->SetPlacementFrame(0x65);

    //if (RenderOptions::bSingleMushing)
    //     m_dw6C = CSingleMush::CreateMush(this);

    return pPartArray;
}

CPartArray *CPartArray::CreateParticle(CPhysicsObj *pPhysicsObj, DWORD ObjCount, Vector *Unused)
{
    CPartArray *pPartArray = new CPartArray();

    pPartArray->m_pPhysicsObj = pPhysicsObj;
    pPartArray->m_Sequence.set_object(pPhysicsObj);

    pPartArray->m_pPartSetup = CSetup::makeParticleSetup(ObjCount, Unused);

    if (!pPartArray->m_pPartSetup || !pPartArray->InitParts())
    {
        delete pPartArray;
        return NULL;
    }

    return pPartArray;
}

void CPartArray::AddLightsToCell(CObjCell *pCell)
{
    if (!pCell || !m_pLightList)
        return;

    for (DWORD i = 0; i < m_pLightList->m_LightCount; i++)
        pCell->add_light(&m_pLightList->m_Lights[i]);
}

void CPartArray::RemoveLightsFromCell(CObjCell *pCell)
{
    if (!pCell || !m_pLightList)
        return;

    for (DWORD i = 0; i < m_pLightList->m_LightCount; i++)
        pCell->remove_light(&m_pLightList->m_Lights[i]);
}

void CPartArray::SetCellID(DWORD ID)
{
    for (DWORD i = 0; i < m_iPartCount; i++)
    {
        if (m_pPartObjs[i])
            m_pPartObjs[i]->m_Pos30.m_LandCell = ID;
    }
}

BOOL CPartArray::SetMeshID(DWORD ID)
{
    if (!ID)
        return FALSE;

    CSetup *pSetup = CSetup::makeSimpleSetup(ID);

    if (!pSetup)
        return FALSE;

    DestroyParts();
    DestroySetup();

    m_pPartSetup = pSetup;

    if (!InitParts())
        return FALSE;

    return TRUE;
}

BOOL CPartArray::SetSetupID(DWORD ID, BOOL Unknown)
{
    if (m_pPartSetup && m_pPartSetup->m_Key == ID)
        return TRUE;

    CSetup *pSetup = CSetup::Get(ID);

    if (!pSetup)
        return FALSE;

    DestroyPals();
    DestroyLights();
    DestroyParts();
    DestroySetup();

    m_pPartSetup = pSetup;

    if (Unknown)
    {
        if (!InitParts())
            return FALSE;
    }

    InitLights();
    InitDefaults();

    return TRUE;
}

void CPartArray::DestroyParts()
{
    if (m_pPartObjs)
    {
        for (DWORD i = 0; i < m_iPartCount; i++)
        {
            if (m_pPartObjs[i])
                delete m_pPartObjs[i];
        }

        delete [] m_pPartObjs;
        m_pPartObjs = NULL;
    }
    m_iPartCount = 0;

    if (m_pMush)
    {
        delete m_pMush;
        m_pMush = NULL;
    }
}

void CPartArray::DestroySetup()
{
    if (m_pPartSetup)
    {
        if (m_pPartSetup->m_Key)
            CSetup::Release(m_pPartSetup);
        else
            delete m_pPartSetup;

        m_pPartSetup = NULL;
    }
}

void CPartArray::DestroyPals()
{
    if (m_pPalettes)
    {
        for (DWORD i = 0; i < m_iPartCount; i++)
        {
            if (m_pPalettes[i])
            {
                Palette::releasePalette(m_pPalettes[i]);
                m_pPalettes[i] = NULL;
            }
        }

        delete [] m_pPalettes;
        m_pPalettes = NULL;
    }
}

void CPartArray::DestroyLights()
{
    if (!m_pPhysicsObj || !m_pLightList)
        return;

    RemoveLightsFromCell(m_pPhysicsObj->m_ObjCell);

    if (m_pLightList)
        delete m_pLightList;

    m_pLightList = NULL;
}

BOOL CPartArray::InitLights()
{
    if (m_pPhysicsObj && (m_pPartSetup->m_LightCount > 0))
    {
        m_pLightList = new LIGHTLIST(m_pPartSetup->m_LightCount);

        for (DWORD i = 0; i < m_pLightList->m_LightCount; i++)
        {
            // Init the lights
            m_pLightList->m_Lights[i].m_LightInfo = &m_pPartSetup->m_Lights[i];

            if (FBitSet(m_pPhysicsObj->m_A8, 0))
                m_pLightList->m_Lights[i].m_Flags |= 1;
        }

        AddLightsToCell(m_pPhysicsObj->m_ObjCell); 
    }

    return TRUE;
}

void CPartArray::InitDefaults()
{
    if (m_pPartSetup->m_DefaultAnim)
    {
        m_Sequence.clear_animations();
        
        AnimData DefaultAnim;
        DefaultAnim.m_AnimID = m_pPartSetup->m_DefaultAnim;
        DefaultAnim.m_LowFrame = 0;
        DefaultAnim.m_HighFrame = 0xFFFFFFFF;
        DefaultAnim.m_PlaySpeed = 30.0f;

        m_Sequence.append_animation(&DefaultAnim);
    }

    if (m_pPhysicsObj)
        m_pPhysicsObj->InitDefaults(m_pPartSetup);
}

BOOL CPartArray::InitParts()
{
    m_iPartCount = m_pPartSetup->m_iObjectCount;

    if (!m_iPartCount)
        return FALSE;

    m_pPartObjs = new CPhysicsPart*[ m_iPartCount ];

    if (!m_pPartObjs)
        return FALSE;

    for (DWORD i = 0; i < m_iPartCount; i++)
        m_pPartObjs[i] = NULL;

    if (m_pPartSetup->m_pObjectIDs)
    {
        DWORD i;

        for (i = 0; i < m_iPartCount; i++)
        {
            m_pPartObjs[i] = CPhysicsPart::makePhysicsPart(m_pPartSetup->m_pObjectIDs[i]);

            if (!m_pPartObjs[i])
                break;
        }

        if (i != m_iPartCount)
            return FALSE;
        
        for (i = 0; i < m_iPartCount; i++)
        {
            m_pPartObjs[i]->m_PhysicsObj = m_pPhysicsObj;
            m_pPartObjs[i]->m_PartIndex = i;
        }

        if (m_pPartSetup->m_pObjectScales)
        {
            for (i = 0; i < m_iPartCount; i++)
                m_pPartObjs[i]->m_Scale = m_pPartSetup->m_pObjectScales[i];
        }
    }

    return TRUE;
}

BOOL CPartArray::SetPlacementFrame(DWORD ID)
{
    PlacementType *pt;
    
    pt = m_pPartSetup->m_Placements9C.lookup(ID);

    if (!pt)
    {
        pt = m_pPartSetup->m_Placements9C.lookup(0);

        if (!pt)
        {
            m_Sequence.set_placement_frame(0, 0);
            return FALSE;
        }
        else
            m_Sequence.set_placement_frame(&pt->m_AnimFrame, 0);
    }
    else
        m_Sequence.set_placement_frame(&pt->m_AnimFrame, ID);

    return TRUE;
}

void CPartArray::SetFrame(Frame *pFrame)
{
    UpdateParts(pFrame);

    if (m_pLightList)
        m_pLightList->set_frame(pFrame);
}

void CPartArray::SetTranslucencyInternal(float Amount)
{
    if (m_pPartSetup)
    {
        for (DWORD i = 0; i < m_iPartCount; i++)
        {
            CPhysicsPart *pPart = m_pPartObjs[i];
            
            if (pPart)
                pPart->SetTranslucency(Amount);
        }
    }
}

BOOL CPartArray::CacheHasPhysicsBSP()
{
    for (DWORD i = 0; i < m_iPartCount; i++)
    {
        if (m_pPartObjs[i]->m_GfxObjs[0]->m_ColTree)
        {
            m_dw00 |= 0x10000;
            return TRUE;
        }
    }

    m_dw00 &= ~(0x10000UL);
    return FALSE;
}

BOOL CPartArray::AllowsFreeHeading()
{
    return(m_pPartSetup->m_bType4);
}

float CPartArray::GetHeight()
{
    return(m_pPartSetup->m_fHeight * m_Scale.z);
}

float CPartArray::GetRadius()
{
    return(m_pPartSetup->m_fRadius * m_Scale.z);
}

void CPartArray::UpdateParts(Frame *pFrame)
{
    AnimFrame *CurrFrame = m_Sequence.get_curr_animframe();

    if (!CurrFrame)
        return;

    DWORD PartCount = min(CurrFrame->m_FrameCount, m_iPartCount);

    for (DWORD i = 0; i < PartCount; i++)
        m_pPartObjs[i]->m_Pos30.m_Frame.combine(pFrame, &CurrFrame->m_pFrames[i], &m_Scale);
}

void CPartArray::Update(float fTimeElapsed, Frame *pFrame)
{
    m_Sequence.update(fTimeElapsed, pFrame);
}

void CPartArray::UpdateViewerDistance(void)
{
    for (DWORD i = 0; i < m_iPartCount; i++)
    {
        if (m_pPartObjs[i])
            m_pPartObjs[i]->UpdateViewerDistance();
    }
}

void CPartArray::UpdateViewerDistance(float Distance, const Vector& Angle)
{
    for (DWORD i = 0; i < m_iPartCount; i++)
    {
        if (m_pPartObjs[i])
            m_pPartObjs[i]->UpdateViewerDistance(Distance, Angle);
    }
}

void CPartArray::SetNoDrawInternal(BOOL NoDraw)
{
    if (!m_pPartSetup)
        return;

    for (DWORD i = 0; i < m_iPartCount; i++)
    {
        CPhysicsPart *pPart = m_pPartObjs[i];

        if (pPart)
            pPart->SetNoDraw(NoDraw);
    }
}

void CPartArray::Draw(Position *Pos)
{
    if (m_pMush)
    {
        // Finish me.
        __asm int 3;
    }
    else
    {
        for (DWORD i = 0; i < m_iPartCount; i++)
        {
            CPhysicsPart *pPart = m_pPartObjs[i];

            if (pPart)
                pPart->Draw(FALSE);
        }
    }
}

DWORD CPartArray::GetDataID(void)
{
    if (m_pPartSetup->m_Key)
    {
        // Return Setup ID.
        return m_pPartSetup->m_Key;
    }
    else
    if (m_iPartCount == 1)
    {
        // Return Mesh ID.
        return m_pPartObjs[0]->m_GfxObjs[0]->m_Key;
    }
    else
        return 0;
}

LIGHTLIST::LIGHTLIST(DWORD LightCount)
{
    m_LightCount = LightCount;
    m_Lights = new LIGHTOBJ[ LightCount ];
}

LIGHTLIST::~LIGHTLIST()
{
    delete [] m_Lights;
}

void LIGHTLIST::set_frame(Frame *pFrame)
{
    for (DWORD i = 0; i < m_LightCount; i++)
        m_Lights[i].m_Frame = *pFrame;
}

LIGHTOBJ::LIGHTOBJ() : m_LightInfo(NULL)
{
    m_Flags = 0;
}

LIGHTOBJ::~LIGHTOBJ()
{
    // Finish me?
}




