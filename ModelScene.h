
#pragma once
#include "Camera.h"

class ModelCamera : public BaseCamera
{
    // Insert goodies here.
};


class ModelScene :
    public RenderScene
{
public:
    ModelScene();
    virtual ~ModelScene();

    virtual void Input_KeyDown(BYTE bKey);
    virtual void Input_KeyUp(BYTE bKey);

    void MoveCameraWithInput(float FrameTime, BOOL *Keys);

protected:

    void UpdateCamera();

    ModelCamera m_Camera;

    double        m_LastCamUpdate;
    BOOL        m_Keys[ 256 ];

};


