
#include "StdAfx.h"
#include "DATDisk.h"
#include "LandDefs.h"
#include "RenderScene.h"
#include "Preferences.h"
#include "SDDialog.h"

HWND                SDDialog::m_hWnd = 0;
long                SDDialog::CurrentSelectedIndex = -1;
long                SDDialog::CurrentSelectedDungeonID = -1;
DWORD                SDDialog::CurrentDungeonID = 0;
char                SDDialog::CurrentDungeonLabel[256] = "\0";
SmartArray<DWORD>    SDDialog::DungeonIDs;
DVProfileManager    SDDialog::DungeonProfiles;

void SDDialog::Create(HWND hParent)
{
    if (m_hWnd)
        return;

    // Create the window.
    m_hWnd = CreateDialog(Globals::GetAppInstance(), (LPCTSTR)IDD_SELECTDUNGEON, hParent, (DLGPROC)DlgProc);

    if (!m_hWnd)
    {
        // Failure.
        MessageBox(hParent, "Failed creating select dungeon dialog!", "Error", MB_ICONHAND);
    }
}

DWORD SDDialog::GetCurrentDungeonID(void)
{
    return CurrentDungeonID;
}

const char *SDDialog::GetCurrentDungeonLabel(void)
{
    return CurrentDungeonLabel;
}

void SDDialog::InitDialog(void)
{
    static BOOL FirstInit = TRUE;

    if (FirstInit)
    {
        Time::Update();
        double startTime = Time::GetTimeElapsed();

        DungeonProfiles.UnPack(Preferences::GetDVProfilePath());

        Time::Update();
        double endTime = Time::GetTimeElapsed();

        DEBUGOUT("Profiles loaded in %f seconds.\r\n", endTime - startTime);

        ScanForDungeons();

        FirstInit = FALSE;
    }

    PopulateList();
}

void SDDialog::PopulateList(void)
{
    HWND hList = GetDlgItem(m_hWnd, IDC_DUNGEONLIST);
    SendMessage(hList, LB_RESETCONTENT, 0, 0);

    int defaultdungeon = -1;

    for (int i = 0; i < DungeonIDs.num_used; i++)
    {
        char DungeonLabel[256];
        DWORD BlockID = DungeonIDs.array_data[i];

        _snprintf(DungeonLabel, 255, "0x%04X - %s",
            BlockID, DungeonProfiles.GetBlockName(BlockID));

        SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)DungeonLabel);

        if (BlockID == 0x1F5)
         defaultdungeon = i;
    }

    SendMessage(hList, LB_SETCURSEL, (WPARAM)defaultdungeon, 0);
}

void SDDialog::ScanForDungeons(void)
{
    if (!DATDisk::pCell)
        return;

    SDSplash::Create(m_hWnd);

    // The massive scan..
    DATDisk::pCell->FindFileIDsWithinRange(0, 0xFFFFFFFF, &ScanCallback, &SDSplash::SetProgress);

    SDSplash::Destroy();
}

void SDDialog::ScanCallback(DWORD ID, BTreeEntry *pEntry)
{
    if ((ID & LandDefs::cellid_mask) == LandDefs::lbi_cell_id)
    {
        // DANGEROUS !! If Turbine changes CEnvCell I may break!!
        // DWORD CellFlags[3];

        // if (DATDisk::pCell->GetDataEx(pEntry->BlockHead, &CellFlags, sizeof(DWORD) * 2))
        // {
        //  if (CellFlags[2] > 0)
        //    {
                // LandBlock has cells.
                // Add this dungeon to the list.
                ID >>= LandDefs::block_part_shift;
                DungeonIDs.add(&ID);
        //     }
        // }
    }
}

void SDDialog::ViewDungeon(void)
{
    HWND hList = GetDlgItem(m_hWnd, IDC_DUNGEONLIST);
    int SelectedIndex = SendMessage(hList, LB_GETCURSEL, 0, 0);

    if (SelectedIndex >= 0)
    {
        int Length = SendMessage(hList, LB_GETTEXTLEN, (WPARAM)SelectedIndex, 0);
        char *Text;

        if ((Length > 0) && (Text = new char[Length + 3]))
        {
            Text[0] = '\0';
            Length = SendMessage(hList, LB_GETTEXT, (WPARAM)SelectedIndex, (LPARAM)Text);

            if (Length >= 4)
            {
                DWORD Dungeon;
                sscanf(Text, "%X", &Dungeon);
                strncpy(CurrentDungeonLabel, Text, 255);

                ViewDungeon(Dungeon);
            }

            delete [] Text;
        }
    }
}

void SDDialog::ViewDungeon(DWORD Dungeon)
{
    RenderScene::CreateDungeonScene(Dungeon << LandDefs::block_part_shift);

    CurrentDungeonID = Dungeon;
}

void SDDialog::SelectionChanged(void)
{
    HWND hList = GetDlgItem(m_hWnd, IDC_DUNGEONLIST);

    if ((CurrentSelectedIndex = SendMessage(hList, LB_GETCURSEL, 0, 0)) >= 0)
    {
        int Length = SendMessage(hList, LB_GETTEXTLEN, (WPARAM)CurrentSelectedIndex, 0);
        char *Text;

        if ((Length > 0) && (Text = new char[Length + 3]))
        {
            Text[0] = '\0';
            Length = SendMessage(hList, LB_GETTEXT, (WPARAM)CurrentSelectedIndex, (LPARAM)Text);

            if (Length >= 4)
            {    
                sscanf(Text, "%X", &CurrentSelectedDungeonID);

                char BlockIDString[12];
                sprintf(BlockIDString, "0x%04X", CurrentSelectedDungeonID);

                SetDlgItemText(m_hWnd, IDC_BLOCKID, BlockIDString);
                SetDlgItemText(m_hWnd, IDC_BLOCKNAME, DungeonProfiles.GetBlockName(CurrentSelectedDungeonID));
            }

            delete [] Text;
        }
    }
}

void SDDialog::ChangeSelectedName(void)
{
    if (CurrentSelectedDungeonID < 0)
        return;

    HWND hBlockName = GetDlgItem(m_hWnd, IDC_BLOCKNAME);

    int NameLength = GetWindowTextLength(hBlockName) + 1;

    char *Name = new char[NameLength];
    GetWindowText(hBlockName, Name, NameLength);

    DungeonProfiles.SetBlockName (CurrentSelectedDungeonID, Name);
    DungeonProfiles.Pack(Preferences::GetDVProfilePath());

    char DungeonLabel[256];

    _snprintf(DungeonLabel, 255, "0x%04X - %s",
        CurrentSelectedDungeonID, Name);

    SendMessage(
        GetDlgItem(m_hWnd, IDC_DUNGEONLIST), LB_INSERTSTRING,
        CurrentSelectedIndex, (LPARAM)DungeonLabel);

    SendMessage(
        GetDlgItem(m_hWnd, IDC_DUNGEONLIST), LB_DELETESTRING,
        CurrentSelectedIndex + 1, 0);

    SendMessage(
        GetDlgItem(m_hWnd, IDC_DUNGEONLIST), LB_SETCURSEL,
        CurrentSelectedIndex, 0);

    delete [] Name;
}

INT_PTR SDDialog::DlgProc(HWND hWnd, UINT MsgID, WPARAM wParam, LPARAM lParam)
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
            case IDC_DUNGEONLIST:

                switch(HIWORD(wParam))
                {
                case LBN_SELCHANGE:
                    SelectionChanged();
                    break;

                case LBN_DBLCLK:
                    ViewDungeon();
                    DestroyWindow(hWnd);
                    break;
                }

                break;
            case IDC_SETNAME:
                ChangeSelectedName();

                break;
            case IDC_VIEWDUNGEON:
                ViewDungeon();
                DestroyWindow(hWnd);

                break;
            case IDCANCEL:
                DestroyWindow(hWnd);

                break;
            }

            return TRUE;
        }
    }

    return FALSE;
}

HANDLE SDSplash::m_hThread = NULL;
HANDLE SDSplash::m_hQuit = NULL;

HWND SDSplash::m_hWnd = 0;
HWND SDSplash::m_hProgress = 0;
long SDSplash::m_lProgress = 0;

void SDSplash::Destroy(void)
{
    if (m_hThread)
    {
        if (m_hQuit)
        {
            SetEvent(m_hQuit);

            if (WAIT_TIMEOUT == WaitForSingleObject(m_hThread, 3000))
                TerminateThread(m_hThread, 0);

            CloseHandle(m_hQuit);
            m_hQuit = 0;
        }
        else
            TerminateThread(m_hThread, 0);

        CloseHandle(m_hThread);
        m_hThread = 0;
    }
    else
    if (m_hQuit)
    {
        CloseHandle(m_hQuit);
        m_hQuit = 0;
    }
}

void SDSplash::Create(HWND hParent)
{
    if (!m_hQuit)
        m_hQuit = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (!m_hThread)
    {
        DWORD dwThreadId;

        m_hThread = CreateThread(
            NULL,
            0,
            (LPTHREAD_START_ROUTINE)&ThreadProc,
            (LPVOID)hParent,
            /*CREATE_SUSPENDED*/0,
            &dwThreadId);
    }
}

DWORD WINAPI SDSplash::ThreadProc(LPVOID hParent)
{
    if (m_hWnd)
        return 0;

    // Create the window.
    m_hWnd = CreateDialog(Globals::GetAppInstance(), (LPCTSTR)IDD_SELECTDUNGEONSPLASH, (HWND)0, (DLGPROC)DlgProc);

    if (!m_hWnd)
    {
        // Failure.
        MessageBox((HWND)hParent, "Failed creating progress dialog!", "Error", MB_ICONHAND);
    }

    MSG Msg;

    for (;;) {
        if (::PeekMessage(&Msg, NULL, 0U, 0U, PM_REMOVE))
        {
            if (Msg.message == WM_QUIT)
                return 0;

            if (!IsDialogMessage(Msg.hwnd, &Msg))
            {
                TranslateMessage(&Msg);
                DispatchMessage(&Msg);
            }
        }

        if (WAIT_OBJECT_0 == WaitForSingleObject(m_hQuit, 0))
            break;
    }

    if (m_hWnd)
    {
        DestroyWindow(m_hWnd);
        m_hWnd;
    }

    return 0;
}

void SDSplash::InitDialog(void)
{
    m_hProgress = GetDlgItem(m_hWnd, IDC_PROGRESS);
    m_lProgress = 0;

    SendMessage(m_hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 200));
    SendMessage(m_hProgress, PBM_SETPOS, 0, 0);
}

void SDSplash::SetProgress(float Progress)
{
    long Position = Progress * 2;

    if (Position > 200)
        Position = 200;

    if (m_lProgress != Position)
    {
        m_lProgress = Position;

        SendMessage(m_hProgress, PBM_SETPOS, m_lProgress, 0);
    }
}

INT_PTR SDSplash::DlgProc(HWND hWnd, UINT MsgID, WPARAM wParam, LPARAM lParam)
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
        }
    }

    return FALSE;
}




