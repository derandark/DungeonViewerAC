
#include "StdAfx.h"
#include "Texture.h"
#include "Palette.h"
#include "Surface.h"

LongNIValHash<CSurface *> CSurface::custom_surface_table(128);

CSurface::CSurface() : DBObj(ObjCaches::Surfaces)
{
    m_dw18            = 0;            // 0x18
    m_dw1C            = 0;            // 0x1C
    m_dw20            = 0;            // 0x20
    m_dw24            = 0;            // 0x24
    m_dwFlags        = 0;            // 0x28
    m_dw2C            = 0;            // 0x2C
    m_dwRGBColor    = 0xFFFFFFFF;    // 0x30
    m_dw34            = 0xFFFFFFFF;    // 0x34
    m_dwCurTexID    = 0;            // 0x38
    m_pTexture        = NULL;        // 0x3C
    m_pPalette        = NULL;        // 0x40
    m_fTranslucency    = 0.0f;        // 0x44
    m_fLuminosity    = 0.0f;        // 0x48
    m_fDiffuse        = 1.0f;        // 0x4C
    m_dwTextureID    = 0;            // 0x50
    m_dwPaletteID    = 0;            // 0x54
    m_f58            = 0.0f;        // 0x58
    m_f5C            = 1.0f;        // 0x5C
}

CSurface::CSurface(CSurface *pSurface) : DBObj(ObjCaches::Surfaces)
{
    m_dw18            = 0;            // 0x18
    m_dw1C            = 0;            // 0x1C
    m_dw20            = 0;            // 0x20
    m_dw24            = 0;            // 0x24

    if (this != pSurface)
    {
        m_dwFlags            = pSurface->m_dwFlags;

        if (pSurface->m_dw2C == 1)
            m_dw2C            = 4;
        else
            m_dw2C            = pSurface->m_dw2C;

        m_dwCurTexID        = pSurface->m_dwCurTexID;

        m_pTexture            = NULL;
        m_pPalette            = NULL;

        m_dwRGBColor        = pSurface->m_dwRGBColor;
        m_dw34                = pSurface->m_dw34;

        m_fTranslucency        = pSurface->m_fTranslucency;
        m_fLuminosity        = pSurface->m_fLuminosity;
        m_fDiffuse            = pSurface->m_fDiffuse;

        m_dwTextureID        = pSurface->m_dwTextureID;
        m_dwPaletteID        = pSurface->m_dwPaletteID;
        m_f58                = pSurface->m_f58;
        m_f5C                = pSurface->m_f5C;

        if (pSurface->m_pTexture)
            SetTexture(pSurface->m_pTexture);

        if (pSurface->m_pPalette)
            SetPalette(pSurface->m_pPalette);
    }
}

CSurface::~CSurface()
{
    Destroy();
}

DBObj* CSurface::Allocator()
{
    return((DBObj *)new CSurface());
}

void CSurface::Destroyer(DBObj* pSurface)
{
    delete ((CSurface *)pSurface);
}

CSurface *CSurface::Get(DWORD ID)
{
    return (CSurface *)ObjCaches::Surfaces->Get(ID);
}

void CSurface::Release(CSurface *pSurface)
{
    if (pSurface)
        ObjCaches::Surfaces->Release(pSurface->m_Key);
}

CSurface *CSurface::makeCustomSurface(CSurface *pSurface)
{
    if (!pSurface)
        return NULL;

    CSurface *pCustom = new CSurface(pSurface);

    pCustom->m_Key = 0;
    pCustom->m_lLinks = 0;
    custom_surface_table.add(pCustom, (DWORD)pCustom);

    return pCustom;
}

void CSurface::releaseCustomSurface(CSurface *pSurface)
{
    /*
    // Don't ask.
    HashBaseData<unsigned long> *pRemoved = 
        ((HashBase<unsigned long> *)&custom_surface_table)->remove((DWORD)pSurface);

    if (pRemoved)
    {
        delete pRemoved;

        if (pSurface)
            delete pSurface;
    }
    */
    if (custom_surface_table.remove((DWORD)pSurface, &pSurface))
    {
        if (pSurface)
            delete pSurface;
    }
}

void CSurface::releaseSurfaces()
{
    ImgTex::releaseAllSurfaces();
}

void CSurface::ReleaseTexture()
{
    if (m_pTexture) 
    {
        ImgTex::releaseTexture(m_pTexture);
        m_pTexture = NULL;
    }
}

void CSurface::ReleasePalette()
{
    if (m_pPalette)
    {
        Palette::releasePalette(m_pPalette);
        m_pPalette = NULL;
    }
}

void CSurface::Destroy()
{
    ReleaseTexture();
    ReleasePalette();

    m_dwRGBColor    = 0xFFFFFFFF;    // 0x30
    m_dw34            = 0xFFFFFFFF;    // 0x34
    m_dwFlags        = 0;            // 0x28
    m_dwCurTexID    = 0;            // 0x38
    m_dwTextureID    = 0;            // 0x50
    m_dwPaletteID    = 0;            // 0x54
    m_fTranslucency    = 0.0f;        // 0x44
    m_fLuminosity    = 0.0f;        // 0x48
    m_f58            = 0;            // 0x58
    m_dw2C            = 0;            // 0x2C
    m_fDiffuse        = 1.0f;        // 0x4C
    m_f5C            = 1.0f;        // 0x5C
}

BOOL CSurface::ClearSurface()
{
    if (m_dw2C != 1)
        return FALSE;

    ReleaseTexture();
    ReleasePalette();

    return TRUE;
}

DWORD CSurface::GetFlags()
{
    return m_dwFlags;
}

ImgTex* CSurface::GetTexture()
{
    return m_pTexture;
}

BOOL CSurface::RestorePalette()
{
    if (m_dw2C == 1)
        return FALSE;

    if (!m_pTexture)
        return FALSE;

    SetPalette(m_pTexture->m_pPalette);
    return TRUE;
}

void CSurface::SetPalette(Palette* pPalette)
{
    ReleasePalette();

    m_pPalette = Palette::copyRef(pPalette);
}

BOOL CSurface::UsePalette(Palette* pPalette)
{
    if (m_dw2C == 1)
        return FALSE;

    if (m_dw2C == 4)
        m_dw2C = 2;

    if (m_dw2C != 2)
        return FALSE;

    if (!pPalette)
        return FALSE;

    SetPalette(pPalette);

    return TRUE;
}

BOOL CSurface::UseTextureMap(ImgTex *pTexture, DWORD dw2CNum)
{
    if (m_dw2C == 1)
        return FALSE;

    if (m_dw2C != dw2CNum)
        return FALSE;

    if (!pTexture || m_pTexture)
        return FALSE;
    
    m_pTexture = pTexture;
    m_dwFlags = 2;

    if (!m_dwTextureID)
        m_dwTextureID = m_pTexture->m_Key;

    ReleasePalette();

    return TRUE;
}

BOOL CSurface::SetDiffuse(float Value)
{
    if (m_dw2C == 1)
        return FALSE;

    if (Value < m_fLuminosity || Value > 1.0)
        return FALSE;

    m_fDiffuse = Value;

    return TRUE;
}

BOOL CSurface::SetLuminosity(float Value)
{
    if (m_dw2C == 1)
        return FALSE;

    if (Value < 0.0 || Value > 1.0)
        return FALSE;

    m_fLuminosity = Value;

    return TRUE;
}

BOOL CSurface::SetTranslucency(float Value)
{
    if (m_dw2C == 1)
        return FALSE;

    if (Value < 0.0 || Value > 1.0)
        return FALSE;

    m_fTranslucency = Value;

    if (m_fTranslucency > F_EPSILON)
        m_dwFlags |= ST_TRANSLUCENT;
    else
        m_dwFlags &= ~ST_TRANSLUCENT;

    return TRUE;
}

DWORD CSurface::GetOriginalTextureMapID()
{
    return m_dwTextureID;
}

DWORD CSurface::GetOriginalPaletteID() 
{
    return m_dwPaletteID;
}

DWORD CSurface::GetTextureCode(ImgTex* pTexture, Palette* pPalette) 
{
    if (pPalette->m_bInCache)
        return ((pTexture->m_Key << 16) | (pTexture->m_Key & 0xFFFF));
    else
        return 0;
}

BOOL CSurface::UnPack(BYTE** ppData, ULONG iSize)
{
#ifdef PRE_TOD
    UNPACK(DWORD, m_Key);
#else
#endif
    UNPACK(DWORD, m_dwFlags);

    if (GetFlags() & 6)
    {
        UNPACK(DWORD, m_dwTextureID);
        UNPACK(DWORD, m_dwPaletteID);
    }
    else
        UNPACK(DWORD, m_dwRGBColor);

    UNPACK(float, m_fTranslucency);
    UNPACK(float, m_fLuminosity);
    UNPACK(float, m_fDiffuse);

    m_f58 = m_fLuminosity;
    m_f5C = m_fDiffuse;
    m_dw2C = 1;

    InitEnd(1);

    return TRUE;
}

void CSurface::InitEnd(int Unknown)
{
    DWORD TextureID = m_dwCurTexID ? m_dwCurTexID : m_dwTextureID;

    if (!TextureID)
        ReleaseTexture();
    else
    {
        if (GetFlags() & 6)
        {
            long old_image_type = ImgTex::image_use_type;

            // Force type on this texture.
            if (GetFlags() & 4)
                ImgTex::image_use_type = 1;

            ImgTex* pTexture = ImgTex::Get(TextureID);

            // Return to old setting.
            ImgTex::image_use_type = old_image_type;

            if (pTexture)
            {
                // Replace old texture.
                ReleaseTexture();
                m_pTexture = pTexture;

                if (pTexture->m_lType == 2)
                {
                    m_dwCurTexID = pTexture->m_Key;

                    // No surface palette? Inherit from the texture!
                    if (!m_pPalette)
                    {
                        Palette* pPalette = m_pTexture->m_pPalette;

                        if (pPalette)
                        {
                            SetPalette(pPalette);

                            if (Unknown == 1 || Unknown == 2)
                            {
                                // Go ahead and init the palette ID.
                                m_dwPaletteID = m_pPalette->m_Key;
                            }
                        }
                    }
                }
            }
            else
            {
                // Couldn't get ImgTex by that ID!
                if (Unknown == 4)
                    ReleaseTexture();
            }
        }
    }

    if (m_pTexture && (m_pTexture->m_lType == 2) && (Unknown != 2))
    {
        // If we have no palette, Release!
        if (!m_pPalette)
        {
            ReleaseTexture();
        }
        else if (!SetTextureAndPalette(m_pTexture, m_pPalette))
        {
            DEBUGOUT("Failed STAP\r\n");
            ReleaseTexture();
        }
    }

    if (m_fTranslucency > F_EPSILON)
        m_dwFlags |= ST_TRANSLUCENT;
}


void CSurface::SetTexture(ImgTex *pTexture)
{
    ReleaseTexture();

    if (pTexture)
    {
        if (pTexture->m_bInCache)
            m_pTexture = ImgTex::Get(pTexture->m_Key);
        else
            m_pTexture = ImgTex::getCustomTexture(pTexture->m_Key);
    }
}

// Textures may desire a surface palette instead.
// If that is the case, a custom texture gets created here.
BOOL CSurface::SetTextureAndPalette(ImgTex* pTexture, Palette* pPalette)
{
    m_dw2C = 2;

    ImgTex* custom_texture;
    DWORD    custom_texture_ID;

    if (pPalette->m_bInCache)
        custom_texture_ID = (pTexture->m_Key << 16) | (pPalette->m_Key & 0xFFFF);
    else
        custom_texture_ID = 0;

    // Does a custom texture already exist?
    custom_texture = ImgTex::getCustomTexture(custom_texture_ID);

    if (!custom_texture)
    {
        // Ok, make one then..
        custom_texture = ImgTex::makeCustomTexture(custom_texture_ID);

        if (!custom_texture)
        {
            // Failed making texture!
            return FALSE;
        }

        // Combine.
        if (!custom_texture->Combine(pTexture, pPalette, /*Flag 4*/(m_dwFlags >> 2) & 1, 7))
        {
            // Failed to combine!
            ReleaseTexture();
            return FALSE;
        }
    }

    ReleaseTexture();
    m_pTexture = custom_texture;

    return TRUE;
}
