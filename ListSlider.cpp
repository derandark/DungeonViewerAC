
#include "StdAfx.h"
#include "Render.h"
#include "DATDisk.h"
#include "ListSlider.h"

#define NUM_FILEIDS_TO_DRAW 9

// File Slider HUD control
FileSliderHC *FileSliderHC::CallbackClass;

FileSliderHC::FileSliderHC(RECT Rect, BYTE FileType) : HUDControl(Rect)
{
    InitFiles(FileType);

    m_SelColor = 0xFFFFFF00;
}

FileSliderHC::~FileSliderHC()
{
}

void FileSliderHC::InitFiles(BYTE FileType)
{
    DWORD Min = (DWORD)FileType << 24;
    DWORD Max = Min | 0xFFFFFF;

    CallbackClass = this;
    DATDisk::pPortal->FindFileIDsWithinRange(Min, Max, &InitFileCallback, NULL);
}

void FileSliderHC::InitFileCallback(DWORD ID, BTreeEntry *pEntry)
{
    CallbackClass->AppendFileToList(ID);
}

void FileSliderHC::AppendFileToList(DWORD ID)
{
    m_FileList.insert(ID);
}

void FileSliderHC::DrawInternal()
{
    DrawRegionBox();

    // Must have some spacing for the selection box to work.
    const int LINE_SPACING = 4;

    DrawListHeading(0, 0);
    
    int SelectionY =
        DrawListEntries(4, m_LineHeight + 8, m_LineHeight + LINE_SPACING);

    if (SelectionY >= 0)
        DrawSelectionBox(1, SelectionY - (LINE_SPACING / 2), 68, m_LineHeight + LINE_SPACING);
}

void FileSliderHC::DrawListHeading(int x, int y)
{
    m_LineHeight = Render::DrawString(m_Rect.left + x, m_Rect.top + y, "Select File:");
}

// Returns Y offset of selection.
int FileSliderHC::DrawListEntries(int x, int y, int LineHeight)
{
    FileSliderList::iterator LVTop = m_ListSel;

    if (LVTop == m_FileList.end())
        return -1;

    for (int i = 0; i < ((NUM_FILEIDS_TO_DRAW-1) / 2); i++)
    {
        if (LVTop == m_FileList.begin())
            break;

        LVTop--;
    }

    int SelectionY = -1;
    char StringNum[12];
    StringNum[0] = '0';
    StringNum[1] = 'x';

    for (int i = 0; i < NUM_FILEIDS_TO_DRAW; i++)
    {
        if (LVTop == m_FileList.end())
            break;

        DWORD ID = *LVTop;

        if (ID == m_SelID)
            SelectionY = y;

        sprintf(&StringNum[2], "%08X", ID);

        Render::DrawString(m_Rect.left + x, m_Rect.top + y, StringNum);
        y += LineHeight;

        LVTop++;
    }

    return SelectionY;
}

void FileSliderHC::DrawSelectionBox(int x, int y, int Width, int Height)
{
    DrawBox(x, y, Width, Height, m_SelColor, 1.0f, TRUE);
}

void FileSliderHC::SelectID(DWORD Key, D3DCOLOR Color)
{
    m_ListSel = m_FileList.find(Key);

    m_SelID = Key;
    m_SelColor = Color | 0xFF000000;
}

DWORD FileSliderHC::GetPrevSel()
{
    if (m_ListSel == m_FileList.begin())
        return 0;

    FileSliderList::iterator Pos = m_ListSel;
        
    return(*(--Pos));
}

DWORD FileSliderHC::GetNextSel()
{
    FileSliderList::iterator Pos = m_ListSel;
    
    if ((++Pos) == m_FileList.end())
        return 0;

    return(*Pos);
}

DWORD FileSliderHC::GetPrevPageSel()
{
    if (m_ListSel == m_FileList.begin())
        return 0;

    FileSliderList::iterator Pos = m_ListSel;
    DWORD LastID = 0;

    for (int i = 0; i < NUM_FILEIDS_TO_DRAW; i++)
    {
        if ((--Pos) == m_FileList.begin())
        {
            LastID = *Pos;
            break;
        }

        LastID = *Pos;
    }
    
    return LastID;
}

DWORD FileSliderHC::GetNextPageSel()
{
    FileSliderList::iterator Pos = m_ListSel;
    DWORD LastID = 0;

    for (int i = 0; i < NUM_FILEIDS_TO_DRAW; i++)
    {
        if ((++Pos) == m_FileList.end())
            break;

        LastID = *Pos;
    }
    
    return LastID;
}





