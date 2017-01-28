
#include "StdAfx.h"
#include "PackObj.h"
#include "Preferences.h"
#include "DungeonProfiles.h"

DVBlockProfile::DVBlockProfile()
{
    m_BlockID = 0;
    m_Name = 0;
}

DVBlockProfile::~DVBlockProfile()
{
    Destroy();
}

void DVBlockProfile::Destroy(void)
{
    m_BlockID = 0;

    if (m_Name)
    {
        delete [] m_Name;
        m_Name = NULL;
    }
}

BOOL DVBlockProfile::UnPack(BYTE **ppData, ULONG iSize)
{
    Destroy();

    UNPACK(WORD, m_BlockID);

    WORD NameLength;
    UNPACK(WORD, NameLength);

    m_Name = new char[ NameLength + 1 ];
    m_Name[ NameLength ] = '\0';

    memcpy(m_Name, *ppData, NameLength);
    *ppData = (*ppData) + NameLength;
    iSize = (iSize) - NameLength;

    return TRUE;
}

ULONG DVBlockProfile::pack_size(void)
{
    DWORD PackSize = 0;

    PackSize += sizeof(WORD); // Block ID
    PackSize += sizeof(WORD); // Name Length
    PackSize += (m_Name ? strlen(m_Name) : 0); // Name String

    return PackSize;
}

ULONG DVBlockProfile::Pack(BYTE **ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    if (iSize >= PackSize)
    {
        PACK(WORD, m_BlockID);

        WORD NameLength = (m_Name? (WORD) strlen(m_Name) : 0);
        PACK(WORD, NameLength);
        
        memcpy(*ppData, m_Name, NameLength);
        *ppData = (*ppData) + NameLength;
        iSize = (iSize) - NameLength;
    }

    return iSize;
}

WORD DVBlockProfile::GetBlockID(void)
{
    return m_BlockID;
}

const char *DVBlockProfile::GetName(void)
{
    return (m_Name? m_Name : "");
}

void DVBlockProfile::SetBlockID(WORD id)
{
    m_BlockID = id;
}

void DVBlockProfile::SetName(const char *name)
{
    if (m_Name)
        delete [] m_Name;

    m_Name = CopyString(name);
}

DVProfileManager::DVProfileManager()
{
    m_Version = 1;
}

DVProfileManager::~DVProfileManager()
{
    Destroy();
}

void DVProfileManager::Destroy(void)
{
    for (std::vector<DVBlockProfile*>::iterator i = m_BlockProfiles.begin();
        i != m_BlockProfiles.end(); i++)
    {
        DVBlockProfile *pProfile = *i;

        if (pProfile)
            delete pProfile;
    }

    m_BlockProfiles.clear();
    m_BlockProfileMap.clear();
    m_Version = 1;
}

BOOL DVProfileManager::UnPack(BYTE **ppData, ULONG iSize)
{
    Destroy();

    UNPACK(DWORD, m_Version);

    DWORD NumBlockProfiles;
    UNPACK(DWORD, NumBlockProfiles);

    for (DWORD i = 0; i < NumBlockProfiles; i++)
    {
        DVBlockProfile *pProfile = new DVBlockProfile;
        UNPACK_POBJ(pProfile);

        m_BlockProfiles.push_back(pProfile);
        m_BlockProfileMap[ pProfile->GetBlockID() ] = pProfile;
    }

    ConvertFormat();

    return TRUE;
}

void DVProfileManager::ConvertFormat(void)
{
    if (m_Version == 1)
        return;

    DEBUGOUT ("Bad profile version.\r\n");
}

ULONG DVProfileManager::pack_size(void)
{
    DWORD PackSize = 0;

    PackSize += sizeof(DWORD); // Version
    PackSize += sizeof(DWORD); // Num Block Profiles

    for (std::vector<DVBlockProfile*>::iterator i = m_BlockProfiles.begin();
        i != m_BlockProfiles.end(); i++)
    {
        PackSize += (*i)->pack_size();
    }

    return PackSize;
}

ULONG DVProfileManager::Pack(BYTE **ppData, ULONG iSize)
{
    ULONG PackSize = pack_size();

    if (iSize >= PackSize)
    {
        PACK(DWORD, m_Version);
        PACK(DWORD, m_BlockProfiles.size());
        
        for (std::vector<DVBlockProfile*>::iterator i = m_BlockProfiles.begin();
            i != m_BlockProfiles.end(); i++)
        {
            PACK_POBJ((*i));
        }
    }

    return iSize;
}

void DVProfileManager::Pack(const char *FilePath)
{
    DWORD FileSize = pack_size();
    
    FILE *output = fopen(FilePath, "wb");
    if (output)
    {
        BYTE *buffer = new BYTE[ FileSize ];

        BYTE *stream = buffer;
        Pack(&stream, FileSize);

        fwrite(buffer, FileSize, 1, output);
        fclose(output);
    }
}

void DVProfileManager::UnPack(const char *FilePath)
{
    FILE *input = fopen(FilePath, "rb");
    if (input)
    {
        fseek(input, 0, SEEK_END);
        DWORD FileSize = (DWORD)ftell(input);
        fseek(input, 0, SEEK_SET);

        BYTE *buffer = new BYTE[ FileSize ];
        BYTE *stream = buffer;
        fread(buffer, FileSize, 1, input);
        UnPack(&stream, FileSize);

        fclose(input);
    }
}

void DVProfileManager::ImportFromINI(void)
{
    char Name[512];

    for (DWORD i = 0; i <= 0xFFFF; i++)
    {
        char SectionName[24];
        sprintf(SectionName, "Dungeon 0x%04X", i);
    
        if (GetPrivateProfileString(SectionName, "Name", "", Name, sizeof(Name), Preferences::GetDungeonINIPath()) > 0)
            SetBlockName(i, Name);
    }

    Pack(Preferences::GetDVProfilePath());
}

const char *DVProfileManager::GetBlockName(WORD id)
{
    std::map<WORD, DVBlockProfile*>::iterator
        i = m_BlockProfileMap.find(id);

    return (i != m_BlockProfileMap.end()) ? i->second->GetName() : "";
}

void DVProfileManager::SetBlockName(WORD id, const char *name)
{
    std::map<WORD, DVBlockProfile*>::iterator
        i = m_BlockProfileMap.find(id);

    if (i == m_BlockProfileMap.end())
    {
        DVBlockProfile *pProfile = new DVBlockProfile;
        
        pProfile->SetBlockID(id);
        pProfile->SetName(name);

        m_BlockProfiles.push_back(pProfile);
        m_BlockProfileMap[ id ] = pProfile;
    }
    else
    {
        DVBlockProfile *pProfile = i->second;
        
        pProfile->SetName(name);
    }
}




