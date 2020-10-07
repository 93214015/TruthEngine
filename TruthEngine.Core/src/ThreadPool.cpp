#include "pch.h"
#include "ThreadPool.h"

namespace TruthEngine {


	ThreadPool::~ThreadPool()
	{
		Finish();
	}

	void ThreadPool::Start(uint32_t threadNum)
	{
		for (uint32_t i = 0; i < threadNum; ++i)
		{
			m_Futures.push_back(std::async(std::launch::async, [this]() {ThreadTask(); }));
		}
	}

	void ThreadPool::Abort()
	{
		CancelPendings();
		Finish();
	}

	void ThreadPool::CancelPendings()
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		m_Works.clear();
	}

	void ThreadPool::Finish()
	{
		m_Done = true;

		m_CVariable.notify_all();

		m_Futures.clear();
	}

	void ThreadPool::ThreadTask()
	{
		while (!m_Done)
		{
			std::packaged_task<void()> pTask;

			{
				std::unique_lock<std::mutex> lock(m_Mutex);

				if (m_Works.empty())
				{
					m_CVariable.wait(lock, [this]() { return !m_Works.empty() || m_Done; });
				}

				if (m_Done)
					break;

				pTask = std::move(m_Works.front());
				m_Works.pop_front();
			}

			pTask();
		}
	}

	TruthEngine::ThreadPool ThreadPool::s_ThreadPool;

}