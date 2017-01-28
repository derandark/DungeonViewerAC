
#include "StdAfx.h"
#include "Globals.h"

UINT64    Time::m_iCounterStart = 0;
UINT64    Time::m_iCounterFrequency = 1;
double    Time::m_fTimeElapsed = 0;

void Time::Init()
{
    QueryPerformanceCounter((LARGE_INTEGER *)&m_iCounterStart);
    QueryPerformanceFrequency((LARGE_INTEGER *)&m_iCounterFrequency);
}

void Time::Update()
{
    UINT64 CurrentCounter;
    QueryPerformanceCounter((LARGE_INTEGER *)&CurrentCounter);
    CurrentCounter -= m_iCounterStart;

    m_fTimeElapsed = CurrentCounter / (double)m_iCounterFrequency;
}

double Time::GetTimeElapsed()
{
    return m_fTimeElapsed;
}

// Same thing, matches name the client might use?
double Time::GetTimeCurrent()
{
    return m_fTimeElapsed;
}

HINSTANCE Globals::m_hInstance;

void Globals::Init(HINSTANCE hInstance)
{
    m_hInstance = hInstance;

    Time::Init();
}

HINSTANCE Globals::GetAppInstance()
{
    return m_hInstance;
}

void Globals::GetScreenDimensions(long *plWidth, long *plHeight)
{
    RECT ScreenRect;
    GetWindowRect(GetDesktopWindow(), &ScreenRect);
    *plWidth = ScreenRect.right - ScreenRect.left;
    *plHeight = ScreenRect.bottom - ScreenRect.top;
}

void Globals::Cleanup()
{
}

GlobalResource::GlobalResource(LPCTSTR name, LPCTSTR type)
{
    HRSRC hResourceInfo = FindResource(NULL, name, type);

    if (hResourceInfo)
    {
        m_hResource = LoadResource(NULL, hResourceInfo);

        if (m_hResource)
        {
            m_lpData = LockResource(m_hResource);
            m_dwSize = SizeofResource(NULL, hResourceInfo);
        }
        else
        {
            m_lpData = NULL;
            m_dwSize = 0;
        }
    }
    else
    {
        m_hResource = NULL;
        m_lpData = NULL;
        m_dwSize = 0;
    }
}

GlobalResource::~GlobalResource()
{
    if (m_hResource)
    {
        FreeResource(m_hResource);
        m_hResource = NULL;
    }
}

LPVOID GlobalResource::GetData(void)
{
    return m_lpData;
}

DWORD GlobalResource::GetSize(void)
{
    return m_dwSize;
}







