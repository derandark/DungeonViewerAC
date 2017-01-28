
#pragma once

#include "ObjCache.h"

class RGBAUnion
{
public:
    static void Get(DWORD RGBA, float *R, float *G, float *B);
};

class RGBColor
{
public:
    RGBColor();

    BOOL UnPack(BYTE **ppData, ULONG iSize);
    void SetColor32(DWORD Color);

    float m_fRed;
    float m_fGreen;
    float m_fBlue;
};

class Palette : public DBObj
{
public:
    Palette();
    ~Palette();

    static DBObj* Allocator();
    static void Destroyer(DBObj*);
    static Palette* Get(DWORD ID);

    static void        releasePalette(Palette *pPalette);
    static Palette *copyRef(Palette *pPalette);
    static Palette *get_solid_color_palette();
    static DWORD    get_solid_color_index();

    virtual BOOL UnPack(BYTE** ppData, ULONG iSize);

    BOOL InitEnd();

    WORD get_color(DWORD Index);
    DWORD get_color_32(DWORD Index); // this was made up
    void set_color_index(DWORD Index, DWORD Color);

    static LongNIValHash<Palette*>    custom_palette_table; // 0x005E1288
    static Palette*                    solid_color_palette; // 0x005F3BD4;
    static DWORD                    curr_solid_index;

// protected:

    DWORD    m_dw18;
    DWORD    m_dw1C;
    DWORD    m_dw20;
    DWORD    m_dw24;
    DWORD    m_dwNumPaletteColors; // 0x1C / 0x28
    DWORD    m_dwNumScreenColors; // 0x20 / 0x2C
    WORD*    m_pScreenColors; // 0x24 / 0x30
    float    m_fMinimumAlpha; // 0x28 / 0x34
    BOOL    m_bInCache; // 0x2C / 0x38
    DWORD*    m_pPaletteColors; // 0x30 / 0x3C

    // this was made up:
    DWORD*   m_pScreenColors32;
};
