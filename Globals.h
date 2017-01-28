
#pragma once

class Time
{
public:
    static void Init();
    static void Update();

    static double GetTimeElapsed();
    static double GetTimeCurrent();
    
private:
    static UINT64 m_iCounterStart;
    static UINT64 m_iCounterFrequency;

    static double m_fTimeElapsed;
};

class Globals
{
public:
    static void Init(HINSTANCE hInstance);
    static void Cleanup();

    static HINSTANCE GetAppInstance();
    static HGLOBAL GetResource(LPCTSTR name, LPCTSTR type);
    static void GetScreenDimensions(long *plWidth, long *plHeight);

private:

    static HINSTANCE m_hInstance;
};

class GlobalResource
{
public:
    GlobalResource(LPCTSTR name, LPCTSTR type);
    ~GlobalResource();

    LPVOID    GetData();
    DWORD    GetSize();

private:

    HGLOBAL m_hResource;
    LPVOID    m_lpData;
    DWORD    m_dwSize;
};


