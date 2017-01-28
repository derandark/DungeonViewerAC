
#pragma once

class DebugDialog
{
public:
    static void Create(HWND hParent);

    static void OutputDirect(const char* Text);
    static void Output(const char* Format, ...);

private:

    static void Clear();
    static void SaveToFile();

    static INT_PTR DlgProc(HWND hWnd, UINT MsgID, WPARAM wParam, LPARAM lParam);

    static HWND m_hWnd;
};
