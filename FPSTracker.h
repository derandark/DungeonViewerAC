
#pragma once

class FPSTracker
{
public:

    void Init(float UpdateInterval)
    {
        m_fLastIntervalFPS = 0;
        m_fUpdateInterval = UpdateInterval;

        BeginInterval();
    }

    void Update()
    {
        if (m_fIntervalEnd <= Time::GetTimeElapsed())
        {
            double Elapsed = Time::GetTimeElapsed() - m_fIntervalStart;
            m_fLastIntervalFPS = (float)(m_iIntervalFrames / Elapsed);

            BeginInterval();
        }
    }

    void IncrementFrame()
    {
        m_iIntervalFrames++;
    }

    float GetFPS()
    {
        return m_fLastIntervalFPS;
    }

private:

    void BeginInterval()
    {
        m_iIntervalFrames = 0;
        m_fIntervalStart = Time::GetTimeElapsed();
        m_fIntervalEnd = Time::GetTimeElapsed() + m_fUpdateInterval;
    }

    DWORD m_iIntervalFrames;
    double m_fIntervalStart;
    double m_fIntervalEnd;

    float m_fUpdateInterval;
    float m_fLastIntervalFPS;
};



