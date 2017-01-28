
#pragma once

class PrefDialog
{
public:
    static void Create(HWND hParent);

private:

    static void LoadSettings();
    static void SaveSettings();

    static void UpdateColorEditControl(int Control, D3DCOLOR Color);

    static void BrowseForPortal();
    static void BrowseForCell();
    static void ChooseTextColor();
    static void ChooseFillColor();
    static D3DCOLOR ChooseColor(D3DCOLOR DefaultColor);

    static INT_PTR DlgProc(HWND hWnd, UINT MsgID, WPARAM wParam, LPARAM lParam);

    static HWND m_hWnd;
    static HWND m_hFillMode;
    static HWND m_hMultiSample;

    static D3DCOLOR m_TextColor;
    static D3DCOLOR m_FillColor;
};
