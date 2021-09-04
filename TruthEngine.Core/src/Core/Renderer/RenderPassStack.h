#pragma once

namespace TruthEngine
{
	class RenderPass;

	class RenderPassStack
	{
	public:

		RenderPassStack();

		~RenderPassStack();

		RenderPassStack(const RenderPassStack&);
		RenderPassStack& operator=(const RenderPassStack&);

		RenderPassStack(RenderPassStack&&) noexcept;
		RenderPassStack& operator=(RenderPassStack&&) noexcept;

		void PushRenderPass(RenderPass* renderPass, uint32_t placeIndex = -1);
		void PushDelegate(std::function<void()> _Delegate);

		void PopAll();

		void OnUpdate(double _DeltaTime);
		void BeginScene();
		void RenderAsync();
		void Render();
		void EndScene();

		std::vector<RenderPass*>::iterator begin();
		std::vector<RenderPass*>::iterator end();

		std::vector<RenderPass*>::const_iterator begin() const;
		std::vector<RenderPass*>::const_iterator end() const;

		std::vector<RenderPass*>::reverse_iterator rbegin();
		std::vector<RenderPass*>::reverse_iterator rend();

		std::vector<RenderPass*>::const_reverse_iterator rbegin() const;
		std::vector<RenderPass*>::const_reverse_iterator rend() const;
	private:
		std::vector<RenderPass*> m_Vector;

		std::vector<std::function<void()>> m_DelegateList;

	};

}
