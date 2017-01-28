
#pragma once

class SetupScene :
    public ModelScene
{
public:
    SetupScene();
    virtual ~SetupScene();

    void Destroy();
    BOOL Init(DWORD ID);

    virtual int SceneType();
    virtual void Render(double);

    void set_sequence_animation(DWORD AnimID, BOOL ClearAnims, long LowFrame, float FrameRate);

private:

    CPartArray *m_pPartArray;
};
