
#include "StdAfx.h"
#include "MainDialog.h"
#include "PrefDialog.h"
#include "DebugDialog.h"
#include "CreditsDialog.h"
#include "SDDialog.h"
#include "Preferences.h"
#include "Render.h"

#include "TestRoutines.h"

void SaveBitmapFile(const char *szFile, UINT Width, UINT Height, UINT Bpp, LPVOID lpData);

HWND    MainDialog::m_hWnd = 0;
HBRUSH    MainDialog::m_hBGBrush = 0;
HFONT    MainDialog::m_hFont = 0;
HACCEL    MainDialog::m_hAccelTable = 0;

void MainDialog::Init()
{
    m_hBGBrush = CreateSolidBrush(DEF_BACKGROUND_COLOR);

    LOGFONT lf;
    lf.lfHeight = DEFAULT_DIALOG_FONT_HEIGHT;
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = FW_NORMAL;
    lf.lfItalic = FALSE;
    lf.lfUnderline = FALSE;
    lf.lfStrikeOut = FALSE;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = ANTIALIASED_QUALITY;
    lf.lfPitchAndFamily = VARIABLE_PITCH;
    strcpy(lf.lfFaceName, DEFAULT_DIALOG_FONT_NAME);

    m_hFont = CreateFontIndirect(&lf);
}

void MainDialog::Cleanup()
{
    DeleteObject(m_hFont);
    DeleteObject(m_hBGBrush);
}

HFONT MainDialog::GetWindowFont()
{
    return m_hFont;
}

HWND MainDialog::GetWindowHandle()
{
    return m_hWnd;
}

void MainDialog::RegisterClass()
{
    WNDCLASSEX wcex;
    HINSTANCE hInstance = Globals::GetAppInstance();

    wcex.cbSize            = sizeof(WNDCLASSEX);
    wcex.style            = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = (WNDPROC)WndProc;
    wcex.cbClsExtra        = 0;
    wcex.cbWndExtra        = 0;
    wcex.hInstance        = hInstance;
    wcex.hIcon            = LoadIcon(hInstance, (LPCTSTR)IDI_MAINICON);
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground    = m_hBGBrush;
    wcex.lpszMenuName    = (LPCTSTR)IDM_MAINMENU;
    wcex.lpszClassName    = DEF_CLASS_NAME;
    wcex.hIconSm        = LoadIcon(hInstance, (LPCTSTR)IDI_MAINICON);

    ::RegisterClassEx(&wcex);
}

void MainDialog::UnregisterClass()
{
    ::UnregisterClass(DEF_CLASS_NAME, Globals::GetAppInstance());
}

BOOL MainDialog::Create()
{
    // Find desktop center.
    long lScreenWidth;
    long lScreenHeight;

    Globals::GetScreenDimensions(&lScreenWidth, &lScreenHeight);

    int X = (lScreenWidth - DEF_WINDOW_WIDTH) / 2;
    int Y = (lScreenHeight - DEF_WINDOW_HEIGHT) / 2;

    if (X < 0)
        X = 0;
    if (Y < 0)
        Y = 0;

    // Create the window.

    m_hWnd = CreateWindow(
        DEF_CLASS_NAME, DEF_WINDOW_NAME,
        WS_BORDER | WS_MINIMIZEBOX | WS_SYSMENU,
        X, Y, DEF_WINDOW_WIDTH, DEF_WINDOW_HEIGHT,
        NULL, NULL, Globals::GetAppInstance(), NULL);

    return(m_hWnd ? TRUE : FALSE);
}

BOOL MainDialog::LoadHotkeys()
{
    m_hAccelTable = LoadAccelerators(Globals::GetAppInstance(), (LPCTSTR)IDA_HOTKEYS);

    return(m_hAccelTable ? TRUE : FALSE);
}

long MainDialog::Modal()
{

    ShowWindow(m_hWnd, SW_SHOWNORMAL);
    UpdateWindow(m_hWnd);

    MSG Msg;

    for (;;)
    {
        if (::PeekMessage(&Msg, NULL, 0U, 0U, PM_REMOVE))
        {
            if (Msg.message == WM_QUIT)
                return (long)Msg.wParam;

            if (!TranslateAccelerator(Msg.hwnd, m_hAccelTable, &Msg)) 
            {
                TranslateMessage(&Msg);
                DispatchMessage(&Msg);
            }
        }
        else
        {
            //if (::GetActiveWindow() == m_hWnd)
            Render::DrawFrame();
            
            if (!RenderPreferences::NoSleep)
                Sleep(1);
        }
    }
}

LRESULT MainDialog::WndProc(HWND hWnd, UINT MsgID, WPARAM wParam, LPARAM lParam)
{
    switch(MsgID)
    {
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            RECT rect;
            HDC hDC;

            hDC = BeginPaint(hWnd, &ps);
            ::GetClientRect(hWnd, &rect);

            // Draw(hDC);
            
            EndPaint(hWnd, &ps);

            break;
        }
    case WM_CLOSE:
        {
            PostQuitMessage(0);

            break;
        }
    case WM_DESTROY:
        {
            m_hWnd = 0;

            break;
        }
    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case ID_FILE_EXIT:
                // Exit
                PostQuitMessage(0);
                break;

            case ID_FILE_PREFERENCES:
                // Preferences
                PrefDialog::Create(hWnd);
                break;

            case ID_FILE_VIEWDEBUG:
            case ID_VIEWDEBUG:
                // Debug
                DebugDialog::Create(hWnd);
                break;

            case ID_VIEW_DUNGEON:
                // View Dungeon
                SDDialog::Create(hWnd);
                break;

            case ID_HELP_CREDITS:
                // Show Credits
                CreditsDialog::Create(hWnd);
                break;

            case ID_TAKESCREENSHOT:
                // Screenshot
                SaveScreenshot();
                break;
            } 

            break;
        }
    }

    if (RenderScene::WndProc(MsgID, wParam, lParam))
        return 0;

    return DefWindowProc(hWnd, MsgID, wParam, lParam);
}

void MainDialog::SaveScreenshot()
{
    D3DDISPLAYMODE DisplayMode;
    if (FAILED(g_pD3DDevice->GetDisplayMode(0, &DisplayMode)))
        return;

    LPDIRECT3DSURFACE9 pSurface = NULL;
    if (FAILED(g_pD3DDevice->
        CreateOffscreenPlainSurface(DisplayMode.Width, DisplayMode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pSurface, NULL)))
        return;

    // FrontBuffer should be received as D3DFMT_A8R8G8B8
    if (SUCCEEDED(g_pD3DDevice->GetFrontBufferData(0, pSurface)))
    {
        // D3DSURFACE_DESC SurfaceDesc;
        // if (FAILED(pSurface->GetDesc(&SurfaceDesc)))
        //    return;
        UINT Width = DisplayMode.Width;
        UINT Height = DisplayMode.Height;

        D3DLOCKED_RECT Locked;
        if (SUCCEEDED(pSurface->LockRect(&Locked, NULL, D3DLOCK_READONLY)))
        {
            RGBTRIPLE *pOutput = new RGBTRIPLE[ Width * Height ];

            LPVOID pBitStream = Locked.pBits;
            RGBTRIPLE *pBitDest = pOutput;

            for (UINT y = 0; y < Height; y++)
            {
                LPVOID pNextLine = (LPVOID) (((BYTE *)pBitStream) + Locked.Pitch);

                for (UINT x = 0; x < Width; x++)
                {
                    *pBitDest = *((RGBTRIPLE *)pBitStream);
                    pBitDest += 1;

                    pBitStream = (LPVOID) (((BYTE *)pBitStream) + 4); // D3DFMT_A8R8G8B8
                }

                pBitStream = pNextLine;
            }

            pSurface->UnlockRect();

            // Ask where the file is to be saved to.
            char szFile[ MAX_PATH ];
            szFile[0] = '\0';

            OPENFILENAME ofn;
            memset(&ofn, 0, sizeof(OPENFILENAME));

            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = m_hWnd;
            ofn.lpstrFilter = "Bitmap File\0*.bmp\0\0";
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrInitialDir = Preferences::GetApplicationPath();
            ofn.lpstrTitle = "Save Screenshot As..";
            ofn.lpstrDefExt = "bmp";
            ofn.Flags = OFN_OVERWRITEPROMPT;

            if (GetSaveFileName(&ofn))
            {
                // Create the output bitmap.
                SaveBitmapFile(szFile, Width, Height, 24, pOutput);
            }

            delete [] pOutput;
        }        
    }

    pSurface->Release();
}

void SaveBitmapFile(const char *szFile, UINT Width, UINT Height, UINT Bpp, LPVOID lpData)
{
    // Bpp = Bits per Pixel
    UINT BytesPerPixel = Bpp >> 3;

    // Create the output file
    HANDLE hFile = CreateFile(szFile, GENERIC_WRITE,
        0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        DEBUGOUT("Couldn't create output bitmap file: \"%s\"!\r\n", szFile);
        return;
    }

    // Create the Bitmap Header
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER InfoHeader;

    // Scan lines must be DWORD-aligned.
    UINT LineSize = Width * BytesPerPixel;
    UINT LinePadding = 0;

    if (LineSize & 3)
        LinePadding += 4 - (LineSize & 3);

    FileHeader.bfType = 'MB';
    FileHeader.bfSize = sizeof(FileHeader) + sizeof(InfoHeader) + (LineSize + LinePadding) * Height;
    FileHeader.bfReserved1 = 0;
    FileHeader.bfReserved2 = 0;
    FileHeader.bfOffBits = sizeof(FileHeader) + sizeof(InfoHeader);

    InfoHeader.biSize = sizeof(InfoHeader);
    InfoHeader.biWidth = Width;
    InfoHeader.biHeight = Height;
    InfoHeader.biPlanes = 1;
    InfoHeader.biBitCount = Bpp;

    InfoHeader.biCompression = BI_RGB;
    InfoHeader.biSizeImage = FileHeader.bfSize;
    InfoHeader.biXPelsPerMeter = 0;
    InfoHeader.biYPelsPerMeter = 0;
    InfoHeader.biClrUsed = 0;
    InfoHeader.biClrImportant = 0;

    // Write the Bitmap File Header.
    DWORD Dummy;

    if (!WriteFile(hFile, &FileHeader, sizeof(FileHeader), &Dummy, NULL))
        DEBUGOUT("Failed writing bitmap file header!\r\n");
    if (!WriteFile(hFile, &InfoHeader, sizeof(InfoHeader), &Dummy, NULL))
        DEBUGOUT("Failed writing bitmap info header!\r\n");

    // Bitmap's prefer bottom-up format, so we're doing it that way.
    for (long y = (long)Height - 1; y >= 0; y--)
    {
        WriteFile(hFile, (BYTE *)lpData + y*LineSize, LineSize, &Dummy, NULL);

        DWORD Padding = 0;
        WriteFile(hFile, &Padding, LinePadding, &Dummy, NULL);
    }

    CloseHandle(hFile);
}




