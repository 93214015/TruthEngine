#include "pch.h"
#include "TimerProfile.h"

namespace TruthEngine::Core {



	void TimerProfile::Start() noexcept
	{
		m_startTimePoint = std::chrono::high_resolution_clock::now();
	}

	double TimerProfile::GetTotalTime() const noexcept
	{
		auto endTimePoint = std::chrono::high_resolution_clock::now();
		
		std::chrono::duration<double, std::milli> d = (endTimePoint - m_startTimePoint);

		return d.count();
	}

}