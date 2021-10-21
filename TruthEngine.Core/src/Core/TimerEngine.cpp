#include "pch.h"
#include "TimerEngine.h"
#include "Event/EventApplication.h"
#include "Application.h"


namespace TruthEngine {

	static bool _LimitedTick(TimerEngine& _TimerEngine, const std::chrono::duration<double, std::ratio<1>> _deltaDuration)
	{
		_TimerEngine.m_TimeAccumulator += _deltaDuration.count();

		if (_TimerEngine.m_TimeAccumulator < _TimerEngine.m_TimeLimit)
		{
			return false;
		}

		_TimerEngine.m_FrameCount++;
		_TimerEngine.m_DeltaTime = _TimerEngine.m_TimeAccumulator;

		_TimerEngine.m_TimeAccumulator = 0.0;
		return true;
	}

	static bool _UnlimitedTick(TimerEngine& _TimerEngine, const std::chrono::duration<double, std::ratio<1>> _deltaDuration)
	{
		_TimerEngine.m_DeltaTime = _deltaDuration.count();

		_TimerEngine.m_FrameCount++;

		return true;
	}


	TimerEngine::TimerEngine(double _TimeLimit)
		: m_TimeLimit(_TimeLimit), m_FuncPtrTick(_TimeLimit == 0 ? &_UnlimitedTick : _LimitedTick)
	{}

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

			m_DeltaTime = 0.0;
		}
	}

	bool TimerEngine::Tick()
	{
		if (m_Stopped)
		{
			return false;
		}

		m_CurrentTimePoint = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double, std::ratio<1>> deltaDuration = m_CurrentTimePoint - m_PrevTimePoint;

		m_PrevTimePoint = m_CurrentTimePoint;

		bool _Result = m_FuncPtrTick(*this, deltaDuration);

		m_OneSecondTimer += deltaDuration.count();

		if (m_OneSecondTimer > 1.0f)
		{
			m_avgCPUFrameTime = m_OneSecondTimer / m_FrameCount;
			m_FPS = static_cast<uint32_t>((1.0 / m_avgCPUFrameTime)) + 1;
			m_FrameCount = 0;
			m_OneSecondTimer = 0.0;

			EventAppOneSecondPoint event;
			TE_INSTANCE_APPLICATION->OnEvent(event);
		}

		return _Result;
	}

	void TimerEngine::SetTimeLimit(double _TimeLimit)
	{
		m_TimeLimit = _TimeLimit;

		m_FuncPtrTick = _TimeLimit == 0 ? &_UnlimitedTick : _LimitedTick;
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