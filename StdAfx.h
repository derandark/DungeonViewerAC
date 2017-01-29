
#pragma once

// Change this if you want to use DAT files after TOD, ...
// ... but those textures and file formats were never made compatible (didn't put in the effort to reverse engineer them)
// #define PRE_TOD

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES

#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>
#include <process.h>
#include <stdio.h>
#include <time.h>
#include <set>
#include <tchar.h>
#include <Shlwapi.h>
#include <assert.h>

#define INITGUID
#include <dxdiag.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "resource.h"
#include "Globals.h"

#ifdef _DEBUG
#include <crtdbg.h>

#define MYDEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new MYDEBUG_NEW

#endif

#define F_EPSILON ((float)0.0002)
#define DEBUGOUT        OutputDebug
#define DEBUGOUTFRAME    OutputDebugFrame
#define DEBUGOUTMATRIX    OutputDebugMatrix

#define UNFINISHED(comment) __asm int 3;

#if _DEBUG
#define UNFINISHED_WARNING(comment) DEBUGOUT(comment);DEBUGOUT("\r\n");
#else
#define UNFINISHED_WARNING(comment);
#endif

extern void OutputDebug(const char *Format, ...);
extern void OutputDebugFrame(class Frame *pFrame);
extern void OutputDebugMatrix(struct _D3DMATRIX *pMatrix);

inline DWORD FloatToDWORD(float fl) {
    return *((DWORD *)&fl);
}

inline char *CopyString(const char *string)
{
    size_t length = strlen(string) + 1;

    char *dupestring = new char[ length ];
    memcpy(dupestring, string, length);

    return dupestring;
}

#define DEFAULT_DIALOG_FONT_NAME "Arial"
#define DEFAULT_DIALOG_FONT_HEIGHT 12
#define DEFAULT_RENDER_FONT_NAME "Arial" 
#define DEFAULT_RENDER_FONT_HEIGHT 14

#pragma warning(disable: 4244) // Truncation from 'double' to 'float'
#pragma warning(disable: 4305) // Argument truncation from 'double' to 'float'
