
#include "StdAfx.h"
#include "CreditsDialog.h"

HWND CreditsDialog::m_hWnd = 0;

void CreditsDialog::Create(HWND hParent)
{
    if (m_hWnd)
        return;

    // Create the window.
    m_hWnd = CreateDialog(Globals::GetAppInstance(), (LPCTSTR)IDD_CREDITS, hParent, (DLGPROC)DlgProc);

    if (!m_hWnd)
    {
        // Failure.
        MessageBox(hParent, "Failed creating credits dialog!", "Error", MB_ICONHAND);
    }
}

void CreditsDialog::InitDialog(void)
{
}

#include "MainDialog.h"
#include "Random.h"

INT_PTR CreditsDialog::DlgProc(HWND hWnd, UINT MsgID, WPARAM wParam, LPARAM lParam)
{
    switch(MsgID)
    {
    case WM_INITDIALOG:
        {
            m_hWnd = hWnd;

            InitDialog();

            return TRUE;
        }
    case WM_CLOSE:
        {
            DestroyWindow(hWnd);

            return TRUE;
        }
    case WM_DESTROY:
        {
            m_hWnd = 0;
    
            return TRUE;
        }
    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case IDOK:
                DestroyWindow(hWnd);

                break;
            }

            return TRUE;
        }
    }

    return FALSE;
}





