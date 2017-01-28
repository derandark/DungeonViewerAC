
#include "StdAfx.h"
#include "Render.h"
#include "Frame.h"
#include "Camera.h"
#include "LandDefs.h"

BaseCamera::BaseCamera(float fov, float znear, float zfar, float scale)
{
    m_FOV = fov;
    m_ZNear = znear;
    m_ZFar = zfar;
    m_Scale = scale;

    m_ViewPoint = Vector(0.0f, 0.0f, 0.0f);
    m_Pitch = m_Yaw = m_Roll = 0;

    // Update projection
    RebuildProjMatrix();
    RebuildViewMatrix();
}

BaseCamera::~BaseCamera()
{
}

void BaseCamera::SetFOV(float degs)
{
    m_FOV = degs;

    // Update projection
    RebuildProjMatrix();
}

void BaseCamera::SetZNear(float dist)
{
    m_ZNear = dist;

    // Update view & projection
    RebuildProjMatrix();
    RebuildViewMatrix();
}

void BaseCamera::SetZFar(float dist)
{
    m_ZFar = dist;

    // Update view & projection
    RebuildProjMatrix();
    RebuildViewMatrix();
}

void BaseCamera::UpdateBoxSize()
{
    RECT WindowRect = Render::GetRenderRect();

    m_3DWidth = (float)(WindowRect.right - WindowRect.left);
    m_3DHeight = (float)(WindowRect.bottom - WindowRect.top);
}

void BaseCamera::RebuildProjMatrix()
{
    Render::SetFOV(m_FOV);
    return;

    UpdateBoxSize();

    float fov_radian = D3DXToRadian(m_FOV);

    m_XInvScale = 1 / m_Scale;
    m_YInvScale = 1 / m_Scale;
    m_TX = m_XInvScale * 0.5f * (m_3DWidth - 1);
    m_TY = m_YInvScale * 0.5f * (m_3DHeight - 1);
    m_VDst = m_TX / tan(fov_radian * 0.5);
    float wtf = 1 / tan(fov_radian * 0.5);
    
    m_mProj._11 = 1.0f;
    m_mProj._21 = m_mProj._31 = m_mProj._41 = 0.0f;
    m_mProj._12 = m_mProj._32 = m_mProj._42 = 0.0f;
    m_mProj._13 = m_mProj._23 = 0.0f;
    m_mProj._14 = m_mProj._24 = m_mProj._44 = 0.0f;

    m_mProj._22 = m_3DWidth / m_3DHeight;
    m_mProj._33 = m_ZFar / (m_ZFar - m_VDst);
    m_mProj._43 = -wtf;
    m_mProj._34 = 1 / wtf;
}

void BaseCamera::RebuildViewMatrix()
{
    Render::update_viewpoint(m_ViewFrame);
    return;

    RebuildViewMatrixSW();
}

void BaseCamera::RebuildViewMatrixOld()
{
    Vector Xaxis, Yaxis, Zaxis;
    
    Xaxis.x = m_ViewFrame._11;
    Xaxis.y = m_ViewFrame._12;
    Xaxis.z = m_ViewFrame._13;

    Yaxis.x = m_ViewFrame._21;
    Yaxis.y = m_ViewFrame._22;
    Yaxis.z = m_ViewFrame._23;

    Zaxis.x = m_ViewFrame._31;
    Zaxis.y = m_ViewFrame._32;
    Zaxis.z = m_ViewFrame._33;

    Quaternion Unknown;

    Unknown.x = Yaxis.x;
    Unknown.y = Yaxis.y;
    Unknown.z = Yaxis.z;
    Unknown.w = -((m_ViewFrame.m_origin.x * Yaxis.x) + (m_ViewFrame.m_origin.y * Yaxis.y) +    (m_ViewFrame.m_origin.z * Yaxis.z)) - m_ZNear;

    Vector ViewPoint = m_ViewFrame.globaltolocal(Vector(0, 0, 0));

    m_mView._11 = Xaxis.x;
    m_mView._21 = Xaxis.z;
    m_mView._31 = Xaxis.y;
    m_mView._41 = ViewPoint.x;

    m_mView._12 = Zaxis.x;
    m_mView._22 = Zaxis.z;
    m_mView._32 = Zaxis.y;
    m_mView._42 = ViewPoint.z;

    m_mView._13 = Yaxis.x;
    m_mView._23 = Yaxis.z;
    m_mView._33 = Yaxis.y;
    m_mView._43 = ViewPoint.y;

    m_mView._14 = 0.0f;
    m_mView._24 = 0.0f;
    m_mView._34 = 0.0f;
    m_mView._44 = 1.0f;

    m_ViewPoint = m_ViewFrame.m_origin;
}

void BaseCamera::RebuildViewMatrixSW()
{
    Vector Xaxis, Yaxis, Zaxis;
    Xaxis.x = m_ViewFrame._11;
    Xaxis.y = m_ViewFrame._12;
    Xaxis.z = m_ViewFrame._13;

    Yaxis.x = m_ViewFrame._21;
    Yaxis.y = m_ViewFrame._22;
    Yaxis.z = m_ViewFrame._23;

    Zaxis.x = m_ViewFrame._31;
    Zaxis.y = m_ViewFrame._32;
    Zaxis.z = m_ViewFrame._33;

    Vector ViewPoint = m_ViewFrame.globaltolocal(Vector(0, 0, 0));

    double inv_vdst = 1 / m_VDst;
    m_mView._11 = (0.5 * m_3DWidth * Yaxis.x * inv_vdst) + (Xaxis.x * m_Scale);
    m_mView._21 = (0.5 * m_3DHeight * Yaxis.x * inv_vdst) - (Zaxis.x * m_Scale);
    m_mView._31 = Yaxis.x * inv_vdst;
    m_mView._41 = Yaxis.x * inv_vdst;

    m_mView._12 = (0.5 * m_3DWidth * Yaxis.y * inv_vdst) + (Xaxis.y * m_Scale);
    m_mView._22 = (0.5 * m_3DHeight * Yaxis.y * inv_vdst) - (Zaxis.y * m_Scale);
    m_mView._32 = Yaxis.y * inv_vdst;
    m_mView._42 = Yaxis.y * inv_vdst;

    m_mView._13 = (0.5 * m_3DWidth * Yaxis.z * inv_vdst) + (Xaxis.z * m_Scale);
    m_mView._23 = (0.5 * m_3DHeight * Yaxis.z * inv_vdst) - (Zaxis.z * m_Scale);
    m_mView._33 = Yaxis.z * inv_vdst;
    m_mView._43 = Yaxis.z * inv_vdst;

    m_mView._14 = (0.5 * m_3DWidth * ViewPoint.y * inv_vdst) + (ViewPoint.x * m_Scale);
    m_mView._24 = (0.5 * m_3DHeight * ViewPoint.y * inv_vdst) - (ViewPoint.z * m_Scale);
    m_mView._34 = (m_ZNear + ViewPoint.y) * inv_vdst;
    m_mView._44 = ViewPoint.y * inv_vdst;

    m_ViewPoint = m_ViewFrame.m_origin;
}

void BaseCamera::SetViewPosition(const Frame& frame)
{
    m_ViewFrame = frame;

    RebuildViewMatrix();
}

/*
void BaseCamera::SetViewPosition2(const Position& Pos)
{
    Frame viewer_frame = Pos.m_Frame;

    Vector Xaxis, Yaxis, Zaxis;
    
    Xaxis.x = viewer_frame._11;
    Xaxis.y = viewer_frame._12;
    Xaxis.z = viewer_frame._13;

    Yaxis.x = viewer_frame._21;
    Yaxis.y = viewer_frame._22;
    Yaxis.z = viewer_frame._23;

    Zaxis.x = viewer_frame._31;
    Zaxis.y = viewer_frame._32;
    Zaxis.z = viewer_frame._33;

    Quaternion Unknown;

    Unknown.x = Yaxis.x;
    Unknown.y = Yaxis.y;
    Unknown.z = Yaxis.z;
    Unknown.w = -((Pos.m_Frame.m_origin.x * Yaxis.x) + (Pos.m_Frame.m_origin.y * Yaxis.y) +    (Pos.m_Frame.m_origin.z * Yaxis.z)) - m_ZNear;

    Vector Offset(0, 0, 0);
    viewer_frame.globaltolocal(&m_ViewPoint, Offset);

    double inv_vdst = 1 / m_VDst;

    m_mView._11 = (0.5 * m_3DWidth * Yaxis.x * inv_vdst) + (Xaxis.x * m_Scale);
    m_mView._21 = (0.5 * m_3DHeight * Yaxis.x * inv_vdst) - (Zaxis.x * m_Scale);
    m_mView._31 = Yaxis.x * inv_vdst;
    m_mView._41 = Yaxis.x * inv_vdst;

    m_mView._12 = (0.5 * m_3DWidth * Yaxis.y * inv_vdst) + (Xaxis.y * m_Scale);
    m_mView._22 = (0.5 * m_3DHeight * Yaxis.y * inv_vdst) - (Zaxis.y * m_Scale);
    m_mView._32 = Yaxis.y * inv_vdst;
    m_mView._42 = Yaxis.y * inv_vdst;

    m_mView._13 = (0.5 * m_3DWidth * Yaxis.z * inv_vdst) + (Xaxis.z * m_Scale);
    m_mView._23 = (0.5 * m_3DHeight * Yaxis.z * inv_vdst) - (Zaxis.z * m_Scale);
    m_mView._33 = Yaxis.z * inv_vdst;
    m_mView._43 = Yaxis.z * inv_vdst;

    m_mView._14 = (0.5 * m_3DWidth * m_ViewPoint.y * inv_vdst) + (m_ViewPoint.x * m_Scale);
    m_mView._24 = (0.5 * m_3DHeight * m_ViewPoint.y * inv_vdst) - (m_ViewPoint.z * m_Scale);
    m_mView._34 = (m_ZNear + m_ViewPoint.y) * inv_vdst;
    m_mView._44 = m_ViewPoint.y * inv_vdst;
}
*/

void BaseCamera::Rotate(const Vector& Angles)
{
    m_ViewFrame.rotate(Angles);

    RebuildViewMatrix();
}

void BaseCamera::Move(const Vector& Offset)
{
    AFrame MoveFrame;
    MoveFrame.m_origin = Offset;
    MoveFrame.m_angles = Quaternion(1, 0, 0, 0);

    m_ViewFrame.combine(&m_ViewFrame, &MoveFrame);

    RebuildViewMatrix();
}

D3DXMATRIX* BaseCamera::GetProjMatrix()
{
    return &m_mProj;
}

D3DXMATRIX* BaseCamera::GetViewMatrix()
{
    return &m_mView;
}

Frame* BaseCamera::GetViewFrame()
{
    return &m_ViewFrame;
}

Vector *BaseCamera::GetViewPoint()
{
    return &m_ViewPoint;
}

void BaseCamera::SetViewTarget(const Vector &ViewTarget)
{
    /*
    Vector ViewForward = ViewTarget - m_ViewPoint;
    ViewForward.normalize();

    float fYaw;

    if (ViewForward.y != 0)
    {    
        fYaw = D3DXToDegree(atan(ViewForward.x / ViewForward.y));

        if (ViewForward.y < 0)
            fYaw += 180;
    }
    else if (ViewForward.x < 0)
        fYaw = 270;
    else
        fYaw = 90;

    SetYaw(fYaw);

    float fPitch;

    if (ViewForward.y != 0)
    {    
        fPitch = D3DXToDegree(atan(ViewForward.z / ViewForward.y));

        if (ViewForward.y > 0)
            fPitch = -fPitch;
    }
    else if (ViewForward.z < 0)
        fPitch = 90;
    else
        fPitch = -90;

    SetPitch(fPitch);

    RebuildViewMatrix();
    */
}

OrthoCamera::OrthoCamera(float znear, float zfar, float scale)
{
    m_ZNear = znear;
    m_ZFar = zfar;
    m_Scale = scale;

    // Set offset at 0, 0
    m_ViewOffset = Vec2D(0.0, 0.0);
    m_ViewPoint = Vector(0.0, 0.0, ORTHOCAM_HEIGHT);
    m_ViewFrame.m_origin = Vector(0.0, 0.0, ORTHOCAM_HEIGHT);
    m_ViewFrame.cache();

    // Look straight down
    m_ViewFrame.rotate(Vector(D3DXToRadian(-90.0f), 0, 0));

    // Update projection
    RebuildProjMatrix();
    RebuildViewMatrix();
}

OrthoCamera::~OrthoCamera()
{
}

void OrthoCamera::AdjustZLimits(float ZNear, float ZFar)
{
    m_ZNear += ZNear;
    m_ZFar += ZFar;
    
    if (m_ZNear < ORTHOCAM_MINZNEAR)
        m_ZNear = ORTHOCAM_MINZNEAR;
    if (m_ZFar > ORTHOCAM_MAXZFAR)
        m_ZFar = ORTHOCAM_MAXZFAR;

    if (m_ZNear > m_ZFar)
        m_ZNear = m_ZFar;
    else
    if (m_ZFar < m_ZNear)
        m_ZFar = m_ZNear;

    // Update view & projection
    RebuildProjMatrix();
    RebuildViewMatrix();
}

void OrthoCamera::SetZLimits(float ZNear, float ZFar)
{
    m_ZNear = ZNear;
    m_ZFar = ZFar;
    
    if (m_ZNear < ORTHOCAM_MINZNEAR)
        m_ZNear = ORTHOCAM_MINZNEAR;
    if (m_ZFar > ORTHOCAM_MAXZFAR)
        m_ZFar = ORTHOCAM_MAXZFAR;

    // Update view & projection
    RebuildProjMatrix();
    RebuildViewMatrix();
}

void OrthoCamera::UpdateBoxSize()
{
    RECT WindowRect = Render::GetRenderRect();

    m_3DWidth = (float)(WindowRect.right - WindowRect.left);
    m_3DHeight = (float)(WindowRect.bottom - WindowRect.top);
}

void OrthoCamera::RebuildProjMatrix()
{
    UpdateBoxSize();

    float Aspect = m_3DWidth / m_3DHeight;
    float Zoom = m_Scale * 100;

    D3DXMatrixOrthoLH(&m_mProj, Zoom * Aspect, Zoom, m_ZNear, m_ZFar);
}

void OrthoCamera::RebuildViewMatrix()
{
    Vector Xaxis, Yaxis, Zaxis;
    
    Xaxis.x = m_ViewFrame._11;
    Xaxis.y = m_ViewFrame._12;
    Xaxis.z = m_ViewFrame._13;

    Yaxis.x = m_ViewFrame._21;
    Yaxis.y = m_ViewFrame._22;
    Yaxis.z = m_ViewFrame._23;

    Zaxis.x = m_ViewFrame._31;
    Zaxis.y = m_ViewFrame._32;
    Zaxis.z = m_ViewFrame._33;

    Quaternion Unknown;

    Unknown.x = Yaxis.x;
    Unknown.y = Yaxis.y;
    Unknown.z = Yaxis.z;
    Unknown.w = -((m_ViewFrame.m_origin.x * Yaxis.x) + (m_ViewFrame.m_origin.y * Yaxis.y) +    (m_ViewFrame.m_origin.z * Yaxis.z)) - m_ZNear;

    Vector ViewPoint = m_ViewFrame.globaltolocal(Vector(0, 0, 0));

    m_mView._11 = Xaxis.x;
    m_mView._21 = Xaxis.z;
    m_mView._31 = Xaxis.y;
    m_mView._41 = ViewPoint.x;

    m_mView._12 = Zaxis.x;
    m_mView._22 = Zaxis.z;
    m_mView._32 = Zaxis.y;
    m_mView._42 = ViewPoint.z;

    m_mView._13 = Yaxis.x;
    m_mView._23 = Yaxis.z;
    m_mView._33 = Yaxis.y;
    m_mView._43 = ViewPoint.y;

    m_mView._14 = 0.0f;
    m_mView._24 = 0.0f;
    m_mView._34 = 0.0f;
    m_mView._44 = 1.0f;

    m_ViewPoint = m_ViewFrame.m_origin;
}

void OrthoCamera::Move(const Vec2D& Offset)
{
    m_ViewOffset += Offset;

    m_ViewFrame.m_origin = Vector(m_ViewOffset.x, m_ViewOffset.y, ORTHOCAM_HEIGHT);
    m_ViewFrame.cache();

    RebuildViewMatrix();
}

void OrthoCamera::AdjustZoom(float ZoomDir)
{
    if (!ZoomDir)
        return;

    if (ZoomDir < -0.3f)
        ZoomDir = -0.3f;
    if (ZoomDir > 0.3f)
        ZoomDir = 0.3f;

    m_Scale -= ZoomDir; 
    RebuildProjMatrix();
}

D3DXMATRIX* OrthoCamera::GetProjMatrix()
{
    return &m_mProj;
}

D3DXMATRIX* OrthoCamera::GetViewMatrix()
{
    return &m_mView;
}

Frame* OrthoCamera::GetViewFrame()
{
    return &m_ViewFrame;
}

Vec2D *OrthoCamera::GetViewOffset()
{
    return &m_ViewOffset;
}

Vector *OrthoCamera::GetViewPoint()
{
    return &m_ViewPoint;
}

/* *****************************
        FREE VIEW camera
   ***************************** */

FreeViewCam::FreeViewCam(float fov)
{
    m_FOV = fov;

    m_ViewAngles = Vector(0, 0, 0);
    m_ViewPoint = Vector(0, 0, 0);

    UpdateProj();
    UpdateView();
}

FreeViewCam::~FreeViewCam()
{
}

void FreeViewCam::SetFOV(float degs)
{
    m_FOV = degs;
    UpdateProj();
}

void FreeViewCam::UpdateProj(void)
{
    Render::SetFOV(m_FOV);
}

void FreeViewCam::UpdateView(void)
{
    Render::update_viewpoint(m_ViewFrame);
}

void FreeViewCam::SetViewPosition(const Frame& frame)
{
    m_ViewFrame = frame;
    UpdateView();
}

void FreeViewCam::SetViewTarget(const Position& target)
{
    Position NullCell;
    NullCell.m_Frame = m_ViewFrame;
    
    Frame HeadingFrame;
    HeadingFrame.set_vector_heading(NullCell.localtolocal(target, Vector(0, 0, 0)));

    m_ViewAngles = Vector(0, 0, 0);

    Rotate(Vector(0, HeadingFrame.get_heading(), 0));
}

void FreeViewCam::Rotate(const Vector& angles)
{
    m_ViewAngles += angles;

    if (m_ViewAngles.x > FREEVIEW_MAX_PITCH)
        m_ViewAngles.x = FREEVIEW_MAX_PITCH;
    else
    if (m_ViewAngles.x < FREEVIEW_MIN_PITCH)
        m_ViewAngles.x = FREEVIEW_MIN_PITCH;

    if (m_ViewAngles.y >= 180)
        m_ViewAngles.y -= 360;
    else
    if (m_ViewAngles.y < -180)
        m_ViewAngles.y += 360;

    if (m_ViewAngles.z > FREEVIEW_MAX_ROLL)
        m_ViewAngles.z = FREEVIEW_MAX_ROLL;
    else
    if (m_ViewAngles.z < FREEVIEW_MIN_ROLL)
        m_ViewAngles.z = FREEVIEW_MIN_ROLL;

    Vector RadianAngles;

    RadianAngles.x = D3DXToRadian(m_ViewAngles.x);
    RadianAngles.y = D3DXToRadian(m_ViewAngles.z); // Invert Y/Z
    RadianAngles.z = D3DXToRadian(m_ViewAngles.y);

    m_ViewFrame.euler_set_rotate(RadianAngles, 0);

    // m_ViewFrame.rotate(angles);
    UpdateView();
}

void FreeViewCam::Move(const Vector& dir)
{
    // Translate origin relative to our current angles.
    m_ViewFrame.m_origin += m_ViewFrame.localtoglobalvec(dir);
    UpdateView();
}

Frame* FreeViewCam::GetViewFrame()
{
    return &m_ViewFrame;
}

Vector *FreeViewCam::GetViewPoint()
{
    return &m_ViewPoint;
}

Vector *FreeViewCam::GetViewAngles()
{
    return &m_ViewAngles;
}