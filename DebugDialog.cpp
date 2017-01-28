
#include "StdAfx.h"
#include "DebugDialog.h"
#include "Preferences.h"

HWND DebugDialog::m_hWnd = 0;

void DebugDialog::Create(HWND hParent)
{
    if (m_hWnd)
        return;

    // Create the window.
    m_hWnd = CreateDialog(Globals::GetAppInstance(), (LPCTSTR)IDD_DEBUG, hParent, (DLGPROC)DlgProc);

    if (!m_hWnd)
    {
        // Failure.
        MessageBox(hParent, "Failed creating debug dialog!", "Error", MB_ICONHAND);
    }
}

void DebugDialog::OutputDirect(const char* Text)
{
    HWND hOutput = GetDlgItem(m_hWnd, IDC_OUTPUT);

    if (hOutput)
    {
        DWORD SelStart, SelEnd;
        DWORD TextLength = SendMessage(hOutput, WM_GETTEXTLENGTH, 0, 0);

        SendMessage(hOutput, EM_GETSEL, (WPARAM)&SelStart, (LPARAM)&SelEnd);
        SendMessage(hOutput, EM_SETSEL, TextLength, TextLength);
        SendMessage(hOutput, EM_REPLACESEL, FALSE, (LPARAM)Text);
        SendMessage(hOutput, EM_SETSEL, SelStart, SelEnd);
    }
}

void DebugDialog::Output(const char* Format, ...)
{
    if (!m_hWnd)
        return;

    va_list    ArgPtr;
    va_start(ArgPtr, Format);

    int TempSize = _vscprintf(Format, ArgPtr) + 1;
    char *TempBuffer = new char[ TempSize ];

    int WriteCount = _vsnprintf(TempBuffer, TempSize, Format, ArgPtr);

    if (WriteCount > 0)
        OutputDirect(TempBuffer);

    // Free temporary buffer.
    delete [] TempBuffer;

    va_end(ArgPtr);
}

void DebugDialog::Clear()
{
    HWND hOutput = GetDlgItem(m_hWnd, IDC_OUTPUT);

    if (hOutput)
    {
        SendMessage(hOutput, WM_SETTEXT, 0, (LPARAM)"");
    }
}

void DebugDialog::SaveToFile()
{
    // Ask where the file is to be saved to.
    char szFile[ MAX_PATH ];
    szFile[0] = '\0';

    OPENFILENAME ofn;
    memset(&ofn, 0, sizeof(OPENFILENAME));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = m_hWnd;
    ofn.lpstrFilter = "Text File\0*.txt\0\0";
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrInitialDir = Preferences::GetApplicationPath();
    ofn.lpstrTitle = "Save Debug File As..";
    ofn.lpstrDefExt = "txt";
    ofn.Flags = OFN_OVERWRITEPROMPT;

    if (!GetSaveFileName(&ofn))
        return;

    HWND hOutput = GetDlgItem(m_hWnd, IDC_OUTPUT);

    if (hOutput)
    {
        DWORD Length = (DWORD)SendMessage(hOutput, WM_GETTEXTLENGTH, 0, 0);
        DWORD AllocLength = Length + 1;
        char *TempBuffer = new char[ AllocLength ];

        if (TempBuffer)
        {
            // For safety.
            ZeroMemory(TempBuffer, AllocLength);

            SendMessage(hOutput, WM_GETTEXT, (WPARAM)AllocLength, (LPARAM)TempBuffer);

            FILE *fp = fopen(szFile, "wt");

            if (fp)
            {
                fprintf(fp, "%s", TempBuffer);
                fclose(fp);
            }
            else
                MessageBox(m_hWnd, "Failed opening text file for output.", "Error", MB_ICONHAND);

            delete [] TempBuffer;
        }
    }
}

INT_PTR DebugDialog::DlgProc(HWND hWnd, UINT MsgID, WPARAM wParam, LPARAM lParam)
{
    switch(MsgID)
    {
    case WM_INITDIALOG:
        {
            m_hWnd = hWnd;

            Output("Debug window opened.\r\n");
            
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
            case IDC_SAVETOFILE:

                SaveToFile();

                break;

            case IDC_CLEAR:

                Clear();

                break;
            }

            return TRUE;
        }
    }

    return FALSE;
}