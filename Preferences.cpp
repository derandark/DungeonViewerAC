
#include "StdAfx.h"
#include "Preferences.h"
#include "Render.h"

DWORD RenderPreferences::RenderHud;
DWORD RenderPreferences::RenderObjects;
DWORD RenderPreferences::RenderLights;
DWORD RenderPreferences::RenderFullbrightOutsideCells;
DWORD RenderPreferences::NoSleep;
DWORD RenderPreferences::FillMode;
DWORD RenderPreferences::MultiSample;
DWORD RenderPreferences::LightMod;

void RenderPreferences::LoadFromRegistry(void)
{
	if (!Preferences::GetRegistryInt("RenderHud", &RenderHud))
		RenderHud = TRUE;

	if (!Preferences::GetRegistryInt("RenderObjects", &RenderObjects))
		RenderObjects = TRUE;

	if (!Preferences::GetRegistryInt("RenderLights", &RenderLights))
		RenderLights = TRUE;

	if (!Preferences::GetRegistryInt("RenderFullbrightOutsideCells", &RenderFullbrightOutsideCells))
		RenderFullbrightOutsideCells = TRUE;

	if (!Preferences::GetRegistryInt("NoSleep", &NoSleep))
		NoSleep = FALSE;

	if (!Preferences::GetRegistryInt("FillMode", &FillMode))
		FillMode = D3DFILL_SOLID;

	if (!Preferences::GetRegistryInt("MultiSample", &MultiSample))
		MultiSample = D3DMULTISAMPLE_NONE;

	if (!Preferences::GetRegistryInt("LightMod", &LightMod))
		LightMod = 0;
}

void RenderPreferences::SaveToRegistry(void)
{
	Preferences::SetRegistryInt("RenderHud", RenderHud);
	Preferences::SetRegistryInt("RenderObjects", RenderObjects);
	Preferences::SetRegistryInt("RenderLights", RenderLights);
	Preferences::SetRegistryInt("RenderFullbrightOutsideCells", RenderFullbrightOutsideCells);
	Preferences::SetRegistryInt("NoSleep", NoSleep);
	Preferences::SetRegistryInt("FillMode", FillMode);
	Preferences::SetRegistryInt("MultiSample", MultiSample);
	Preferences::SetRegistryInt("LightMod", LightMod);
}

BOOL Preferences::CheckIfFirstRun()
{
	static BOOL Checked = FALSE;
	static BOOL FirstRun;

	if (!Checked)
	{
		DWORD Value = TRUE;

		if (!GetRegistryInt("FirstRun", &Value) || Value)
		{
			FirstRun = TRUE;
			SetRegistryInt("FirstRun", 0);
		}
		else
			FirstRun = FALSE;

		Checked = TRUE;
	}

	return FirstRun;
}

const char *Preferences::GetApplicationPath()
{
	static char AppPath[MAX_PATH+10];
	/*
	GetModuleFileName(NULL, AppPath, sizeof(AppPath));
	char *p = strrchr(AppPath, '\\');
	if (p) // chop off filename
	p[1] = '\0';
	*/

	GetCurrentDirectory(MAX_PATH, AppPath);
	strcat(AppPath, "\\");

	return AppPath;
}

const char *Preferences::GetDungeonINIPath()
{
	static char DungeonINIPath[MAX_PATH];
	sprintf(DungeonINIPath, "%s\\dungeons.ini", GetApplicationPath());

	return DungeonINIPath;
}

const char *Preferences::GetDVProfilePath()
{
	static char DVProfilePath[MAX_PATH];
	sprintf(DVProfilePath, "%s\\dungeons.dvp", GetApplicationPath());

	return DVProfilePath;
}

const char* Preferences::GetPortalDATPath()
{
	/*
	const char* Path = GetRegistryStr("PortalPath");

	if (*Path)
		return Path;
	*/

	// Default to AC portal.dat.
	const char* ACPath = GetACPath();

	if (*ACPath)
	{
		static char DefaultPortal[MAX_PATH];
		strcpy(DefaultPortal, ACPath);
#ifdef PRE_TOD
		strcat(DefaultPortal, "portal.dat");
#else
		strcat(DefaultPortal, "client_portal.dat");
#endif

		return DefaultPortal;
	}
	else
		return "";
}

const char*    Preferences::GetCellDATPath()
{
	/*
	const char* Path = GetRegistryStr("CellPath");

	if (*Path)
		return Path;
		*/

	// Default to AC portal.dat.
	const char* ACPath = GetACPath();

	if (*ACPath)
	{
		static char DefaultCell[MAX_PATH];
		strcpy(DefaultCell, ACPath);
#ifdef PRE_TOD
		strcat(DefaultCell, "cell.dat");
#else
		strcat(DefaultCell, "client_cell_1.dat");
#endif

		return DefaultCell;
	}
	else
		return "";
}

TCHAR *GetAppFolder(_Out_ TCHAR *PathBuffer)
{
	GetModuleFileName(0, PathBuffer, MAX_PATH);
	PathBuffer[MAX_PATH - 1] = 0;
	PathRemoveFileSpec(PathBuffer);

	return PathBuffer;
}

TCHAR *GetAppFile(_In_ const TCHAR *filename, _Out_ TCHAR *pathBuffer)
{
	TCHAR Folder[MAX_PATH];
	_sntprintf(pathBuffer, MAX_PATH, TEXT("%s\\%s"), GetAppFolder(Folder), filename);
	pathBuffer[MAX_PATH - 1] = 0;
	return pathBuffer;
}

const char *Preferences::GetACPath()
{
#ifdef PRE_TOD
	/*
   const char* ACKeyRoot = "Software\\Microsoft\\Microsoft Games\\Asheron's Call\\1.00";

	HKEY hTempKey = 0;

	static char ACPath[MAX_PATH];
	memset(ACPath, 0, sizeof(ACPath));
	DWORD ACPathSize = sizeof(ACPath)-1;

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, ACKeyRoot, NULL, KEY_QUERY_VALUE, &hTempKey))
	{
		if (ERROR_SUCCESS == RegQueryValueEx(hTempKey, "Portal Dat", NULL, NULL, (BYTE *)ACPath, &ACPathSize))
		{
			if (ACPath[ ACPathSize-2 ] != '\\')
			{
				ACPath[ ACPathSize-1 ] = '\\';
				ACPath[ ACPathSize ] = 0;
			}
		}
	}

	if (hTempKey)
		RegCloseKey(hTempKey);

	return ACPath;
	*/

	/*
	static char currentFolder[MAX_PATH + 10];
	GetAppFolder(currentFolder);
	return currentFolder;
	*/

	static char currentFolder[MAX_PATH + 10];
	GetCurrentDirectory(MAX_PATH, currentFolder);
	strcat(currentFolder, "\\");
	return currentFolder;

#else
	// return "e:\\program files\\turbine\\Asheron's Call - Throne of Destiny\\";

	static char currentFolder[MAX_PATH + 10];
	GetCurrentDirectory(MAX_PATH, currentFolder);
	strcat(currentFolder, "\\");
	return currentFolder;
#endif
}

COLORREF Preferences::GetTextColor()
{
	COLORREF Color;

	if (!GetRegistryInt("TextColor", &Color))
		Color = 0x00FFFFFF;

	return Color;
}

COLORREF Preferences::GetFillColor()
{
	COLORREF Color;

	if (!GetRegistryInt("FillColor", &Color))
		Color = 0x00000040;

	return Color;
}

void Preferences::SetPortalDATPath(const char *Path)
{
	SetRegistryStr("PortalPath", Path);
}

void Preferences::SetCellDATPath(const char *Path)
{
	SetRegistryStr("CellPath", Path);
}

void Preferences::SetTextColor(COLORREF Color)
{
	Render::SetTextColor(Color);

	return SetRegistryInt("TextColor", Color);
}

void Preferences::SetFillColor(COLORREF Color)
{
	Render::SetFillColor(Color);

	return SetRegistryInt("FillColor", Color);
}

const char* Preferences::GetRegistryStr(const char *ValueName)
{
	HKEY hTempKey = 0;

	static char StringValue[512];
	memset(StringValue, 0, sizeof(StringValue));
	DWORD StringSize = sizeof(StringValue);

	if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, PREFERENCES_KEY_ROOT, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hTempKey, NULL))
	{
		if (ERROR_SUCCESS == RegQueryValueEx(hTempKey, ValueName, NULL, NULL, (BYTE *)StringValue, &StringSize))
		{
		}
	}

	if (hTempKey)
		RegCloseKey(hTempKey);

	return StringValue;
}

void Preferences::SetRegistryStr(const char *ValueName, const char *Value)
{
	HKEY hTempKey;

	if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, PREFERENCES_KEY_ROOT, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hTempKey, NULL))
	{
		DWORD StringSize = (DWORD)strlen(Value) + 1;

		if (ERROR_SUCCESS == RegSetValueEx(hTempKey, ValueName, NULL, REG_SZ, (BYTE *)Value, StringSize))
		{
		}
	}

	if (hTempKey)
		RegCloseKey(hTempKey);
}

BOOL Preferences::GetRegistryInt(const char *ValueName, DWORD *pValue)
{
	HKEY hTempKey = 0;

	BOOL bReturn = FALSE;
	DWORD IntValue = 0;
	DWORD IntSize = sizeof(IntValue);

	if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, PREFERENCES_KEY_ROOT, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hTempKey, NULL))
	{
		if (ERROR_SUCCESS == RegQueryValueEx(hTempKey, ValueName, NULL, NULL, (BYTE *)&IntValue, &IntSize))
		{
			*pValue = IntValue;
			bReturn = TRUE;
		}
	}

	if (hTempKey)
		RegCloseKey(hTempKey);

	return bReturn;
}

void Preferences::SetRegistryInt(const char *ValueName, DWORD Value)
{
	HKEY hTempKey;

	if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, PREFERENCES_KEY_ROOT, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hTempKey, NULL))
	{
		if (ERROR_SUCCESS == RegSetValueEx(hTempKey, ValueName, NULL, REG_DWORD, (BYTE *)&Value, sizeof(DWORD)))
		{
		}
	}

	if (hTempKey)
		RegCloseKey(hTempKey);
}







