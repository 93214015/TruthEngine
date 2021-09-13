#pragma once

#include "Core/Renderer/RenderPass.h"
#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/Pipeline.h"

namespace TruthEngine
{
	class RenderPass_ScreenSpaceReflection final : public RenderPass
	{
	public:
		RenderPass_ScreenSpaceReflection(class RendererLayer* _RendererLayer);

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

		void OnEventRendererViewportResize(class EventRendererViewportResize& _Event);

	private:

		RendererCommand m_RendererCommand_Reflection;
		RendererCommand m_RendererCommand_Blend;

		PipelineGraphics m_Pipeline_Reflection;
		PipelineGraphics m_Pipeline_Blend;

		TextureRenderTarget* m_RenderTarget_Reflection;
		RenderTargetView m_RTV_Reflection;

		/*struct ConstantBufferData_SSReflection
		{
			ConstantBufferData_SSReflection() = default;
			ConstantBufferData_SSReflection(float _ViewAngleThreshold, float _EdgeDistThreshold, float _DepthBias, float _ReflectionScale)
				: ViewAngleThreshold(_ViewAngleThreshold), EdgeDistThreshold(_EdgeDistThreshold), DepthBias(_DepthBias), ReflectionScale(_ReflectionScale)
			{}

			float ViewAngleThreshold;
			float EdgeDistThreshold;
			float DepthBias;
			float ReflectionScale;
		};*/

		struct ConstantBufferData_SSReflection
		{
			ConstantBufferData_SSReflection() {};
			ConstantBufferData_SSReflection(float _MaxDistance, float _Resolution, float _Thichness, float _Steps, float _ViewAngleThreshold, float _ReflectionScale, float _DepthBias)
				: MaxDistance(_MaxDistance), Resolution(_Resolution), Thickness(_Thichness), Steps(_Steps), ViewAngleThreshold(_ViewAngleThreshold), ReflectionScale(_ReflectionScale), DepthBias(_DepthBias)
			{}

			float MaxDistance = 30.0;
			float Resolution = 0.3f;
			float Thickness = 1.0f;
			float Steps = 10.0f;

			float ViewAngleThreshold = 0.5f;
			float ReflectionScale = 1.0f;
			float DepthBias = 1.0f;
			float _pad0 = 0.0f;
		};

		ConstantBufferUpload<ConstantBufferData_SSReflection>* m_ConstantBuffer_Reflection;

		std::vector<EventListenerHandle> m_EventListenerList;
	};
}
