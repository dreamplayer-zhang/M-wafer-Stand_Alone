#ifndef __STOPWATCH_H__
#define __STOPWATCH_H__
#include <sys/timeb.h>

class STOPWATCH
{
private:
	struct _timeb m_Stb;
	struct _timeb m_Etb;
	__int64 m_stime;
	__int64 m_etime;
public:
	void Start()
	{
		_ftime_s(&m_Stb);
	}
	void End()
	{
		_ftime_s(&m_Etb);
	}
	__int64 TimeCheck()
	{
		m_stime = (__int64)m_Stb.time * (1000) + (__int64)m_Stb.millitm;
		m_etime = (__int64)m_Etb.time * (1000) + (__int64)m_Etb.millitm;
		return (m_etime - m_stime);
	}
};

#endif
