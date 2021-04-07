#pragma once


#define TE_TIMER_SCOPE_FUNC auto t##__LINE__ = TruthEngine::TimerScope(__func__);
#define TE_TIMER_SCOPE(x) auto t##__LINE__  = TruthEngine::TimerScope(x);

namespace TruthEngine {

	class EventAppOneSecondPoint;

	class TimerProfile {

	public:
		//set start time point
		void Start() noexcept;
		//return duration from start time point in milisecond
		double GetTotalTime() const noexcept;
	private:


	private:
		std::chrono::high_resolution_clock::time_point m_startTimePoint;
	};

	class TimerProfile_OneSecond {

	public:
		TimerProfile_OneSecond();
		//set start time point
		void Start() noexcept;
		void End() noexcept;
		//return duration from start time point in milisecond
		inline double GetAverageTime() const noexcept
		{
			return m_AverageTime;
		}

		void OnEventOnSecond(EventAppOneSecondPoint& event);
	private:


	private:
		double m_AverageTime = 0.0f;
		double m_Accumulator = 0.0f;

		double m_TickCount = 0.0f;

		std::chrono::high_resolution_clock::time_point m_startTimePoint;
	};

	class TimerScope {
	public:
		TimerScope(const char* name) : m_Name(name), m_StartTimePoint(std::chrono::high_resolution_clock::now()) {}
		~TimerScope()
		{
			auto endTimePoint = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> d = endTimePoint - m_StartTimePoint;

			TE_LOG_CORE_INFO("{0} takes {1} milisecond", m_Name.c_str(), d.count());
		}
	private:
		std::chrono::high_resolution_clock::time_point m_StartTimePoint;
		std::string m_Name;
	};

}
