
#pragma once

// static Dialog interface.

#define DEF_BACKGROUND_COLOR 0x00000000UL
#define DEF_WINDOW_WIDTH 1200 // 800
#define DEF_WINDOW_HEIGHT 920 // 620
#define DEF_CLASS_NAME "DungeonViewer"
#define DEF_WINDOW_NAME "Dungeon Viewer"

class MainDialog
{
public:
    static void Init();
    static void Cleanup();

    static void RegisterClass();
    static void UnregisterClass();
    static BOOL Create();
    static BOOL LoadHotkeys();

    static long Modal();

    static HFONT GetWindowFont();
    static HWND GetWindowHandle();

private:

    static void SaveScreenshot();
    static LRESULT WndProc(HWND hWnd, UINT MsgID, WPARAM wParam, LPARAM lParam);

    static HWND        m_hWnd;
    static HBRUSH    m_hBGBrush;
    static HFONT    m_hFont;
    static HACCEL    m_hAccelTable;
};
