#pragma once

namespace TruthEngine {

	class TimerEngine {

	public:
		TimerEngine(double _TimeLimit = 0.0);

		double TotalTime()const;

		//Return elapsed time from previous frame in seconds
		double DeltaTime()const noexcept { return m_DeltaTime; }

		inline uint32_t GetFPS() const noexcept { return m_FPS; }
		inline double GetAverageCpuTime() const noexcept { return m_avgCPUFrameTime; }



		void Start();
		void Reset();
		void Stop();
		bool Tick();
		void SetTimeLimit(double _LimitTime);

	private:

		bool (*m_FuncPtrTick)(TimerEngine&, const std::chrono::duration<double, std::ratio<1>>);

		/*bool _LimitedTick(const std::chrono::duration<double, std::ratio<1>> _deltaDuration);
		bool _UnlimitedTick(const std::chrono::duration<double, std::ratio<1>> _deltaDuration);*/

	private:
		double m_DeltaTime = 0.0;
		double m_TimeAccumulator = 0.0;
		double m_PausedTime = 0.0;
		double m_TimeLimit = 0.0;

		uint32_t m_FrameCount = 0;
		uint32_t m_FPS = 0;

		double m_avgCPUFrameTime = 0.0;
		double m_OneSecondTimer = 0.0;
		
		std::chrono::high_resolution_clock::time_point m_BaseTimePoint;
		std::chrono::high_resolution_clock::time_point m_StoppedTimePoint;
		std::chrono::high_resolution_clock::time_point m_PrevTimePoint;
		std::chrono::high_resolution_clock::time_point m_CurrentTimePoint;

		bool m_Stopped;

		friend bool _LimitedTick(TimerEngine&, const std::chrono::duration<double, std::ratio<1>>);
		friend bool _UnlimitedTick(TimerEngine&, const std::chrono::duration<double, std::ratio<1>>);

	};

}
