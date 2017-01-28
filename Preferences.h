
#pragma once

#define PREFERENCES_KEY_ROOT "SOFTWARE\\Pea\\DungeonViewer"

class RenderPreferences
{
public:
    static void LoadFromRegistry(void);
    static void SaveToRegistry(void);

    static DWORD RenderHud;
    static DWORD RenderObjects;
    static DWORD RenderLights;
	static DWORD RenderFullbrightOutsideCells;
    static DWORD NoSleep;
    static DWORD FillMode;
    static DWORD MultiSample;
	static DWORD LightMod;
};

// Why is this a bunch of static stuff? Change to a namespace or encapsulate it in a real class dummy
class Preferences
{
    friend class RenderPreferences;

public:
    static BOOL            CheckIfFirstRun();

    static const char*    GetApplicationPath();
    static const char*    GetDungeonINIPath();
    static const char*    GetDVProfilePath();
    static const char*    GetACPath();
    static const char*    GetPortalDATPath();
    static const char*    GetCellDATPath();
    static COLORREF        GetTextColor();
    static COLORREF        GetFillColor();

    static void            SetPortalDATPath(const char *Path);
    static void            SetCellDATPath(const char *Path);
    static void            SetTextColor(COLORREF Color);
    static void            SetFillColor(COLORREF Color);

private:

    static const char*    GetRegistryStr(const char *ValueName);
    static void            SetRegistryStr(const char *ValueName, const char *Value);

    static BOOL            GetRegistryInt(const char *ValueName, DWORD *pValue);
    static void            SetRegistryInt(const char *ValueName, DWORD Value);

};
