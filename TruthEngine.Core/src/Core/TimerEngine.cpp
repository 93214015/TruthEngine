#include "pch.h"
#include "TimerEngine.h"
#include "Event/EventApplication.h"
#include "Application.h"


namespace TruthEngine {


	TimerEngine::TimerEngine()
	{

	}

	void TimerEngine::Start()
	{
		auto startTime = std::chrono::high_resolution_clock::now();

		if (m_Stopped) {

			std::chrono::duration<double, std::ratio<1>> d = startTime - m_StoppedTimePoint;

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

		std::chrono::duration<double, std::ratio<1>> deltaDurtion = m_CurrentTimePoint - m_PrevTimePoint;

		m_DeltaTime = deltaDurtion.count();

		m_PrevTimePoint = m_CurrentTimePoint;

		if (m_DeltaTime < 0.0)
		{
			m_DeltaTime = 0.0;
		}

		m_FrameCount++;
		m_OneSecondTimer += m_DeltaTime;

		if (m_OneSecondTimer > 1.0f)
		{
			m_avgCPUFrameTime = m_OneSecondTimer / m_FrameCount;
			m_FPS = static_cast<uint32_t>((1.0 / m_avgCPUFrameTime)) + 1;
			m_FrameCount = 0;
			m_OneSecondTimer = 0.0;

			EventAppOneSecondPoint event;
			TE_INSTANCE_APPLICATION->OnEvent(event);
		}
	}

	double TimerEngine::TotalTime() const
	{
		if (m_Stopped)
		{
			auto d = std::chrono::duration<double, std::ratio<1>>(m_StoppedTimePoint - m_BaseTimePoint).count();

			return d - m_PausedTime;
		}
		else
		{
			auto d = std::chrono::duration<double, std::ratio<1>>(m_CurrentTimePoint - m_BaseTimePoint).count();
			return d - m_PausedTime;
		}
	}


}