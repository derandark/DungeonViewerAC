
//
// GameSky.h
//

#pragma once
#include "MathLib.h"
#include "SmartArray.h"

class CEnvCell;
class CPhysicsObj;

class SkyDesc // 0x28 in size.
{
public:
    SkyDesc();
    ~SkyDesc();

    Vec2D m_DrawVec; // 0x04, 0x08
};

class GameSky
{
public:
    GameSky();
    ~GameSky();

    CPhysicsObj *    MakeObject(DWORD ModelID, Vec2D& TexVel, DWORD arg_8);
    void            UseTime(void);

    SmartArray<SkyDesc> m_SkyDescs; // 0x00
    SmartArray<CPhysicsObj *> m_Objs0C; // 0x0C
    SmartArray<CPhysicsObj *> m_Objs18; // 0x18
    CEnvCell *m_EnvCell24; // 0x24
    CEnvCell *m_EnvCell28; // 0x28
};
