#pragma once
#include "Core/Renderer/RenderPass.h"

#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/Viewport.h"

namespace TruthEngine
{
	template<class T>
	class ConstantBufferDirect;

	class ShaderManager;
	class BufferManager;
	class LightManager;


	class RenderPass_GenerateShadowMap final : public RenderPass
	{

	public:
		RenderPass_GenerateShadowMap(RendererLayer* _RendererLayer, uint32_t ShadowMapSize);


		void OnAttach() override;


		void OnDetach() override;


		void OnImGuiRender() override;


		void OnUpdate(double _DeltaTime) override;


		void BeginScene() override;


		void EndScene() override;


		void Render() override;


	private:

		void InitPipeline();

		void RenderSpotLightShadowMap();

	private:

		uint32_t m_ShadoWMapSize;

		RendererCommand m_RendererCommand;
		RendererCommand m_RendererCommand_SpotLights;

		TextureDepthStencil* m_TextureDepthStencil_SunLight;
		TextureDepthStencil* m_TextureDepthStencil_SpotLight;
		DepthStencilView m_DepthStencilView_SunLight;
		DepthStencilView m_DepthStencilView_SpotLight;

		Viewport m_Viewport;
		ViewRect m_ViewRect;

		std::unordered_map<TE_IDX_MESH_TYPE, PipelineGraphics*> m_PipelinesForwardDepth;
		std::unordered_map<TE_IDX_MESH_TYPE, PipelineGraphics*> m_PipelinesReveresedDepth;

		ShaderManager* m_ShaderManager;
		BufferManager* m_BufferManager;
		LightManager* m_LightManager;

		struct alignas(16) ConstantBuffer_Data_Per_Mesh
		{
			ConstantBuffer_Data_Per_Mesh()
				: mWorldMatrix(IdentityMatrix)
			{}
			ConstantBuffer_Data_Per_Mesh(const float4x4A& world)
				: mWorldMatrix(world)
			{}

			float4x4A mWorldMatrix;
		};


		struct alignas(16) ConstantBuffer_Data_Per_Light
		{
			ConstantBuffer_Data_Per_Light()
			{}
			ConstantBuffer_Data_Per_Light(const float4x4A& viewProj)
				: mLightViewProj(viewProj)
			{}

			float4x4A mLightViewProj;
		};

		ConstantBufferDirect<ConstantBuffer_Data_Per_Mesh>* m_ConstantBufferDirect_PerMesh;
		ConstantBufferDirect<ConstantBuffer_Data_Per_Light>* m_ConstantBufferDirect_PerLight;

		TimerProfile_OneSecond m_TimerBegin;
		TimerProfile_OneSecond m_TimerEnd;
		TimerProfile_OneSecond m_TimerRender;
		TimerProfile_OneSecond m_TimerRender_PrepareMeshList;
		TimerProfile_OneSecond m_TimerRender_PrepareRenderCommand;
	};
}
