
#include "StdAfx.h"
#include "Material.h"

CMaterial::CMaterial()
{
    m_bTranslucent = FALSE;
    
    ZeroMemory(&m_D3DMaterial, sizeof(m_D3DMaterial));

    // m_D3DMaterial.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    // m_D3DMaterial.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}

CMaterial::CMaterial(CMaterial* pMaterial)
{
    m_bTranslucent = pMaterial->m_bTranslucent;
}

CMaterial::~CMaterial()
{
}

void CMaterial::CheckAlphaValues()
{
    if (m_D3DMaterial.Ambient.a < 1.0f || m_D3DMaterial.Diffuse.a < 1.0f ||
         m_D3DMaterial.Specular.a < 1.0f || m_D3DMaterial.Emissive.a < 1.0f)
        m_bTranslucent = TRUE;
    else
        m_bTranslucent = FALSE;
}

void CMaterial::SetDiffuseSimple(float Amount)
{
    m_D3DMaterial.Diffuse.r = Amount;
    m_D3DMaterial.Diffuse.g = Amount;
    m_D3DMaterial.Diffuse.b = Amount;
}

void CMaterial::SetLuminositySimple(float Amount)
{
    m_D3DMaterial.Emissive.r = Amount;
    m_D3DMaterial.Emissive.g = Amount;
    m_D3DMaterial.Emissive.b = Amount;
}

void CMaterial::SetTranslucencySimple(float Amount)
{
    m_D3DMaterial.Ambient.a = Amount;
    m_D3DMaterial.Diffuse.a = Amount;
    m_D3DMaterial.Specular.a = Amount;
    m_D3DMaterial.Emissive.a = Amount;

    CheckAlphaValues();
}





