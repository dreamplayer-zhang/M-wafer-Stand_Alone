#pragma once

#ifdef __FRAMERATE_H__
#define __FRAMERATE_H__
#endif

#include <deque>

class CFrameRateCounter
{
public:
	CFrameRateCounter(unsigned long queueLenth = 1);
	virtual ~CFrameRateCounter();

	double GetFrameRate();
	int GetPeriod();
	void NewFrame();

private:
	double	m_dFrameRate;
	int		m_nPeriod;
	std::deque<double> m_FrameTime;
	__int64 m_Frequency;
	__int64 m_PrevTime;
	__int64 m_LastTime;
};