#pragma once

#include "Core/Renderer/RenderPass.h"

namespace TruthEngine
{
	class RenderPass_ScreenSpaceReflection final : public RenderPass
	{
	public:
		// Inherited via RenderPass
		virtual void OnImGuiRender() override;

		virtual void OnUpdate(double _DeltaTime) override;

		virtual void BeginScene() override;

		virtual void EndScene() override;

		virtual void Render() override;

	private:

		virtual void InitRendererCommand() override;

		virtual void InitTextures() override;

		virtual void InitBuffers() override;

		virtual void InitPipelines() override;

		virtual void ReleaseRendererCommand() override;

		virtual void ReleaseTextures() override;

		virtual void ReleaseBuffers() override;

		virtual void ReleasePipelines() override;

		virtual void RegisterEventListeners() override;

		virtual void UnRegisterEventListeners() override;

	private:




	};
}
