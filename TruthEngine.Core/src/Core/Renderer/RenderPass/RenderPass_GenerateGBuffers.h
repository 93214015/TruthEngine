#pragma once
#include "Core/Renderer/RenderPass.h"

#include "Core/Renderer/RendererCommand.h"
#include "Core/Renderer/TextureRenderTarget.h"
#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/Viewport.h"
#include "Core/Renderer/Pipeline.h"

namespace TruthEngine
{
	class RenderPass_GenerateGBuffers final : public RenderPass
	{
	public:

		RenderPass_GenerateGBuffers(class RendererLayer* _RendererLayer);

		// Inherited via RenderPass
		void OnImGuiRender() override;
		void OnUpdate(double _DeltaTime) override;
		void BeginScene() override;
		void EndScene() override;
		void Render() override;

	private:
		void InitRendererCommand() override;
		void InitTextures() override;
		void InitBuffers() override;
		void InitPipelines() override;


		void ReleaseRendererCommand() override;
		void ReleaseTextures() override;
		void ReleaseBuffers() override;
		void ReleasePipelines() override;

		void RegisterEventListeners() override;
		void UnRegisterEventListeners() override;

		void InitPipelines(const class Material* _Material);

		void OnRendererViewportResize(const class EventRendererViewportResize& _Event);
		void OnAddMaterial(const class EventEntityAddMaterial& _Event);
		void OnUpdateMaterial(const class EventEntityUpdateMaterial& _Event);

	private:
		RendererCommand m_RendererCommand;

		RenderTargetView m_RenderTargetViewGBufferColor;
		RenderTargetView m_RenderTargetViewGBufferNormal;
		RenderTargetView m_RenderTargetViewGBufferSpecular;

		TextureRenderTarget* m_TextureGBufferColor;
		TextureRenderTarget* m_TextureGBufferNormal;
		TextureRenderTarget* m_TextureGBufferSpecular;

		std::vector<PipelineGraphics> m_ContainerPipelines;
		std::unordered_map<uint32_t, PipelineGraphics*> m_MapMaterialPipeline;


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

		std::vector<EventListenerHandle> m_EventListeners;

	};
}
