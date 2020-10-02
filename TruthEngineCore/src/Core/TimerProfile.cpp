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
		
		return std::chrono::duration_cast<std::chrono::duration<double, std::chrono::milliseconds>>(endTimePoint - m_startTimePoint).count();
	}

}