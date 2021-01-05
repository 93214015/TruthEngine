#pragma once

namespace TruthEngine::Core
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
		void PopRenderPass(RenderPass* renderPass);

		void PopAll();

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

	};

}
