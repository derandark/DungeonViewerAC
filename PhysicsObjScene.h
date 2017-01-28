
#pragma once

class CPhysicsObj;

class PhysicsObjScene :
    public ModelScene
{
public:
    PhysicsObjScene();
    virtual ~PhysicsObjScene();

    void Destroy();
    BOOL Init(DWORD ID);

    virtual int SceneType();
    virtual void Render(double);

    void set_sequence_animation(DWORD AnimID, BOOL ClearAnims, long LowFrame, float FrameRate);

private:

    CPhysicsObj *m_pPhysicsObj;
};
