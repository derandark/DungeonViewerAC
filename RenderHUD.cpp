
#include "StdAfx.h"
#include "MainDialog.h"
#include "Render.h"
#include "RenderHUD.h"
#include "SDDialog.h"

RenderHUD::RenderHUD()
{
}

RenderHUD::~RenderHUD()
{
}

void RenderHUD::Draw()
{
}

PrimaryHUD::PrimaryHUD()
{
}

PrimaryHUD::~PrimaryHUD()
{
}

char debug_status_string[ 512 ] = { '\0' };

void PrimaryHUD::Draw()
{
#ifdef _DEBUG
#define SHOW_DEBUG_STRING
#else
// show it anyways?
#define SHOW_DEBUG_STRING
#endif

    char hud_string[ 256 ];
    hud_string[255] = 0;

    _snprintf(hud_string, 255,
        // Generic HUD Text formatting:
		  "DungeonID: %s\n"                                                // Dungeon ID
        "FPS: %1.1f\n"                                                // Frames Per Second
        "Polygons: %u\n"                                // Tex Mem / Scene Polygon Count
        "Lights: %u\n"
        // "Selection: %u %u, %1.1f %1.1f %1.1f %1.2f\n"
        "%s\n",                                                        // Debug String

        // Generic HUD Text data:
        SDDialog::GetCurrentDungeonLabel(),                        // Dungeon ID
        Render::GetFPS(),                                            // Frames Per Second
        // g_pD3DDevice->GetAvailableTextureMem() / (1024 * 1024),        // Available Texture Memory
        Render::GetFramePolyCount(),                                // Scene Polygon Count
        Render::pLightManager->m_EnabledLights,
        // Render::m_SelectionX,
        // Render::m_SelectionY,
        // Render::m_SelectionRay.x,
        // Render::m_SelectionRay.y,
        // Render::m_SelectionRay.z,
        // (Render::m_CurrDepth != FLT_MAX)? Render::m_CurrDepth : 0,
#ifdef SHOW_DEBUG_STRING
        debug_status_string                                            // Debug String
#else                                                                // .. or ..
        ""                                                            // Null String
#endif
        );

    Render::DrawString(8, 16, hud_string);
}








