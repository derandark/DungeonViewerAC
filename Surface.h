
#pragma once

#include "ObjCache.h"

class Palette;
class ImgTex;


#define ST_TRANSLUCENT (0x00000010UL)

class CSurface : public DBObj
{
public:
    CSurface(CSurface *pSurface);
    CSurface();
    ~CSurface();

    friend class SurfaceHUD;
    friend class Render;

    static DBObj* Allocator();
    static void Destroyer(DBObj*);
    static CSurface *Get(DWORD ID);
    static void Release(CSurface*);

    static CSurface *makeCustomSurface(CSurface *pSurface);
    static void releaseCustomSurface(CSurface *pSurface);
    static void releaseSurfaces();

    BOOL UnPack(BYTE** ppData, ULONG iSize);

    DWORD GetFlags();

    void Destroy();
    BOOL ClearSurface();
    void ReleaseTexture();
    void ReleasePalette();
    BOOL RestorePalette();
    DWORD GetOriginalTextureMapID();
    DWORD GetOriginalPaletteID();
    DWORD GetTextureCode(ImgTex* pTexture, Palette* pPalette);
    void SetPalette(Palette* pPalette);
    void SetTexture(ImgTex *pTexture);
    BOOL SetTextureAndPalette(ImgTex* pTexture, Palette* pPalette);
    BOOL SetDiffuse(float Value);
    BOOL SetLuminosity(float Value);
    BOOL SetTranslucency(float Value);
    BOOL UsePalette(Palette* pPalette);
    BOOL UseTextureMap(ImgTex *pTexture, DWORD dw2CNum);

    ImgTex* GetTexture();

    void InitEnd(int Unknown);

protected:

    DWORD        m_dw18;                // 0x0C / 0x18
    DWORD        m_dw1C;                // 0x10 / 0x1C
    DWORD        m_dw20;                // 0x14 / 0x20
    DWORD        m_dw24;                // 0x18 / 0x24
    DWORD        m_dwFlags;            // 0x1C / 0x28
    DWORD        m_dw2C;                // 0x20 / 0x2C
    DWORD        m_dwRGBColor;        // 0x24 / 0x30
    DWORD        m_dw34;                // 0x28 / 0x34
    DWORD        m_dwCurTexID;        // 0x2C / 0x38
    ImgTex*        m_pTexture;        // 0x30 / 0x3C
    Palette*    m_pPalette;        // 0x34 / 0x40
    float        m_fTranslucency;    // 0x38 / 0x44
    float        m_fLuminosity;        // 0x3C / 0x48
    float        m_fDiffuse;            // 0x40 / 0x4C
    DWORD        m_dwTextureID;        // 0x44 / 0x50
    DWORD        m_dwPaletteID;        // 0x48 / 0x54
    float        m_f58;                // 0x4C / 0x58
    float        m_f5C;                // 0x50 / 0x5C

    static LongNIValHash<CSurface *> custom_surface_table;
};
