#include "stdafx.h"
#include "FrameRateCounter.h"

CFrameRateCounter::CFrameRateCounter(unsigned long queueLenth)
{
	m_dFrameRate = 0.0;
	m_PrevTime = 0;
	m_nPeriod = 0;

	for (unsigned int i = 0; i < queueLenth; i++)
	{
		m_FrameTime.push_back(0);
	}

	::QueryPerformanceFrequency((LARGE_INTEGER*)&m_Frequency);
	::QueryPerformanceCounter((LARGE_INTEGER*)&m_PrevTime);
}

CFrameRateCounter::~CFrameRateCounter()
{

}

double CFrameRateCounter::GetFrameRate()
{
	return m_dFrameRate;
}

int CFrameRateCounter::GetPeriod()
{
	return m_nPeriod;
}

void CFrameRateCounter::NewFrame()
{
	__int64 CurrentTime;
	::QueryPerformanceCounter((LARGE_INTEGER*)&CurrentTime);
	__int64 TimeDiff64 = CurrentTime - m_PrevTime;
	double TimeDiff = TimeDiff64 / static_cast<double>(m_Frequency);
	
	if (TimeDiff != 0)
	{
		m_FrameTime.pop_front();
		m_FrameTime.push_back(TimeDiff);

		double totalTime = 0.0;
		for (unsigned int i = 0; i < m_FrameTime.size(); i++)
		{
			totalTime += m_FrameTime[i];
		}

		m_nPeriod = (int)(totalTime / m_FrameTime.size());
		m_dFrameRate = 1 / m_nPeriod;
		
		m_PrevTime = CurrentTime;
	}
}