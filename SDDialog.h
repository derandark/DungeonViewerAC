
//
// SDDialog.h
//

#pragma once

#include "SmartArray.h"
#include "DungeonProfiles.h"

struct BTreeEntry;

class SDDialog
{
public:
    static void Create(HWND hParent);

    static const char *GetCurrentDungeonLabel(void);
    static DWORD GetCurrentDungeonID(void);

private:

    static void InitDialog(void);

    static void PopulateList(void);

    static void ScanForDungeons(void);
    static void ScanCallback(DWORD ID, BTreeEntry *);
    static void AddDungeon(DWORD ID);

    static void ViewDungeon(void);
    static void ViewDungeon(DWORD);

    static void SelectionChanged(void);
    static void ChangeSelectedName(void);

    static INT_PTR DlgProc(HWND hWnd, UINT MsgID, WPARAM wParam, LPARAM lParam);

    static HWND        m_hWnd;

    // The current selection.
    static long        CurrentSelectedIndex;
    static long        CurrentSelectedDungeonID;

    // The current dungeon.
    static DWORD    CurrentDungeonID;
    static char        CurrentDungeonLabel[256];

    // The dungeon IDs.
    static SmartArray<DWORD> DungeonIDs;

    // The dungeon profiles.
    static DVProfileManager DungeonProfiles;
};

class SDSplash
{
public:
    static void Create(HWND hParent);
    static void Destroy(void);

    static void SetProgress(float Progress);

private:
    
    static void InitDialog(void);

    static DWORD WINAPI ThreadProc(LPVOID hParent);
    static INT_PTR DlgProc(HWND hWnd, UINT MsgID, WPARAM wParam, LPARAM lParam);

    static HANDLE    m_hThread;
    static HANDLE    m_hQuit;

    static HWND        m_hWnd;
    static HWND        m_hProgress;
    static long        m_lProgress;

};





