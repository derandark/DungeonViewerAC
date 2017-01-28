
#pragma once

#include "Camera.h"

class CPhysics;
class CLandBlock;
class CEnvCell;

class DungeonCamera : public FreeViewCam
{
    // Insert goodies here.
};

class DungeonScene :
    public RenderScene
{
public:
    DungeonScene();
    virtual ~DungeonScene();

    // **** For camera (move this to abtract later?)
public: 
    virtual void Input_KeyDown(BYTE bKey);
    virtual void Input_KeyUp(BYTE bKey);
    virtual void Input_MouseMove(long x, long y);

    void MoveCameraWithInput(float FrameTime, BOOL *Keys);

protected:

    void UpdateCamera();

    DungeonCamera    m_Camera;

    double            m_LastCamUpdate;
    BOOL            m_Keys[ 256 ];

    // **** End of camera

    // **** For dungeon scene
public:
    void Destroy();
    BOOL Init(DWORD LandBlock);

    virtual int SceneType();
    virtual void Render(double);

protected:
    
    static int SortCellCompare(const void *, const void *);
    static Vector camera_pos;

    void SortCells();

    CPhysics*    m_Physics;
    CLandBlock* m_LandBlock;
    CEnvCell**    m_EnvCells;
    DWORD        m_EnvCellCount;
    CEnvCell**    m_SortedCells;
    DWORD        m_CurrentCell;

    // **** End of dungeon scene
};


