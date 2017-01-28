
#pragma once

#include "Camera.h"
#include "Vertex.h"

class CLandBlock;
class CEnvCell;

class DungeonMapCamera : public OrthoCamera
{
    // Insert goodies here.
};

class DungeonMapScene :
    public RenderScene
{
public:
    DungeonMapScene();
    virtual ~DungeonMapScene();

public:
    // For camera (move this to abtract later?)
    virtual void Input_KeyDown(BYTE bKey);
    virtual void Input_KeyUp(BYTE bKey);

    void MoveCameraWithInput(float FrameTime, BOOL *Keys);

protected:

    void UpdateCamera();

    DungeonMapCamera m_Camera;

    double        m_LastCamUpdate;
    BOOL        m_Keys[ 256 ];

public:
    // For dungeon:
    void Destroy();
    BOOL Init(DWORD LandBlock);

    virtual int SceneType();
    virtual void Render(double);

    void CalcBBox();

protected:

    CLandBlock* m_LandBlock;
    CEnvCell**    m_EnvCells;
    DWORD        m_EnvCellCount;

    BBox        m_BBox;
};

