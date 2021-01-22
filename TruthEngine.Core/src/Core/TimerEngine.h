#pragma once

namespace TruthEngine::Core {

	class TimerEngine {

	public:
		TimerEngine();

		double TotalTime()const;
		inline double DeltaTime()const noexcept { return m_DeltaTime; }
		inline uint32_t GetFPS() const noexcept { return m_FPS; }
		inline double GetAverageCpuTime() const noexcept { return m_avgCPUFrameTime; }

		void Start();
		void Reset();
		void Stop();
		void Tick();

	private:
		double m_DeltaTime = 0.0;
		double m_PausedTime = 0.0;

		uint32_t m_FrameCount = 0;
		uint32_t m_FPS = 0;

		double m_avgCPUFrameTime = 0.0;
		double m_OneSecondTimer = 0.0;
		
		std::chrono::high_resolution_clock::time_point m_BaseTimePoint;
		std::chrono::high_resolution_clock::time_point m_StoppedTimePoint;
		std::chrono::high_resolution_clock::time_point m_PrevTimePoint;
		std::chrono::high_resolution_clock::time_point m_CurrentTimePoint;

		bool m_Stopped;

	};

}
