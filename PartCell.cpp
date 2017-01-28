
#include "StdAfx.h"
#include "PartCell.h"

CPartCell::CPartCell() :
    m_ShadowPartCount(0), m_ShadowParts(128)
{
}

CPartCell::~CPartCell()
{
}

CShadowPart::CShadowPart(DWORD CPListCount, ClipPlaneList** CPLists, Frame* CPListFrame, CPhysicsPart *pPart)
{
    m_CPListCount    = CPListCount;
    m_CPLists        = CPLists;
    m_CPListFrame    = CPListFrame;
    m_PhysicsPart    = pPart;
}

CShadowPart::~CShadowPart()
{
    if (m_CPLists)
    {
        for (DWORD i = 0; i < m_CPListCount; i++)
        {
            if (m_CPLists[i])
                delete m_CPLists[i];
        }

        delete [] m_CPLists;
    }
}

void CShadowPart::draw(DWORD ClipPlaneIndex)
{
    // Finish me.
    __asm int 3;
}





