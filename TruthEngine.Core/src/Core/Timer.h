#pragma once


#define TE_TIMER_SCOPE_FUNC auto t##__LINE__ = TruthEngine::Core::TimerScope(__func__)
#define TE_TIMER_SCOPE(x) auto t##__LINE__  = TruthEngine::Core::TimerScope(x)

namespace TruthEngine::Core {

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
