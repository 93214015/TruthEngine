#include "pch.h"
#include "TimerEngine.h"


namespace TruthEngine::Core {


	TimerEngine::TimerEngine()
	{

	}

	void TimerEngine::Start()
	{
		auto startTime = std::chrono::high_resolution_clock::now();

		if (m_Stopped) {

			std::chrono::duration<double, std::milli> d = startTime - m_StoppedTimePoint;

			m_PausedTime += d.count();

			m_PrevTimePoint = startTime;
			//mStoppedTime = 0;
			m_Stopped = false;
		}
	}

	void TimerEngine::Reset()
	{
		auto currentTimePoint = std::chrono::high_resolution_clock::now();

		m_BaseTimePoint = currentTimePoint;
		m_PrevTimePoint = currentTimePoint;
		m_Stopped = false;
	}

	void TimerEngine::Stop()
	{
		if (!m_Stopped)
		{
			m_StoppedTimePoint = std::chrono::high_resolution_clock::now();

			m_Stopped = true;
		}
	}

	void TimerEngine::Tick()
	{
		if (m_Stopped)
		{
			m_DeltaTime = 0.0;
			return;
		}

		m_CurrentTimePoint = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double, std::milli> deltaDurtion = m_CurrentTimePoint - m_PrevTimePoint;

		m_DeltaTime = deltaDurtion.count();

		m_PrevTimePoint = m_CurrentTimePoint;

		if (m_DeltaTime < 0.0)
		{
			m_DeltaTime = 0.0;
		}
	}

	double TimerEngine::TotalTime() const
	{

	}

	double TimerEngine::DeltaTime() const
	{
		return m_DeltaTime;
	}


}