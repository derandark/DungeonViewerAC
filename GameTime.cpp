
//
// GameTime.cpp
//

#include "StdAfx.h"
#include "PackObj.h"
#include "PhysicsObj.h"
#include "GameTime.h"

GameTime *GameTime::current_game_time = NULL;
double GameTime::global_next_event = 0.0;

TimeOfDay::TimeOfDay()
{
    m_04 = 0;
}

BOOL TimeOfDay::UnPack(BYTE **ppData, ULONG& iSize)
{
    UNPACK(DWORD, m_04);
    UNPACK(DWORD, m_08);

    UNPACK_OBJ(m_00);

    DWORD StrPackSize = 2 + (((m_00.m_Length - 1) >= 0xFFFF)?4:0) + (m_00.m_Length - 1);
    if (StrPackSize & 3)
        StrPackSize += 4 - (StrPackSize & 3);
    iSize -= StrPackSize;

#ifdef PRE_TOD
    PACK_ALIGN();
#endif

    return((((signed)iSize) >= 0) ? TRUE : FALSE);
}

WeekDay::WeekDay()
{
}

BOOL WeekDay::UnPack(BYTE **ppData, ULONG& iSize)
{
    UNPACK_OBJ(m_00);

    DWORD StrPackSize = 2 + (((m_00.m_Length - 1) >= 0xFFFF)?4:0) + (m_00.m_Length - 1);
    if (StrPackSize & 3)
        StrPackSize += 4 - (StrPackSize & 3);
    iSize -= StrPackSize;

#ifdef PRE_TOD
    PACK_ALIGN();
#endif

    return((((signed)iSize) >= 0) ? TRUE : FALSE);
}

Season::Season()
{
    m_04 = 0;
}

BOOL Season::UnPack(BYTE **ppData, ULONG& iSize)
{
    UNPACK(DWORD, m_04);
    UNPACK_OBJ(m_00);

    DWORD StrPackSize = 2 + (((m_00.m_Length - 1) >= 0xFFFF)?4:0) + (m_00.m_Length - 1);
    if (StrPackSize & 3)
        StrPackSize += 4 - (StrPackSize & 3);
    iSize -= StrPackSize;

#ifdef PRE_TOD
    PACK_ALIGN();
#endif

    return((((signed)iSize) >= 0) ? TRUE : FALSE);
}

GameTime::GameTime()
{
    m_00 = 0.0;
    m_08 = 0;
    m_0C = 0;
    m_10 = 0;
    m_38 = "";
    m_3C = 0;
    m_40 = 0.0;
    m_48 = 0;
    m_4C = 0;
    m_50 = INVALID_TIME;
    m_58 = 0;
    m_5C = 0;
    m_60 = 0;
    m_64 = 0;
    m_68 = 0;
    m_6C = 0;
    m_70 = 0;
    m_74 = 0;
    m_78 = 0;
    m_80 = 0;
}

GameTime::~GameTime()
{
}

void GameTime::Destroy()
{
}

BOOL GameTime::UnPack(BYTE **ppData, ULONG& iSize)
{
    Destroy();

    UNPACK(double, m_00);
    UNPACK(DWORD, m_08);
    UNPACK(float, m_0C);
    UNPACK(DWORD, m_10);

    m_40 = m_0C * m_10;
    UNPACK_OBJ(m_38);

    DWORD StrPackSize = 2 + (((m_38.m_Length - 1) >= 0xFFFF)?4:0) + (m_38.m_Length - 1);
    if (StrPackSize & 3)
        StrPackSize += 4 - (StrPackSize & 3);
    iSize -= StrPackSize;

#ifdef PRE_TOD
    PACK_ALIGN();
#endif

    DWORD NumTODs;
    UNPACK(DWORD, NumTODs);

    for (DWORD i = 0; i < NumTODs; i++)
    {
        TimeOfDay *TOD = new TimeOfDay;

        if (!UNPACK_POBJ(TOD))
            return FALSE;

        m_TimeOfDays.add(&TOD);
    }

    DWORD NumDays;
    UNPACK(DWORD, NumDays);

    for (DWORD i = 0; i < NumDays; i++)
    {
        WeekDay *WD = new WeekDay;

        if (!UNPACK_POBJ(WD))
            return FALSE;

        m_WeekDays.add(&WD);
    }

    DWORD NumSeasons;
    UNPACK(DWORD, NumSeasons);

    for (DWORD i = 0; i < NumSeasons; i++)
    {
        Season *S = new Season;

        if (!UNPACK_POBJ(S))
            return FALSE;

        m_Seasons.add(&S);
    }

    if (((signed)iSize) < 0)
        return FALSE;

    return TRUE;
}

void GameTime::UseTime()
{
    double var_8 = Time::GetTimeCurrent() + m_78 + m_80;

    if (m_50 < 0.0)
        CalcDayBegin(var_8);

    m_48 = (var_8 - m_50) / m_0C;

    if (var_8 >= m_58)
    {
        if (m_48 >= 0.0f)
        {
            CalcDayBegin(var_8);
            m_48 = (var_8 - m_50) / m_0C;
        }

        CalcTimeOfDay(var_8);
        global_next_event = (m_58 - m_78) + m_80;
    }
}

void GameTime::CalcDayBegin(double arg_0)
{
    UNFINISHED("GameTime::CalcDayBegin");
}

void GameTime::CalcTimeOfDay(double arg_0)
{
    UNFINISHED("GameTime::CalcTimeOfDay");
}






