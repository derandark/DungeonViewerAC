
#include "StdAfx.h"
#include "PrefDialog.h"
#include "Preferences.h"
#include "Render.h"

HWND        PrefDialog::m_hWnd = 0;
HWND        PrefDialog::m_hFillMode = 0;
HWND        PrefDialog::m_hMultiSample = 0;
D3DCOLOR    PrefDialog::m_TextColor = 0;
D3DCOLOR    PrefDialog::m_FillColor = 0;

void PrefDialog::Create(HWND hParent)
{
    if (m_hWnd)
        return;

    // Create the window.
    m_hWnd = CreateDialog(Globals::GetAppInstance(), (LPCTSTR)IDD_PREFERENCES, hParent, (DLGPROC)DlgProc);


    if (!m_hWnd)
    {
        // Failure.
        MessageBox(hParent, "Failed creating preferences dialog!", "Error", MB_ICONHAND);
    }
}

void PrefDialog::SaveSettings()
{
	/*
    char PortalPath[MAX_PATH+1], CellPath[MAX_PATH+1];
    
    // Incase anything goes wrong..
    PortalPath[0] = '\0';
    PortalPath[MAX_PATH] = '\0';
    CellPath[0] = '\0';
    CellPath[MAX_PATH] = '\0';

    GetWindowText(GetDlgItem(m_hWnd, IDC_PORTALDATPATH), PortalPath, MAX_PATH);
    GetWindowText(GetDlgItem(m_hWnd, IDC_CELLDATPATH), CellPath, MAX_PATH);

    Preferences::SetPortalDATPath(PortalPath);
    Preferences::SetCellDATPath(CellPath);
	*/

    Preferences::SetTextColor(m_TextColor);
    Preferences::SetFillColor(m_FillColor);

    RenderPreferences::SaveToRegistry();
}

void PrefDialog::LoadSettings()
{
    SetWindowText(GetDlgItem(m_hWnd, IDC_PORTALDATPATH), "Place your DAT files inside the same folder as this program.");
    SetWindowText(GetDlgItem(m_hWnd, IDC_CELLDATPATH), "Place your DAT files inside the same folder as this program.");

    m_TextColor = Preferences::GetTextColor();
    m_FillColor = Preferences::GetFillColor();

    UpdateColorEditControl(IDC_TCEDIT, m_TextColor);
    UpdateColorEditControl(IDC_FCEDIT, m_FillColor);

    RenderPreferences::LoadFromRegistry();

    SendMessage(GetDlgItem(m_hWnd, IDC_RENDERSTATS), BM_SETCHECK, 
        RenderPreferences::RenderHud ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(GetDlgItem(m_hWnd, IDC_RENDEROBJECTS), BM_SETCHECK, 
        RenderPreferences::RenderObjects ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(GetDlgItem(m_hWnd, IDC_RENDERLIGHTS), BM_SETCHECK, 
        RenderPreferences::RenderLights ? BST_CHECKED : BST_UNCHECKED, 0);
	SendMessage(GetDlgItem(m_hWnd, IDC_FULLBRIGHT), BM_SETCHECK,
		RenderPreferences::RenderFullbrightOutsideCells ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(GetDlgItem(m_hWnd, IDC_NOSLEEP), BM_SETCHECK, 
        RenderPreferences::NoSleep ? BST_CHECKED : BST_UNCHECKED, 0);
	SendMessage(GetDlgItem(m_hWnd, IDC_LIGHTMOD), TBM_SETRANGE, TRUE, MAKELONG(0, 100));
	SendMessage(GetDlgItem(m_hWnd, IDC_LIGHTMOD), TBM_SETPOS, TRUE, RenderPreferences::LightMod);

    // Filling options.
    SendMessage(m_hFillMode, CB_ADDSTRING, 0, (LPARAM)"Filled");
    SendMessage(m_hFillMode, CB_ADDSTRING, 0, (LPARAM)"Wireframe");
    SendMessage(m_hFillMode, CB_ADDSTRING, 0, (LPARAM)"Vertices");
    SendMessage(m_hFillMode, CB_SETITEMDATA, 0, (LPARAM)D3DFILL_SOLID);
    SendMessage(m_hFillMode, CB_SETITEMDATA, 1, (LPARAM)D3DFILL_WIREFRAME);
    SendMessage(m_hFillMode, CB_SETITEMDATA, 2, (LPARAM)D3DFILL_POINT);

    switch (RenderPreferences::FillMode)
    {
    case D3DFILL_SOLID:    
        SendMessage(m_hFillMode, CB_SETCURSEL, 0, 0);

        break;
    case D3DFILL_WIREFRAME:
        SendMessage(m_hFillMode, CB_SETCURSEL, 1, 0);

        break;
    case D3DFILL_POINT:
        SendMessage(m_hFillMode, CB_SETCURSEL, 2, 0);

        break;
    }

    // Multisample options.
    #define ADD_MULTISAMPLE_TO_LIST(x, xstr) \
        if (SUCCEEDED(g_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_D16, FALSE, (D3DMULTISAMPLE_TYPE)x, NULL))) \
            SendMessage(m_hMultiSample, CB_SETITEMDATA, \
                SendMessage(m_hMultiSample, CB_ADDSTRING, 0, (LPARAM)xstr), (LPARAM)x);

    ADD_MULTISAMPLE_TO_LIST(0, "None");
    ADD_MULTISAMPLE_TO_LIST(2, "2X");
    ADD_MULTISAMPLE_TO_LIST(3, "3X");
    ADD_MULTISAMPLE_TO_LIST(4, "4X");
    ADD_MULTISAMPLE_TO_LIST(5, "5X");
    ADD_MULTISAMPLE_TO_LIST(6, "6X");
    ADD_MULTISAMPLE_TO_LIST(7, "7X");
    ADD_MULTISAMPLE_TO_LIST(8, "8X");
    ADD_MULTISAMPLE_TO_LIST(9, "9X");
    ADD_MULTISAMPLE_TO_LIST(10, "10X");
    ADD_MULTISAMPLE_TO_LIST(11, "11X");
    ADD_MULTISAMPLE_TO_LIST(12, "12X");
    ADD_MULTISAMPLE_TO_LIST(13, "13X");
    ADD_MULTISAMPLE_TO_LIST(14, "14X");
    ADD_MULTISAMPLE_TO_LIST(15, "15X");
    ADD_MULTISAMPLE_TO_LIST(16, "16X");

    int count = (int)SendMessage(m_hMultiSample, CB_GETCOUNT, 0, 0);

    for (int i = 0; i < count; i++)
    {
        int val = (int)SendMessage(m_hMultiSample, CB_GETITEMDATA, i, 0);

        if (val != CB_ERR && ((DWORD)val == RenderPreferences::MultiSample))
        {
            SendMessage(m_hMultiSample, CB_SETCURSEL, i, 0);
            break;
        }
    }
}

void PrefDialog::BrowseForPortal()
{
    char PortalPath[ MAX_PATH ];
    *PortalPath = '\0';

    OPENFILENAME OpenFileName;

    OpenFileName.lStructSize = sizeof(OPENFILENAME);
    OpenFileName.hwndOwner = m_hWnd;
    OpenFileName.hInstance = NULL;
    OpenFileName.lpstrFilter = ".DAT File\0*.dat\0All Files\0*\0\0";
    OpenFileName.lpstrCustomFilter = NULL;
    OpenFileName.nMaxCustFilter = 0;
    OpenFileName.nFilterIndex = 0;
    OpenFileName.lpstrFile = PortalPath;
    OpenFileName.nMaxFile = sizeof(PortalPath);
    OpenFileName.lpstrFileTitle = NULL;
    OpenFileName.nMaxFileTitle = 0;
    OpenFileName.lpstrInitialDir = Preferences::GetACPath();
    OpenFileName.lpstrTitle = "Select Portal.DAT File";
    OpenFileName.nFileOffset = 0;
    OpenFileName.nFileExtension = 0;
    OpenFileName.lpstrDefExt = NULL;
    OpenFileName.lCustData = (long)NULL;
    OpenFileName.lpfnHook = NULL;
    OpenFileName.lpTemplateName = NULL;
    OpenFileName.Flags = OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&OpenFileName))
    {
        SetWindowText(GetDlgItem(m_hWnd, IDC_PORTALDATPATH), PortalPath);
    }
}

void PrefDialog::BrowseForCell()
{
    char CellPath[ MAX_PATH ];
    *CellPath = '\0';

    OPENFILENAME OpenFileName;

    OpenFileName.lStructSize = sizeof(OPENFILENAME);
    OpenFileName.hwndOwner = m_hWnd;
    OpenFileName.hInstance = NULL;
    OpenFileName.lpstrFilter = ".DAT File\0*.dat\0All Files\0*\0\0";
    OpenFileName.lpstrCustomFilter = NULL;
    OpenFileName.nMaxCustFilter = 0;
    OpenFileName.nFilterIndex = 0;
    OpenFileName.lpstrFile = CellPath;
    OpenFileName.nMaxFile = sizeof(CellPath);
    OpenFileName.lpstrFileTitle = NULL;
    OpenFileName.nMaxFileTitle = 0;
    OpenFileName.lpstrInitialDir = Preferences::GetACPath();
    OpenFileName.lpstrTitle = "Select Cell.DAT File";
    OpenFileName.nFileOffset = 0;
    OpenFileName.nFileExtension = 0;
    OpenFileName.lpstrDefExt = NULL;
    OpenFileName.lCustData = (long)NULL;
    OpenFileName.lpfnHook = NULL;
    OpenFileName.lpTemplateName = NULL;
    OpenFileName.Flags = OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&OpenFileName))
    {
        SetWindowText(GetDlgItem(m_hWnd, IDC_CELLDATPATH), CellPath);
    }
}

void PrefDialog::UpdateColorEditControl(int Control, COLORREF Color)
{
    char ColorString[32];

    sprintf(ColorString,
        "#%02X%02X%02X",
        Color & 0xFF, (Color >> 8) & 0xFF, (Color >> 16) & 0xFF);

    SetWindowText(GetDlgItem(m_hWnd, Control), ColorString);
}

D3DCOLOR PrefDialog::ChooseColor(D3DCOLOR DefaultColor)
{
    static COLORREF CustomColors[16];

    CHOOSECOLOR CC;

    CC.lStructSize = sizeof(CHOOSECOLOR);
    CC.hInstance = 0;
    CC.hwndOwner = m_hWnd;
    CC.rgbResult = DefaultColor;
    CC.lpCustColors = CustomColors;
    CC.Flags = CC_FULLOPEN | CC_RGBINIT;
    CC.lCustData = NULL;
    CC.lpfnHook = NULL;
    CC.lpTemplateName = NULL;

    if (::ChooseColor(&CC))
        return CC.rgbResult;
    else
        return DefaultColor;
}

void PrefDialog::ChooseTextColor()
{
    m_TextColor = ChooseColor(m_TextColor);

    UpdateColorEditControl(IDC_TCEDIT, m_TextColor);
}

void PrefDialog::ChooseFillColor()
{
    m_FillColor = ChooseColor(m_FillColor);

    UpdateColorEditControl(IDC_FCEDIT, m_FillColor);
}

INT_PTR PrefDialog::DlgProc(HWND hWnd, UINT MsgID, WPARAM wParam, LPARAM lParam)
{
    switch(MsgID)
    {
    case WM_INITDIALOG:
        {
            m_hWnd = hWnd;
            m_hFillMode = GetDlgItem(m_hWnd, IDC_FILLMODE);
            m_hMultiSample = GetDlgItem(m_hWnd, IDC_MULTISAMPLE);

            LoadSettings();

            return TRUE;
        }
    case WM_CLOSE:
        {
            // Load old render settings.
            RenderPreferences::LoadFromRegistry();

            DestroyWindow(hWnd);

            return TRUE;
        }
    case WM_DESTROY:
        {
            m_hWnd = 0;

            return TRUE;
        }

	case WM_HSCROLL:        // Trackbar scroll
		if ((HWND)lParam == GetDlgItem(hWnd, IDC_LIGHTMOD))
		{
			RenderPreferences::LightMod = (LONG)SendMessage(GetDlgItem(hWnd, IDC_LIGHTMOD), TBM_GETPOS, 0, 0);
		}
		break;

    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
            case IDOK:
                SaveSettings();
                DestroyWindow(hWnd);

                break;
            case IDCANCEL:
                // Load old render settings.
                RenderPreferences::LoadFromRegistry();

                DestroyWindow(hWnd);

                break;
            case IDC_BROWSEPORTAL:
                BrowseForPortal();

                break;
            case IDC_BROWSECELL:
                BrowseForCell();

                break;
            case IDC_CHOOSETC:
                ChooseTextColor();

                break;
            case IDC_CHOOSEFC:
                ChooseFillColor();

                break;

            case IDC_RENDERSTATS:
                if (HIWORD(wParam) == BN_CLICKED)
                {
                    RenderPreferences::RenderHud = ((BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_RENDERSTATS), BM_GETCHECK, 0, 0))? TRUE : FALSE);
                }

                break;
            case IDC_RENDEROBJECTS:
                if (HIWORD(wParam) == BN_CLICKED)
                {
                    RenderPreferences::RenderObjects = ((BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_RENDEROBJECTS), BM_GETCHECK, 0, 0))? TRUE : FALSE);
                }

                break;
            case IDC_RENDERLIGHTS:
                if (HIWORD(wParam) == BN_CLICKED)
                {
                    RenderPreferences::RenderLights = ((BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_RENDERLIGHTS), BM_GETCHECK, 0, 0))? TRUE : FALSE);
                }

                break;
			case IDC_LIGHTMOD:

				break;
			case IDC_FULLBRIGHT:
				if (HIWORD(wParam) == BN_CLICKED)
				{
					RenderPreferences::RenderFullbrightOutsideCells = ((BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_FULLBRIGHT), BM_GETCHECK, 0, 0)) ? TRUE : FALSE);
				}

				break;
            case IDC_NOSLEEP:
                if (HIWORD(wParam) == BN_CLICKED)
                {
                    RenderPreferences::NoSleep = ((BST_CHECKED == SendMessage(GetDlgItem(hWnd, IDC_NOSLEEP), BM_GETCHECK, 0, 0))? TRUE : FALSE);
                }

                break;

            case IDC_FILLMODE:
                if (HIWORD(wParam) == CBN_SELCHANGE)
                {
                    int Index = (int)SendMessage(m_hFillMode, CB_GETCURSEL, 0, 0);
                    
                    if (Index >= 0)
                    {
                        int Fill = (int)SendMessage(m_hFillMode, CB_GETITEMDATA, Index, 0);

                        RenderPreferences::FillMode = Fill;

                        if (g_pD3DDevice)
                            Render::SetFilling((D3DFILLMODE)Fill);
                    }
                }

                break;

            case IDC_MULTISAMPLE:
                if (HIWORD(wParam) == CBN_SELCHANGE)
                {
                    int Index = (int)SendMessage(m_hMultiSample, CB_GETCURSEL, 0, 0);
                    
                    if (Index >= 0)
                    {
                        int MultiSample = (int)SendMessage(m_hMultiSample, CB_GETITEMDATA, Index, 0);

                        RenderPreferences::MultiSample = MultiSample;
                        Render::SetMultiSample((D3DMULTISAMPLE_TYPE) MultiSample);
                    }
                }

                break;
            }

            return TRUE;
        }
    }

    return FALSE;
}

