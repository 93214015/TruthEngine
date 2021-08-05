#pragma once

#include "Core/Renderer/RenderPass.h"
#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/TextureRenderTarget.h"
#include "Core/Renderer/TextureDepthStencil.h"
#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/ShaderManager.h"
#include "Core/Renderer/Viewport.h"

namespace TruthEngine
{
	class Model3D;
	class ICamera;
	class Material;
	class EventEntityAddMaterial;
	class EventEntityUpdateMaterial;
	class EventTextureResize;


	class RenderPass_ForwardRendering final : public RenderPass
	{

	public:

		RenderPass_ForwardRendering(RendererLayer* _RendererLayer);

		void OnAttach() override;
		void OnDetach() override;

		void OnImGuiRender() override;
		void OnUpdate(double _DeltaTime) override;

		void BeginScene() override;
		void EndScene() override;

		void Render() override;


	private:
		void PreparePiplineMaterial(const Material* material);
		void PreparePiplineEnvironment();

		void RegisterOnEvents();

		void OnRenderTargetResize(const EventTextureResize& _Event);
		void OnAddMaterial(EventEntityAddMaterial& event);
		void OnUpdateMaterial(const EventEntityUpdateMaterial& event);

		void InitTextures();
		void InitBuffers();
	private:

		RendererCommand m_RendererCommand;
		RendererCommand m_RendererCommand_ResolveTextures;

		RenderTargetView m_RenderTartgetView;
		DepthStencilView m_DepthStencilView;

		TextureDepthStencil* m_TextureDepthStencil = nullptr;
		TextureDepthStencil* m_TextureDepthStencilMS = nullptr;
		TextureRenderTarget* m_TextureRenderTargetHDRMS = nullptr;
		TextureRenderTarget* m_TextureRenderTargetMS = nullptr;


		Viewport m_Viewport;
		ViewRect m_ViewRect;

		ShaderManager* m_ShaderMgr;

		BufferManager* m_BufferMgr;

		class MaterialManager* m_MaterialManager;

		std::unordered_map<uint32_t, PipelineGraphics*> m_MaterialPipelines;
		PipelineGraphics* m_PipelineEnvironmentCube = nullptr;

		struct ConstantBuffer_Data_Per_Mesh
		{
			ConstantBuffer_Data_Per_Mesh()
				: WorldMatrix(IdentityMatrix), WorldInverseTransposeMatrix(IdentityMatrix), MaterialIndex(-1)
			{}
			ConstantBuffer_Data_Per_Mesh(const float4x4A& world, const float4x4A& worldIT, uint32_t materialIndex)
				: WorldMatrix(world), WorldInverseTransposeMatrix(worldIT), MaterialIndex(materialIndex)
			{}

			float4x4A WorldMatrix;
			float4x4A WorldInverseTransposeMatrix;
			uint32_t MaterialIndex;
			float3   Pad;
		};

		ConstantBufferDirect<ConstantBuffer_Data_Per_Mesh>* m_ConstantBufferDirect_PerMesh;
		ConstantBufferDirect<ConstantBuffer_Data_EnvironmentMap>* m_ConstantBufferDirect_EnvironmentMap;

		uint32_t m_TotalMeshNum = 0;
		uint32_t m_TotalVertexNum = 0;

		TimerProfile_OneSecond m_TimerRender;
	};
}
