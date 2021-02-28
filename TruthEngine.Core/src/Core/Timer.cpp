#include "pch.h"
#include "Timer.h"
#include "Application.h"
#include "Event/EventApplication.h"

namespace TruthEngine {



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

	TimerProfile_OneSecond::TimerProfile_OneSecond()
	{
		auto listener_oneSecondPoint = [this](Event& event) 
		{
			OnEventOnSecond(static_cast<EventAppOneSecondPoint&>(event));
		};

		TE_INSTANCE_APPLICATION->RegisterEventListener(EventType::AppOneSecondPoint, listener_oneSecondPoint);
	}

	void TimerProfile_OneSecond::Start() noexcept
	{
		m_startTimePoint = std::chrono::high_resolution_clock::now();
	}

	void TimerProfile_OneSecond::End() noexcept
	{	
		const auto endTimePoint = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double, std::milli> d(endTimePoint - m_startTimePoint);

		m_Accumulator += d.count();
		m_TickCount++;
	}

	void TimerProfile_OneSecond::OnEventOnSecond(EventAppOneSecondPoint& event)
	{
		m_AverageTime = m_Accumulator / m_TickCount;
		m_Accumulator = 0.0;
		m_TickCount = 0.0f;
	}

}