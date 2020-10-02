#pragma once

namespace TruthEngine::Core {

	class TimerEngine {

	public:
		TimerEngine();

		double TotalTime()const;
		double DeltaTime()const;

		void Start();
		void Reset();
		void Stop();
		void Tick();

	private:
		double m_DeltaTime;
		double m_PausedTime;

		std::chrono::high_resolution_clock::time_point m_BaseTimePoint;
		std::chrono::high_resolution_clock::time_point m_StoppedTimePoint;
		std::chrono::high_resolution_clock::time_point m_PrevTimePoint;
		std::chrono::high_resolution_clock::time_point m_CurrentTimePoint;

		bool m_Stopped;

	};

}
