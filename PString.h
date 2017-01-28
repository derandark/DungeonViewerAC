
#pragma once

class PString
{
public:
    PString();
    PString(const char* szString);
    virtual ~PString();

    void Destroy();

    BOOL UnPack(BYTE** ppData, ULONG iSize);

    inline operator const char *() {
        return m_szString;
    }

    inline const char* operator =(const char* str) {
        if (m_szString)
            delete [] m_szString;

        m_Length = (DWORD)strlen(str);
        m_szString = new char[ m_Length + 1 ];
        m_szString[ m_Length ] = '\0';
        memcpy(m_szString, str, m_Length);

        return m_szString;
    }

    char *m_szString;    // ??
    DWORD m_Length;        // (0x00->0x08)
};