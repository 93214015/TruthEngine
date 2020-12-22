#include "pch.h"
#include "RenderPass_ForwardRendering.h"

#include "Core/Entity/Model/Model3D.h"
#include "Core/Entity/Model/Mesh.h"
#include "Core/Entity/Model/ModelManager.h"
#include "Core/Renderer/Material.h"
#include "Core/Renderer/Pipeline.h"

#include "Core/Application.h"
#include "Core/Renderer/SwapChain.h"


namespace TruthEngine::Core
{

	RenderPass_ForwardRendering::RenderPass_ForwardRendering()
		: RenderPass(TE_IDX_RENDERPASS::FORWARDRENDERING)
		, m_TextureDepthStencil(nullptr)
		, m_Viewport{ 0.0f, 0.0f, static_cast<float>(TE_INSTANCE_APPLICATION->GetClientWidth()), static_cast<float>(TE_INSTANCE_APPLICATION->GetClientHeight()), 0.0f, 1.0f }
		, m_ViewREct{ static_cast<long>(0.0), static_cast<long>(0.0), static_cast<long>(TE_INSTANCE_APPLICATION->GetClientWidth()), static_cast<long>(TE_INSTANCE_APPLICATION->GetClientHeight()) }
	{};

	RenderPass_ForwardRendering::~RenderPass_ForwardRendering() = default;

	RenderPass_ForwardRendering::RenderPass_ForwardRendering(const RenderPass_ForwardRendering& renderer3D) = default;

	RenderPass_ForwardRendering& RenderPass_ForwardRendering::operator=(const RenderPass_ForwardRendering& renderer3D) = default;


	void RenderPass_ForwardRendering::OnAttach()
	{
		m_BufferMgr = TE_INSTANCE_BUFFERMANAGER.get();

		m_ShaderMgr = TE_INSTANCE_SHADERMANAGER;

		m_RendererCommand.Init(TE_IDX_RENDERPASS::FORWARDRENDERING, TE_IDX_SHADERCLASS::FORWARDRENDERING, 1, TE_INSTANCE_BUFFERMANAGER, m_ShaderMgr);

		m_TextureDepthStencil = m_RendererCommand.CreateDepthStencil(TE_IDX_DEPTHSTENCIL::SCENEBUFFER, TE_INSTANCE_APPLICATION->GetClientWidth(), TE_INSTANCE_APPLICATION->GetClientHeight(), TE_RESOURCE_FORMAT::R32_TYPELESS, ClearValue_DepthStencil{ 1.0f, 0 }, false);

		m_RendererCommand.CreateDepthStencilView(m_TextureDepthStencil, &m_DepthStencilView);

		for (const auto& mat : TE_INSTANCE_MODELMANAGER->GetMaterials())
		{
			std::string shaderName = std::string("renderer3D_material") + std::to_string(mat.GetID());

			Shader* shader = nullptr;

			m_ShaderMgr->AddShader(&shader, TE_IDX_SHADERCLASS::FORWARDRENDERING, mat.GetRendererStates(), "Assets/Shaders/renderer3D.hlsl", "vs", "ps");

			ShaderInputElement inputElement;
			inputElement.AlignedByteOffset = 0;
			inputElement.Format = TE_RESOURCE_FORMAT::R32G32B32_FLOAT;
			inputElement.InputSlot = 0;
			inputElement.InputSlotClass = TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX;
			inputElement.InstanceDataStepRate = 0;
			inputElement.SemanticIndex = 0;
			inputElement.SemanticName = "POSITION";
			shader->AddInputElement(inputElement);

			inputElement.AlignedByteOffset = 0;
			inputElement.Format = TE_RESOURCE_FORMAT::R32G32B32_FLOAT;
			inputElement.InputSlot = 1;
			inputElement.InputSlotClass = TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX;
			inputElement.InstanceDataStepRate = 0;
			inputElement.SemanticIndex = 0;
			inputElement.SemanticName = "NORMAL";
			shader->AddInputElement(inputElement);

			inputElement.AlignedByteOffset = 12;
			inputElement.Format = TE_RESOURCE_FORMAT::R32G32_FLOAT;
			inputElement.InputSlot = 1;
			inputElement.InputSlotClass = TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX;
			inputElement.InstanceDataStepRate = 0;
			inputElement.SemanticIndex = 0;
			inputElement.SemanticName = "TEXCOORD";
			shader->AddInputElement(inputElement);


			auto pipeline = std::make_shared<Pipeline>();
			pipeline->SetShader(shader);
			pipeline->SetRendererStates(mat.GetRendererStates());

			m_MaterialPipelines[mat.GetID()] = pipeline;
		}

		m_RendererCommand.CreateRenderTargetView(TE_IDX_RENDERTARGET::SCENEBUFFER, &m_RenderTartgetView);
	}

	void RenderPass_ForwardRendering::OnDetach()
	{
		m_RendererCommand.Release();
		m_RendererCommand.ReleaseResource(m_TextureDepthStencil);
		m_MaterialPipelines.clear();
	}


	void RenderPass_ForwardRendering::BeginScene()
	{
		m_RendererCommand.Begin();

		m_RendererCommand.SetViewPort(&m_Viewport, &m_ViewREct);
		//m_RendererCommand.SetRenderTarget(TE_INSTANCE_SWAPCHAIN, m_RenderTartgetView);
		m_RendererCommand.SetRenderTarget(m_RenderTartgetView);
		m_RendererCommand.SetDepthStencil(m_DepthStencilView);
		m_RendererCommand.ClearRenderTarget(m_RenderTartgetView);
		m_RendererCommand.ClearDepthStencil(m_DepthStencilView);
	}


	void RenderPass_ForwardRendering::EndScene()
	{
	}


	void RenderPass_ForwardRendering::Render(std::vector<const Model3D*> models)
	{

		for (auto m : models)
		{
			for (auto mesh : m->GetMeshes())
			{
				UINT32 s = 10;
				auto s2 = s << 1;

				m_RendererCommand.SetPipeline(m_MaterialPipelines[mesh->GetMaterial()->GetID()].get());
				m_RendererCommand.DrawIndexed(mesh);
			}
		}

		m_RendererCommand.End();

	}


	void RenderPass_ForwardRendering::OnResize(uint32_t width, uint32_t height)
	{
		m_RendererCommand.Resize(m_TextureDepthStencil, width, height, &m_DepthStencilView, nullptr);

		m_RendererCommand.CreateRenderTargetView(TE_IDX_RENDERTARGET::SCENEBUFFER, &m_RenderTartgetView);

		m_Viewport.Resize(width, height);

		m_ViewREct = ViewRect{ 0, 0, static_cast<long>(width), static_cast<long>(height)};
	}


}