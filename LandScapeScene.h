
#pragma once

#include "Camera.h"

class LScape;

class LandScapeCamera : public FreeViewCam
{
    // Insert goodies here.
};

class LandScapeScene :
    public RenderScene
{
public:
    LandScapeScene();
    virtual ~LandScapeScene();

    // **** For camera (move this to abtract later?)
public: 
    virtual void Input_KeyDown(BYTE bKey);
    virtual void Input_KeyUp(BYTE bKey);
    virtual void Input_MouseMove(long x, long y);

    void MoveCameraWithInput(float FrameTime, BOOL *Keys);

protected:

    void UpdateCamera();

    LandScapeCamera    m_Camera;

    double            m_LastCamUpdate;
    BOOL            m_Keys[ 256 ];

    // **** End of camera

    // **** For LandScape scene
public:
    void Destroy();
    BOOL Init(DWORD LandBlock);

    virtual int SceneType();
    virtual void Render(double);

protected:
    
    static Vector camera_pos;

    LScape*    m_LandScape;
    DWORD m_CurrentGid;

    // **** End of LandScape scene
};
