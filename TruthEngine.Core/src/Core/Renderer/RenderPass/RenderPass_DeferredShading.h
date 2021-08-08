#pragma once

#include "Core/Renderer/RenderPass.h"

namespace TruthEngine
{
	class RenderPass_DeferredShading final : public RenderPass
	{
	public:

		// Inherited via RenderPass
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;
		virtual void OnUpdate(double _DeltaTime) override;
		virtual void BeginScene() override;
		virtual void EndScene() override;
		virtual void Render() override;

	private:

	};
}
