
//
// GameSky.cpp
//

#include "StdAfx.h"
#include "RegionDesc.h"
#include "EnvCell.h"
#include "GameSky.h"
#include "GameTime.h"
#include "PhysicsObj.h"

#define INCLUDE_SKY_CODE FALSE

GameSky::GameSky()
{
#if INCLUDE_SKY_CODE
    m_EnvCell24 = new CEnvCell;
    m_EnvCell28 = new CEnvCell;
#endif
}

GameSky::~GameSky()
{
#if INCLUDE_SKY_CODE
    // Missing code

    if (m_EnvCell24)
        delete m_EnvCell24;
    if (m_EnvCell28)
        delete m_EnvCell28;
#endif
}

void GameSky::UseTime(void)
{
#if INCLUDE_SKY_CODE
    // MISSING
    // CRegionDesc::CalcDayGroup();

    double time = (GameTime::current_game_time ? GameTime::current_game_time->m_48 : 0.0);

    if (CRegionDesc::GetSky(time, this))
    {
        CreateDeletePhysicsObjects();

        for (DWORD i = 0; i < m_SkyDescs.num_used; i++)
        {
            CPhysicsObj *    object    = m_Objs0C.array_data[i];
            SkyDesc *        skydesc    = &m_SkyDescs.array_data[i];

            if (Object = m_Objs0C.array_data[i])
            {
                Frame drawframe;
                CalcDrawFrame(&drawframe, skydesc->m_DrawVec);

                Object->set_initial_frame(&drawframe);

                // Update luminosity, diffusion, and translucency..
            }
        }
    }
#endif
}

CPhysicsObj *GameSky::MakeObject(DWORD ModelID, Vec2D& TexVel, DWORD arg_8)
{
#if INCLUDE_SKY_CODE
    if (!ModelID)
        return NULL;

    CPhysicsObj *Object;

    if (Object = CPhysicsObj::makeObject(ModelID, 0, 0))
    {
        if (TexVel.x != 0 || TexVel.y != 0)
        {
            // Missing code.
            // Object->SetTextureVelocity(TexVel);
        }

        Object->AddObjectToSingleCell(arg_8 ? m_EnvCell28 : m_EnvCell24);

        if (ImgTex::DoChunkification())
            UNFINISHED("Object->NotifySurfaceTiles()");
    }

    return Object;
#else
    return NULL;
#endif
}

SkyDesc::SkyDesc()
{
}

SkyDesc::~SkyDesc()
{
}






















