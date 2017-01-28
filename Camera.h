
#pragma once

#include "MathLib.h"
#include "Frame.h"

// Camera axes
#define CAMERA_FORWARD        2
#define CAMERA_RIGHT        0
#define CAMERA_UP            1

// Camera constraints
#define MIN_PITCH    -90.0f
#define MAX_PITCH    90.0f

class Position;
class Frame;

class BaseCamera
{
public:
    BaseCamera(float fov=90.0f, float znear=0.01f, float zfar=100.0f, float scale=4000.0f);
    ~BaseCamera();

    void    SetFOV(float degs);
    void    SetZNear(float dist);
    void    SetZFar(float dist);

    void    Rotate(const Vector& Angles);
    void    Move(const Vector& Offset);

    void    SetViewPosition(const Frame& frame);
    void    SetViewTarget(const Vector& Target);

    D3DXMATRIX*        GetProjMatrix();
    D3DXMATRIX*        GetViewMatrix();
    Frame*            GetViewFrame();
    Vector*            GetViewPoint();

private:

    void UpdateBoxSize();

    // Update matrices
    void RebuildProjMatrix();
    void RebuildViewMatrix();
    void RebuildViewMatrixSW();
    void RebuildViewMatrixOld();

    // Projection parameters
    float m_FOV;
    float m_ZNear;
    float m_ZFar;
    float m_Scale;

    // Internal projection states.
    float m_XInvScale;
    float m_YInvScale;
    float m_TX;
    float m_TY;
    float m_VDst;

    // Window ratio
    float m_3DWidth;
    float m_3DHeight;
    
    // Viewer state.
    Frame    m_ViewFrame;
    Vector    m_ViewPoint;

    float m_Pitch;
    float m_Yaw;
    float m_Roll;

    // The end product.
    D3DXMATRIX m_mProj;
    D3DXMATRIX m_mView;
};

#define ORTHOCAM_HEIGHT        1000.0f
#define ORTHOCAM_MINZNEAR    0.1f
#define ORTHOCAM_MAXZFAR    2000.0f

class OrthoCamera
{
public:
    OrthoCamera(float znear=ORTHOCAM_MINZNEAR, float zfar=ORTHOCAM_MAXZFAR, float scale=1.0f);
    ~OrthoCamera();

    void Move(const Vec2D& offset);
    void AdjustZLimits(float ZNearMove, float ZFarMove);
    void AdjustZoom(float ZoomDir);

    void SetZLimits(float ZNear, float ZFar);

    D3DXMATRIX* GetProjMatrix();
    D3DXMATRIX* GetViewMatrix();
    Frame*        GetViewFrame();
    Vector*        GetViewPoint();
    Vec2D*        GetViewOffset();

private:

    void UpdateBoxSize();

    // Update matrices
    void RebuildProjMatrix();
    void RebuildViewMatrix();

    // Projection parameters
    float m_ZNear;
    float m_ZFar;
    float m_Scale;

    // Window ratio
    float m_3DWidth;
    float m_3DHeight;
    
    // Viewer state.
    Vec2D m_ViewOffset;

    // Combined frame
    Vector m_ViewPoint;
    Frame m_ViewFrame;

    // The end product.
    D3DXMATRIX m_mProj;
    D3DXMATRIX m_mView;
};

#define FREEVIEW_MIN_PITCH    -90
#define FREEVIEW_MAX_PITCH     90
#define FREEVIEW_MIN_ROLL    -180
#define FREEVIEW_MAX_ROLL     180

class FreeViewCam
{
public:
    FreeViewCam(float fov=90.0f);
    ~FreeViewCam();

    void    SetFOV(float degs);
    void    SetZNear(float dist);
    void    SetZFar(float dist);

    void    Rotate(const Vector& angles);
    void    Move(const Vector& dir);

    void    SetViewPosition(const Frame& frame);
    void    SetViewTarget(const Position& target);

    Frame*            GetViewFrame();
    Vector*            GetViewPoint();
    Vector*            GetViewAngles();

private:

    void UpdateProj();
    void UpdateView();

    // Projection parameters
    float m_FOV;

    // Viewer state.
    Frame    m_ViewFrame;
    Vector    m_ViewPoint;
    Vector    m_ViewAngles;
};