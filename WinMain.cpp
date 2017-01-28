
#include "StdAfx.h"
#include "Preferences.h"
#include "GetDXVersion.h"
#include "DebugDialog.h"
#include "MainDialog.h"
#include "Render.h"
#include "ObjCache.h"
#include "DATDisk.h"
#include "Frame.h"

void MessageBoxLastError()
{
    LPTSTR lpBuffer;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpBuffer,
        0,
        NULL); 

    // Dump it ?
    MessageBox(NULL, lpBuffer, "Dump of GetLastError():", MB_ICONHAND);

    LocalFree(lpBuffer);
}

void OutputDebug(const char* Format, ...)
{
    va_list    ArgPtr;
    va_start(ArgPtr, Format);

    int CharCount = _vscprintf(Format, ArgPtr) + 1;

    char *CharBuffer = new char[ CharCount ];
    _vsnprintf(CharBuffer, CharCount, Format, ArgPtr);

    // Output to Debug Dialog
    DebugDialog::OutputDirect(CharBuffer);
    
#ifdef _DEBUG
    OutputDebugString(CharBuffer);
#endif
    // Output to Log File?
    // ...

    delete [] CharBuffer;
    va_end(ArgPtr);
}

void OutputDebugFrame(Frame *pFrame)
{
    char *CharBuffer = new char[ 96 ];
    _snprintf(CharBuffer, 96,
        "(X: %.4f Y:%.4f Z:%.4f, W: %.4f X: %.4f Y:%.4f Z:%.4f)",
        pFrame->m_origin.x, pFrame->m_origin.y, pFrame->m_origin.z,
        pFrame->m_angles.w, pFrame->m_angles.x, pFrame->m_angles.y, pFrame->m_angles.z);

    // Output to Debug Dialog
    DebugDialog::OutputDirect(CharBuffer);
    
#ifdef _DEBUG
    OutputDebugString(CharBuffer);
#endif
    // Output to Log File?
    // ...

    delete [] CharBuffer;
}

void OutputDebugMatrix(struct _D3DMATRIX *pMatrix)
{
    char *CharBuffer = new char[ 250 ];
    _snprintf(CharBuffer, 250,
        "(%.4f %.4f %.4f %.4f)\r\n"
        "(%.4f %.4f %.4f %.4f)\r\n"
        "(%.4f %.4f %.4f %.4f)\r\n"
        "(%.4f %.4f %.4f %.4f)\r\n",
        pMatrix->_11, pMatrix->_12, pMatrix->_13, pMatrix->_14,
        pMatrix->_21, pMatrix->_22, pMatrix->_23, pMatrix->_24,
        pMatrix->_31, pMatrix->_32, pMatrix->_33, pMatrix->_34,
        pMatrix->_41, pMatrix->_42, pMatrix->_43, pMatrix->_44);

    // Output to Debug Dialog
    DebugDialog::OutputDirect(CharBuffer);
    
#ifdef _DEBUG
    OutputDebugString(CharBuffer);
#endif
    // Output to Log File?
    // ...

    delete [] CharBuffer;
}

BOOL CheckForDirectX9()
{
    DWORD DXVersionInt;
    TCHAR DXVersionString[16];
    char CloseMessage[ 256 ];
    
    if (SUCCEEDED(GetDXVersion(&DXVersionInt, DXVersionString, 16)))
    {
        // Check if old version
        if (DXVersionInt < 0x90000)
            sprintf(CloseMessage, "This application requires DirectX 9 or higher. You are using version %s. Closing.", DXVersionString);
        else
            return TRUE;
    }
    else
    {
        // No version detected.
        sprintf(CloseMessage, "This application requires DirectX 9 or higher. No directX version was detectable!");
    }

    MessageBox(NULL, CloseMessage, "Whoops!", MB_ICONHAND | MB_OK);
    return FALSE;
}

void CheckForMemoryLeaks()
{
#ifdef _DEBUG
    OutputDebugString("Scanning for memory leaks..\n");

    if (!_CrtDumpMemoryLeaks())
        OutputDebugString("No memory leaks found!\n");
    else
        OutputDebugString("Memory leaks found!\n");
#endif
}

void ThrowDATError()
{
	LPTSTR lpBuffer;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpBuffer,
		0,
		NULL);
	
	char errorText[1024];
#ifdef PRE_TOD
	_snprintf(errorText, 1024, "ERROR opening content .dat files, system gave error: %s\r\nCopy valid pre-Throne of Destiny portal.dat and cell.dat files from the AC folder to the current folder of this program.", lpBuffer);
#else
	_snprintf(errorText, 1024, "ERROR opening content .dat files, system gave error: %s\r\nCopy valid post-Throne of Destiny client_portal.dat and client_cell_1.dat files from the AC folder to the current folder of this program.", lpBuffer);
#endif
	MessageBox(NULL, errorText, "Dungeon Viewer error", MB_ICONHAND|MB_OK);

	LocalFree(lpBuffer);
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR   lpCmdLine,
                     int       nCmdShow)
{
    if (!CheckForDirectX9())
        return 0;

    srand((unsigned int)time(NULL));

    INITCOMMONCONTROLSEX iccex;
    iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    iccex.dwICC = ICC_PROGRESS_CLASS;
    InitCommonControlsEx(&iccex);

    Globals::Init(hInstance);

    // if (Preferences::CheckIfFirstRun())
    //     MessageBox(NULL, "This appears to be your first run of Dungeon Viewer.", "Welcome", MB_OK);
    RenderPreferences::LoadFromRegistry();

    if (DATDisk::OpenDisks())
    {
        ObjCaches::InitCaches();

        MainDialog::Init();
        MainDialog::RegisterClass();

        // Create the main window.
        if (MainDialog::Create())
        {
            MainDialog::LoadHotkeys();

            // Init Direct3D etc.
            if (Render::Init(MainDialog::GetWindowHandle()))
            {
                // Enter the main dialog loop.
                MainDialog::Modal();
            }

            Render::Cleanup();
        }
        else
        {
            MessageBox(NULL, "Failed creating main dialog.", "Error", MB_ICONHAND);
            MessageBoxLastError();
        }

        MainDialog::UnregisterClass();
        MainDialog::Cleanup();

        ObjCaches::DestroyCaches();
    }
    else
        ThrowDATError();

    DATDisk::CloseDisks();
    Globals::Cleanup();

#ifdef _DEBUG
    atexit(CheckForMemoryLeaks);
#endif

    return 0;
}