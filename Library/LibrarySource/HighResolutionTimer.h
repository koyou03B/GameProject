#pragma once

#include<windows.h>

class HighResolutionTimer
{
public:
	HighResolutionTimer() : m_deltaTime(-1.0), m_pausedTime(0), m_stopped(false),m_stopTime(0)
	{
		LONGLONG countsPerSec;
		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&countsPerSec));
		m_secondsPerCount = 1.0 / static_cast<double>(countsPerSec);

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_thisTime));
		m_baseTime = m_thisTime;
		m_lastTime = m_thisTime;
	}

	// Returns the total time elapsed since Reset() was called, NOT counting any
	// time when the clock is stopped.
	float TimeStamp() const  // in seconds
	{
		// If we are stopped, do not count the time that has passed since we stopped.
		// Moreover, if we previously already had a pause, the distance 
		// stop_time - base_time includes paused time, which we do not want to count.
		// To correct this, we can subtract the paused time from mStopTime:  
		//
		//                     |<--paused_time-->|
		// ----*---------------*-----------------*------------*------------*------> time
		//  base_time       stop_time        start_time     stop_time    this_time

		if (m_stopped)
		{
			return static_cast<float>(((m_stopTime - m_pausedTime) - m_baseTime) * m_secondsPerCount);
		}

		// The distance this_time - mBaseTime includes paused time,
		// which we do not want to count.  To correct this, we can subtract 
		// the paused time from this_time:  
		//
		//  (this_time - paused_time) - base_time 
		//
		//                     |<--paused_time-->|
		// ----*---------------*-----------------*------------*------> time
		//  base_time       stop_time        start_time     this_time
		else
		{
			return static_cast<float>(((m_thisTime - m_pausedTime) - m_baseTime) * m_secondsPerCount);
		}
	}

	float TimeInterval() const  // in seconds
	{
		return static_cast<float>(m_deltaTime);
	}

	void Reset() // Call before message loop.
	{
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_thisTime));
		m_baseTime = m_thisTime;
		m_lastTime = m_thisTime;

		m_stopTime = 0;
		m_stopped = false;
	}

	void Start() // Call when unpaused.
	{
		LONGLONG startTime;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&startTime));

		// Accumulate the time elapsed between stop and start pairs.
		//
		//                     |<-------d------->|
		// ----*---------------*-----------------*------------> time
		//  base_time       stop_time        start_time     
		if (m_stopped)
		{
			m_pausedTime += (startTime - m_stopTime);
			m_lastTime = startTime;
			m_stopTime = 0;
			m_stopped = false;
		}
	}

	void Stop() // Call when paused.
	{
		if (!m_stopped)
		{
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_stopTime));
			m_stopped = true;
		}
	}

	void Tick() // Call every frame.
	{
		if (m_stopped)
		{
			m_deltaTime = 0.0;
			return;
		}

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_thisTime));
		// Time difference between this frame and the previous.
		m_deltaTime = (m_thisTime - m_lastTime) * m_secondsPerCount;

		// Prepare for next frame.
		m_lastTime = m_thisTime;

		// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
		// processor goes into a power save mode or we get shuffled to another
		// processor, then mDeltaTime can be negative.
		if (m_deltaTime < 0.0)
		{
			m_deltaTime = 0.0;
		}
	}

private:
	double m_secondsPerCount;
	double m_deltaTime;

	LONGLONG m_baseTime;
	LONGLONG m_pausedTime;
	LONGLONG m_stopTime;
	LONGLONG m_lastTime;
	LONGLONG m_thisTime;

	bool m_stopped;
};
