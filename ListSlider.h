
#pragma once

#include "HUDControls.h"



typedef std::set<DWORD> FileSliderList;

struct BTreeEntry;

class FileSliderHC : public HUDControl
{
public:
    FileSliderHC(RECT Rect, BYTE FileType);
    virtual ~FileSliderHC();

    void SelectID(DWORD ID, D3DCOLOR Color);
    DWORD GetPrevSel();
    DWORD GetNextSel();
    DWORD GetPrevPageSel();
    DWORD GetNextPageSel();

protected:
    virtual void DrawInternal();

    // For initialization of the file list
    void InitFiles(BYTE FileType);
    void AppendFileToList(DWORD ID);
    static void InitFileCallback(DWORD ID, BTreeEntry *);
    static FileSliderHC *CallbackClass;

    // For drawing ..
    int DrawListEntries(int X, int Y, int LineHeight);
    void DrawListHeading(int X, int Y);
    void DrawSelectionBox(int X, int Y, int Width, int Height);

    // The file list
    FileSliderList m_FileList;
    FileSliderList::iterator m_ListSel;

    // The current selection
    DWORD        m_SelID;
    COLORREF    m_SelColor;

    // This needs to be moved
    int            m_LineHeight;
};


