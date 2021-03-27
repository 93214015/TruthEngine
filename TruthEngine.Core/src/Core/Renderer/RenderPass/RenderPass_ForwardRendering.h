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


	class RenderPass_ForwardRendering : public RenderPass
	{

	public:

		RenderPass_ForwardRendering();
		~RenderPass_ForwardRendering();

		RenderPass_ForwardRendering(const RenderPass_ForwardRendering& renderer3D);
		RenderPass_ForwardRendering& operator=(const RenderPass_ForwardRendering& renderer3D);

		void OnAttach() override;
		void OnDetach() override;

		void OnImGuiRender() override;

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
	private:

		RendererCommand m_RendererCommand;
		TextureDepthStencil* m_TextureDepthStencil;

		RenderTargetView m_RenderTartgetView;
		DepthStencilView m_DepthStencilView;

		Viewport m_Viewport;
		ViewRect m_ViewRect;

		ShaderManager* m_ShaderMgr;

		BufferManager* m_BufferMgr;

		MaterialManager* m_MaterialManager;

		std::unordered_map<uint32_t, PipelineGraphics*> m_MaterialPipelines;
		PipelineGraphics* m_PipelineEnvironmentCube = nullptr;

		struct ConstantBuffer_Data_Per_Mesh
		{
			ConstantBuffer_Data_Per_Mesh()
				: WorldMatrix(IdentityMatrix), WorldInverseTransposeMatrix(IdentityMatrix), MaterialIndex(-1)
			{}
			ConstantBuffer_Data_Per_Mesh(float4x4 world, float4x4 worldIT, uint32_t materialIndex)
				: WorldMatrix(world), WorldInverseTransposeMatrix(worldIT), MaterialIndex(materialIndex)
			{}

			float4x4 WorldMatrix;
			float4x4 WorldInverseTransposeMatrix;
			uint32_t MaterialIndex;
			float3   Pad;
		};

		ConstantBufferDirect<ConstantBuffer_Data_Per_Mesh>* m_ConstantBufferDirect_PerMesh;

		uint32_t m_TotalMeshNum = 0;
		uint32_t m_TotalVertexNum = 0;

		TimerProfile_OneSecond m_TimerRender;
	};
}
