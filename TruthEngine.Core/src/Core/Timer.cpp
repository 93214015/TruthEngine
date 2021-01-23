#include "pch.h"
#include "Timer.h"
#include "Application.h"

namespace TruthEngine::Core {



	void TimerProfile::Start() noexcept
	{
		m_startTimePoint = std::chrono::high_resolution_clock::now();
	}

	double TimerProfile::GetTotalTime() const noexcept
	{
		const auto endTimePoint = std::chrono::high_resolution_clock::now();
		
		std::chrono::duration<double, std::milli> d(endTimePoint - m_startTimePoint);

		return d.count();
	}

	TimerProfile_Average::TimerProfile_Average(double averagingDuration)
		: m_AveragingDuration(averagingDuration)
	{}

	void TimerProfile_Average::Start() noexcept
	{
		m_startTimePoint = std::chrono::high_resolution_clock::now();
	}

	void TimerProfile_Average::End() noexcept
	{	
		const auto endTimePoint = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double, std::milli> d(endTimePoint - m_startTimePoint);

		m_Accumulator += d.count();
		m_TickCount++;

		static auto application = TE_INSTANCE_APPLICATION;

		if (m_FrameTimeAccumulator += application->FrameTime(); m_FrameTimeAccumulator  > m_AveragingDuration)
		{
			m_AverageTime = m_Accumulator / m_TickCount;
			m_Accumulator = 0.0;
			m_TickCount = 0.0f;
			m_FrameTimeAccumulator = 0.0f;
		}

	}

}