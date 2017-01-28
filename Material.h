
#pragma once

class CMaterial
{
public:
    CMaterial();
    CMaterial(CMaterial*);
    ~CMaterial();

    void CheckAlphaValues();
    void SetDiffuseSimple(float Amount);
    void SetLuminositySimple(float Amount);
    void SetTranslucencySimple(float Amount);
    
    BOOL m_bTranslucent;
    D3DMATERIAL9 m_D3DMaterial;
};
