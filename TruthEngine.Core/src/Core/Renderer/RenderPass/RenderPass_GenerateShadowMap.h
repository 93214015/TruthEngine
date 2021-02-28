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


	class RenderPass_GenerateShadowMap : public RenderPass
	{

	public:
		RenderPass_GenerateShadowMap();

		void OnAttach() override;


		void OnDetach() override;


		void OnImGuiRender() override;


		void BeginScene() override;


		void EndScene() override;


		void Render() override;


		void OnSceneViewportResize(uint32_t width, uint32_t height) override;

	private:


		void InitPipeline();

	private:

		uint32_t m_ShadoWMapSize = 2048;

		RendererCommand m_RendererCommand;
		TextureDepthStencil* m_TextureDepthStencil;
		DepthStencilView m_DepthStencilView;

		Viewport m_Viewport;
		ViewRect m_ViewRect;

		std::unordered_map<TE_IDX_MESH_TYPE, std::shared_ptr<Pipeline>> m_Pipelines;

		ShaderManager* m_ShaderManager;
		BufferManager* m_BufferManager;
		LightManager* m_LightManager;

		struct ConstantBuffer_Data_Per_Mesh
		{
			ConstantBuffer_Data_Per_Mesh()
				: mWorldMatrix(IdentityMatrix)
			{}
			ConstantBuffer_Data_Per_Mesh(const float4x4& world)
				: mWorldMatrix(world)
			{}

			float4x4 mWorldMatrix;
		};


		struct ConstantBuffer_Data_Per_Light
		{
			ConstantBuffer_Data_Per_Light()
			{}
			ConstantBuffer_Data_Per_Light(const float4x4& viewProj)
				: mLightViewProj(viewProj)
			{}

			float4x4 mLightViewProj;
		};

		ConstantBufferDirect<ConstantBuffer_Data_Per_Mesh>* m_ConstantBufferDirect_PerMesh;
		ConstantBufferDirect<ConstantBuffer_Data_Per_Light>* m_ConstantBufferDirect_PerLight;

		TimerProfile_OneSecond m_TimerBegin;
		TimerProfile_OneSecond m_TimerEnd;
		TimerProfile_OneSecond m_TimerRender;
	};
}
