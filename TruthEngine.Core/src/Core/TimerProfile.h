#pragma once

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

}
