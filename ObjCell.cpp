
#include "StdAfx.h"
#include "ObjCell.h"
#include "PartArray.h"
#include "PhysicsObj.h"
#include "Render.h"

CObjCell::CObjCell() : DBObj(NULL),
    m_ObjectCount(0), m_Objects(128),
    m_LightCount(0), m_Lights(128),
    m_ShadowObjCount(0), m_ShadowObjs(4, 128)
{
    m_C8 = 0;
    m_CC = 0;
    m_PVCellCount = 0;
    m_PVCells = NULL;
    m_D8 = FALSE;
    m_DC = 0;
    m_LandBlock = NULL;
}

CObjCell::~CObjCell()
{
    if (m_DC)
    {
        delete m_DC;
        m_DC = NULL;
    }

    // MISSING CODE HERE --

    m_ShadowObjCount = 0;
}

void CObjCell::add_object(CPhysicsObj *pObject)
{
    if (m_ObjectCount >= m_Objects.alloc_size)
        m_Objects.grow(m_Objects.alloc_size + 5);

    m_Objects.array_data[ m_ObjectCount++ ] = pObject;

    // MISSING CODE (Update detections)
}

void CObjCell::remove_object(CPhysicsObj *pObject)
{
    for (DWORD i = 0; i < m_ObjectCount; i++)
    {
        if (pObject == m_Objects.array_data[i])
        {
            // DEBUGOUT("Removing index %u object (total was %u)\r\n", i, m_ObjectCount);
            m_Objects.array_data[i] = m_Objects.array_data[ --m_ObjectCount ];

            // MISSING CODE HERE
            // update_all_voyeur

            if ((m_ObjectCount+10) < m_Objects.alloc_size)
                m_Objects.shrink(m_ObjectCount+5);

            break;
        }
    }
}

void CObjCell::add_light(LIGHTOBJ *Light)
{
    // if (m_Position.m_LandCell != 0x0120010E)
    //     return;

    if (m_LightCount >= m_Lights.alloc_size)
        m_Lights.grow(m_Lights.alloc_size + 5);

    m_Lights.array_data[ m_LightCount++ ] = Light;

    // DEBUGOUT("Adding light(Mem@%08X) on cell %08X.\r\n", (DWORD)Light, m_Position.m_LandCell);
    Render::pLightManager->AddLight(Light, &m_Position);
}

void CObjCell::remove_light(LIGHTOBJ *Light)
{
    for (DWORD i = 0; i < m_LightCount; i++)
    {
        if (Light == m_Lights.array_data[i])
        {
            // DEBUGOUT("Removing index %u light (total was %u)\r\n", i, m_LightCount);
            m_Lights.array_data[i] = m_Lights.array_data[ --m_LightCount ];

            if ((m_LightCount+10) < m_Lights.alloc_size)
                m_Lights.shrink(m_LightCount+5);

            Render::pLightManager->RemoveLight(Light);
            break;
        }
    }
}

CShadowObj::CShadowObj()
{
    m_PhysObj    = NULL;
    m_CellID    = 0;
    m_14        = 0;
}

CShadowObj::~CShadowObj()
{
}

void CShadowObj::set_physobj(CPhysicsObj *pObject)
{
    m_PhysObj    = pObject;
    m_Key        = pObject->m_IID;
}








