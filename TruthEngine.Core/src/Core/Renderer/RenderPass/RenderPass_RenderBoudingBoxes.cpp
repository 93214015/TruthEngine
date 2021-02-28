#include "pch.h"
#include "RenderPass_RenderBoudingBoxes.h"
#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/ShaderManager.h"
#include "Core/Entity/Components/BoundingBoxComponent.h"


TruthEngine::RenderPass_RenderBoundingBoxes::RenderPass_RenderBoundingBoxes()
	: RenderPass(TE_IDX_RENDERPASS::RENDERBOUNDINGBOX)
	, m_Viewport{ 0.0f, 0.0f, static_cast<float>(TE_INSTANCE_APPLICATION->GetClientWidth()), static_cast<float>(TE_INSTANCE_APPLICATION->GetClientHeight()), 0.0f, 1.0f }
	, m_ViewRect{ static_cast<long>(0.0), static_cast<long>(0.0), static_cast<long>(TE_INSTANCE_APPLICATION->GetClientWidth()), static_cast<long>(TE_INSTANCE_APPLICATION->GetClientHeight()) }
{
}

void TruthEngine::RenderPass_RenderBoundingBoxes::OnAttach()
{
	m_RendererCommand.Init(TE_IDX_RENDERPASS::RENDERBOUNDINGBOX, TE_IDX_SHADERCLASS::RENDERBOUNDINGBOX, TE_INSTANCE_BUFFERMANAGER, TE_INSTANCE_SHADERMANAGER);

	m_RendererCommand.CreateRenderTargetView(TE_IDX_TEXTURE::RT_SCENEBUFFER, &m_RenderTargetView);
	m_RendererCommand.CreateDepthStencilView(TE_IDX_TEXTURE::DS_SCENEBUFFER, &m_DepthStencilView);
}

void TruthEngine::RenderPass_RenderBoundingBoxes::OnDetach()
{
	m_RendererCommand.Release();
}

void TruthEngine::RenderPass_RenderBoundingBoxes::OnImGuiRender()
{
	
}

void TruthEngine::RenderPass_RenderBoundingBoxes::BeginScene()
{

	m_RendererCommand.Begin();

	m_RendererCommand.SetViewPort(&m_Viewport, &m_ViewRect);
	m_RendererCommand.SetRenderTarget(m_RenderTargetView);
	m_RendererCommand.SetDepthStencil(m_DepthStencilView);
}

void TruthEngine::RenderPass_RenderBoundingBoxes::EndScene()
{
	
}

void TruthEngine::RenderPass_RenderBoundingBoxes::Render()
{
	Scene* _ActiveScene = TE_INSTANCE_APPLICATION->GetActiveScene();
	auto _ComponentView = _ActiveScene->ViewEntities<BoundingBoxComponent>();

	for (auto _Entity : _ComponentView)
	{
		auto _AABB = _ActiveScene->GetComponent<BoundingBoxComponent>(_Entity).GetBoundingBox();

		//_AABB.GetCorners();
	}
}

void TruthEngine::RenderPass_RenderBoundingBoxes::OnSceneViewportResize(uint32_t width, uint32_t height)
{
	
}

