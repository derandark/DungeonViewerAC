
#pragma once

class GameSky;
class CLandBlock;
class CGfxObj;
class Position;

class LScape
{
public:
    LScape();
    ~LScape();

    void    calc_draw_order(DWORD, BOOL);
    void    draw(void);
    void    pea_draw(void); // Replacement for "draw"
    CLandBlock* get_all(DWORD LandBlock);
    void    get_block_orient(long x, long y, long& magic1, long& magic2);
    BOOL    get_block_shift(DWORD Block1, DWORD Block2, DWORD* XOut, DWORD* YOut);
    void    release_all(void);
    void    set_sky_position(const Position& Pos);
    void    update_block(DWORD LandBlock, long ShiftX, long ShiftY, BOOL Unknown);
    void    update_loadpoint(DWORD LandBlock);

    void    ChangeRegion(void);
    void    CleanupDetailSurfaces(void);
    BOOL    GenerateDetailMesh(void);
    BOOL    SetMidRadius(long MidRadius);

    long        m_MidRadius;        // 0x00
    long        m_MaxRadius;        // 0x04
    CLandBlock  **m_LandBlocks;        // 0x08
    LPVOID        m_0C;                // 0x0C
    DWORD        m_LoadPointBlock;    // 0x10
    DWORD        m_14;
    DWORD        m_18;
    DWORD        m_1C;
    GameSky        *m_GameSky;            // 0x20
    CGfxObj        *m_DetailMesh;        // 0x24
    DWORD        m_28;
    DWORD        m_2C;
    DWORD        m_30;
    DWORD        m_34;
    DWORD        m_38;
    DWORD        m_3C;
};