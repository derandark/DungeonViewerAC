
#pragma once
#include <map>
#include <vector>
#include "PString.h"

class DVBlockProfile
{
public:
    DVBlockProfile();
    ~DVBlockProfile();

    WORD GetBlockID(void);
    const char* GetName(void);

    void Destroy(void);
    ULONG pack_size(void);
    ULONG Pack(BYTE **ppData, ULONG iSize);
    BOOL UnPack(BYTE **ppData, ULONG iSize);

    void SetBlockID(WORD id);
    void SetName(const char *name);

private:

    WORD        m_BlockID;
    char*        m_Name;
};

class DVProfileManager
{
public:
    DVProfileManager();
    ~DVProfileManager();

    void Destroy(void);
    ULONG pack_size(void);
    ULONG Pack(BYTE **ppData, ULONG iSize);
    void Pack(const char *FilePath);
    BOOL UnPack(BYTE **ppData, ULONG iSize);
    void UnPack(const char *FilePath);

    void ConvertFormat(void);
    void ImportFromINI(void);

    const char *GetBlockName(WORD id);
    void SetBlockName(WORD id, const char *name);    

private:

    DWORD                                m_Version;
    
    std::vector<DVBlockProfile*>        m_BlockProfiles;
    std::map<WORD, DVBlockProfile*>        m_BlockProfileMap;
};


