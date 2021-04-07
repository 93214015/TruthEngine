#pragma once


namespace TruthEngine {

	class ThreadPool {

	public:
		~ThreadPool();

		void Start(uint32_t threadNum);
		void Abort();

		template<class F, class R = std::result_of_t<F&()>>
		std::future<R> Queue(F&& f)
		{
			std::packaged_task<R()> pTask(std::forward<F>(f));
			auto future = pTask.get_future();

			{
				std::unique_lock<std::mutex> lock(m_Mutex);
				m_Works.emplace_back(std::move(pTask));
			}

			m_CVariable.notify_one();

			return future;
		}

		static ThreadPool* GetInstance() 
		{
			static ThreadPool s_ThreadPool;
			return &s_ThreadPool; 
		}

	private:
		void CancelPendings();
		void Finish();
		void ThreadTask();

	private:
		std::mutex m_Mutex;
		std::condition_variable m_CVariable;
		std::vector<std::future<void>> m_Futures;
		std::deque<std::packaged_task<void()>> m_Works;

		bool m_Done = false;


	};

}

#define TE_INSTANCE_THREADPOOL ::TruthEngine::ThreadPool::GetInstance()

#define TE_RUN_TASK(x) TE_INSTANCE_THREADPOOL->Queue(x)
#define TE_RUN_THREAD(x, ...)  std::async(std::launch::async, x, __VA_ARGS__)
