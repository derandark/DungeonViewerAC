
//
// GameTime.h
//

#pragma once
#include "PString.h"
#include "SmartArray.h"

class TimeOfDay
{
public:
    TimeOfDay();

    BOOL UnPack(BYTE **ppData, ULONG& iSize);

    PString m_00;
    DWORD m_04;
    DWORD m_08;
};

class WeekDay
{
public:
    WeekDay();

    BOOL UnPack(BYTE **ppData, ULONG& iSize);

    PString m_00;
};

class Season
{
public:
    Season();

    BOOL UnPack(BYTE **ppData, ULONG& iSize);

    PString m_00;
    DWORD m_04;
};

class GameTime
{
public:
    GameTime();
    ~GameTime();

    void Destroy();
    BOOL UnPack(BYTE **ppData, ULONG& iSize);

    void UseTime();
    void CalcDayBegin(double arg_0);
    void CalcTimeOfDay(double arg_0);

    double m_00;
    DWORD m_08;
    float m_0C;
    DWORD m_10;
    SmartArray<TimeOfDay *>    m_TimeOfDays;
    SmartArray<WeekDay *>    m_WeekDays;
    SmartArray<Season *>    m_Seasons;
    PString m_38;
    DWORD m_3C;
    double m_40;
    float m_48;
    DWORD m_4C;
    double m_50;
    DWORD m_58;
    DWORD m_5C;
    DWORD m_60;
    DWORD m_64;
    DWORD m_68;
    DWORD m_6C;
    DWORD m_70;
    DWORD m_74;
    double m_78;
    double m_80;

    static GameTime *current_game_time;
    static double global_next_event;
};
