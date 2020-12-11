#include "pch.h"
#include "Renderer3D.h"

#include "Core/Entity/Model/Model3D.h"
#include "Core/Entity/Model/Mesh.h"
#include "Core/Renderer/Material.h"
#include "Core/Renderer/Pipeline.h"

#include "Core/Application.h"
#include "Core/Renderer/SwapChain.h"


namespace TruthEngine::Core
{

	Renderer3D::Renderer3D() 
		: m_TextureDepthStencil(
		"renderer3DDepth"
		, TE_INSTANCE_APPLICATION->GetClientWidth()
		, TE_INSTANCE_APPLICATION->GetClientHeight()
		, TE_RESOURCE_FORMAT::R32_TYPELESS
		, TextureDepthStencil::ClearValue{ 1.0f, 0 }
		, false)
	{};

	Renderer3D::~Renderer3D() = default;

	Renderer3D::Renderer3D(const Renderer3D& renderer3D) = default;

	Renderer3D& Renderer3D::operator=(const Renderer3D& renderer3D) = default;


	void Renderer3D::Init(BufferManager* bufferMgr, const std::vector<Material>& materials)
	{

		m_BufferMgr = bufferMgr;

		m_ShaderMgr = ShaderManager::Factory();

		m_BufferMgr->CreateResource(&m_TextureDepthStencil);

		m_RenderTartgetView = m_BufferMgr->CreateRenderTargetView(TE_INSTANCE_SWAPCHAIN);

		m_DepthStencilView = m_BufferMgr->CreateDepthStencilView(&m_TextureDepthStencil);

		m_RendererCommand.Init();

		for (const auto& mat : materials)
		{
			std::string shaderName = std::string("renderer3D_material") + std::to_string(mat.GetID());

			Shader* shader = nullptr;

			m_ShaderMgr->AddShader(&shader, mat.GetRendererStates(), "Assets/Shaders/renderer3D.hlsl", "vs", "ps");

			auto pipeline = std::make_shared<Pipeline>();
			pipeline->SetShader(shader);
			pipeline->SetRendererStates(mat.GetRendererStates());

			m_MaterialPipelines[mat.GetID()] = pipeline;
		}

	}


	void Renderer3D::BeginScene()
	{
		m_RendererCommand.Begin();
	}


	void Renderer3D::EndScene()
	{
	}


	void Renderer3D::Render(std::vector<const Model3D*> models)
	{

		m_RendererCommand.SetRenderTarget(TE_INSTANCE_SWAPCHAIN, m_RenderTartgetView);
		m_RendererCommand.SetDepthStencil(m_DepthStencilView);

		for (auto m : models)
		{
			for (auto mesh : m->GetMeshes())
			{
				m_RendererCommand.SetPipeline(m_MaterialPipelines[mesh->GetMaterial()->GetID()].get());
				m_RendererCommand.DrawIndexed(mesh);
			}
		}


	}

}