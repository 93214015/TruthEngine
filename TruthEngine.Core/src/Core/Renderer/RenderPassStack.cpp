#include "pch.h"
#include "RenderPassStack.h"

#include "RenderPass.h";

namespace TruthEngine
{



	RenderPassStack::RenderPassStack()
	{
		m_Vector.reserve(10);
	}

	RenderPassStack::~RenderPassStack() = default;

	RenderPassStack::RenderPassStack(const RenderPassStack&) = default;

	RenderPassStack& RenderPassStack::operator=(const RenderPassStack&) = default;

	RenderPassStack::RenderPassStack(RenderPassStack&&) noexcept = default;

	RenderPassStack& RenderPassStack::operator=(RenderPassStack&&) noexcept = default;

	void RenderPassStack::PushRenderPass(RenderPass* renderPass, uint32_t placeIndex)
	{
		auto index = placeIndex == -1 ? m_Vector.size() : placeIndex;

		m_Vector.emplace(m_Vector.cbegin() + index, renderPass);

		renderPass->OnAttach();

		auto _lambda_end = [renderPass]() { renderPass->EndScene(); };

		m_DelegateList.push_back(_lambda_end);
	}

	void RenderPassStack::PushDelegate(std::function<void()> _Delegate)
	{
		m_DelegateList.push_back(_Delegate);
	}

	void RenderPassStack::PopAll()
	{
		for (auto rp : m_Vector)
		{
			rp->OnDetach();
		}

		m_Vector.clear();
		m_DelegateList.clear();
	}

	void RenderPassStack::OnUpdate(double _DeltaTime)
	{
		for (auto _RenderPass : m_Vector)
		{
			_RenderPass->OnUpdate(_DeltaTime);
		}
	}

	void RenderPassStack::BeginScene()
	{
		for (auto _RenderPass : m_Vector)
		{
			_RenderPass->BeginScene();
		}
	}

	void RenderPassStack::RenderAsync()
	{
		static std::vector<std::future<void>> m_futures;

		for (auto _RenderPass : m_Vector)
		{
			auto _lambda = [_RenderPass]() { _RenderPass->Render(); };
			m_futures.emplace_back(std::move(ThreadPool::GetInstance()->Queue(_lambda)));
		}

		for (auto& f : m_futures)
		{
			f.wait();
		}

		m_futures.clear();
	}

	void RenderPassStack::Render()
	{
		for (auto _RenderPass : m_Vector)
		{
			_RenderPass->Render();
		}
	}

	void RenderPassStack::EndScene()
	{
		for (auto& _delegate : m_DelegateList)
		{
			_delegate();
		}
	}


	std::vector<RenderPass*>::iterator RenderPassStack::begin()
	{
		return m_Vector.begin();
	}

	std::vector<RenderPass*>::iterator RenderPassStack::end()
	{
		return m_Vector.end();
	}

	std::vector<RenderPass*>::const_iterator RenderPassStack::begin() const
	{
		return m_Vector.begin();
	}

	std::vector<RenderPass*>::const_iterator RenderPassStack::end() const
	{
		return m_Vector.end();
	}

	std::vector<RenderPass*>::reverse_iterator RenderPassStack::rbegin()
	{
		return m_Vector.rbegin();
	}

	std::vector<RenderPass*>::reverse_iterator RenderPassStack::rend()
	{
		return m_Vector.rend();
	}

	std::vector<RenderPass*>::const_reverse_iterator RenderPassStack::rbegin() const
	{
		return m_Vector.rbegin();
	}

	std::vector<RenderPass*>::const_reverse_iterator RenderPassStack::rend() const
	{
		return m_Vector.rend();
	}

}