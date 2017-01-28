
#include "StdAfx.h"
#include "PackObj.h"
#include "PString.h"

PString::PString() {
    m_szString = NULL;
    m_Length = 0;
}

PString::PString(const char* szString)
{
    if (!szString) {
        m_Length = 0;
        m_szString = new char[1];
        *m_szString = '\0';
    } else {
        m_Length = (DWORD)strlen(szString);
        m_szString = new char[ m_Length + 1 ];
        m_szString[ m_Length ] = '\0';
        memcpy(m_szString, szString, m_Length);
    }
}

PString::~PString() {
    Destroy();
}

void PString::Destroy()
{
    if (m_szString)
    {
        delete [] m_szString;
        m_szString = NULL;
    }
}

BOOL PString::UnPack(BYTE** ppData, ULONG iSize)
{
    DWORD Length;

    // Default pack includes 16-bit length.
    WORD Length16;
    if (!UNPACK(WORD, Length16))
        return FALSE;

    if (Length16 == 0xFFFF)
    {
        // If necessary will include 32-bit length.
        DWORD Length32;
        if (!UNPACK(DWORD, Length32))
            return FALSE;

        Length = Length32;
    }
    else
        Length = Length16;

    if (iSize < Length)
        return FALSE;

    // Not the real code.
    m_Length = Length + 1;
    m_szString = new char[ m_Length ];
    memcpy(m_szString, *ppData, Length);
    m_szString[ Length ] = '\0';
    *ppData += Length;
    iSize -= Length;

#ifdef PRE_TOD
    PACK_ALIGN();
#endif

    return TRUE;
}



