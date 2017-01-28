
//
// CreditsDialog.h
//

#pragma once

// For MIDI output.
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mmsystem.h> 

class CreditsDialog
{
public:
    static void Create(HWND hParent);

private:
    static void InitDialog(void);

    static INT_PTR DlgProc(HWND hWnd, UINT MsgID, WPARAM wParam, LPARAM lParam);

    static HWND m_hWnd;
};
