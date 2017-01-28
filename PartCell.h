
#pragma once
#include "DArray.h"

class ClipPlaneList;
class Frame;
class CPhysicsPart;

class CShadowPart
{
public:
    CShadowPart(DWORD CPListCount, ClipPlaneList** CPLists, Frame* CPListFrame, CPhysicsPart *pPart);
    ~CShadowPart();

    void draw(DWORD ClipPlaneIndex);

    DWORD            m_CPListCount; // 0x00
    ClipPlaneList**    m_CPLists; // 0x04
    Frame*            m_CPListFrame; // 0x08
    CPhysicsPart*    m_PhysicsPart; // 0x0C
};

class CPartCell
{
public:
    CPartCell();
    virtual ~CPartCell();

    // virtual add_part
    // virtual remove_part

    DWORD        m_ShadowPartCount;        // 0x04
    DArray<CShadowPart *>
                m_ShadowParts;             // 0x08
};