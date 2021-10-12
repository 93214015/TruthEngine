#include "pch.h"
#include "RenderPass_RenderBoudingBoxes.h"

#include "Core/Renderer/RendererLayer.h"
#include "Core/Renderer/BufferManager.h"
#include "Core/Renderer/ShaderManager.h"
#include "Core/Entity/Components/BoundingBoxComponent.h"
#include "Core/Entity/Components/TransformComponent.h"
#include "Core/Entity/Scene.h"

#include "Core/Profiler/GPUEvents.h"


TruthEngine::RenderPass_RenderBoundingBoxes::RenderPass_RenderBoundingBoxes(RendererLayer* _RendererLayer)
	: RenderPass(TE_IDX_RENDERPASS::RENDERBOUNDINGBOX, _RendererLayer)
{
}

void TruthEngine::RenderPass_RenderBoundingBoxes::OnImGuiRender()
{
	
}

void TruthEngine::RenderPass_RenderBoundingBoxes::OnUpdate(double _DeltaTime)
{
}

void TruthEngine::RenderPass_RenderBoundingBoxes::BeginScene()
{

	if (m_Queue.size() == 0)
		return;

	m_RendererCommand.BeginGraphics(&m_Pipeline);
	GPUBEGINEVENT(m_RendererCommand, "Render BoundingBox");

	m_RendererCommand.SetViewPort(&m_RendererLayer->GetViewportScene(), &m_RendererLayer->GetViewRectScene());
	m_RendererCommand.SetRenderTarget(m_RendererLayer->GetRenderTargetViewSceneSDR());
	m_RendererCommand.SetDepthStencil(m_RendererLayer->GetDepthStencilViewScene());
}

void TruthEngine::RenderPass_RenderBoundingBoxes::EndScene()
{	
	if (m_Queue.size() == 0)
		return;

	GPUENDEVENT(m_RendererCommand);
	m_RendererCommand.End();

	m_Queue.clear();

}

void TruthEngine::RenderPass_RenderBoundingBoxes::Render()
{
	if (m_Queue.size() == 0)
		return;

	m_RendererCommand.ExecutePendingCommands();

	Scene* _ActiveScene = GetActiveScene();

	auto _CBData = m_ConstantBufferDirect_PerBB->GetData();

	for (auto _Entity : m_Queue)
	{
		const auto& _AABB = _ActiveScene->GetComponent<BoundingBoxComponent>(_Entity).GetBoundingBox();

		XMMatrix _XMTransform = Math::ToXM(_ActiveScene->GetComponent<TransformComponent>(_Entity).GetTransform());

		BoundingAABox _TransformedBB;
		_AABB.Transform(_TransformedBB, _XMTransform);

		*_CBData = ConstantBuffer_Data_PerBoundingBox(_TransformedBB.Extents, _TransformedBB.Center);

		m_RendererCommand.SetDirectConstantGraphics(m_ConstantBufferDirect_PerBB);

		m_RendererCommand.Draw(1, 0);
	}

}

void TruthEngine::RenderPass_RenderBoundingBoxes::Queue(Entity _Entity)
{
	m_Queue.push_back(_Entity);
}

void TruthEngine::RenderPass_RenderBoundingBoxes::InitRendererCommand()
{
	m_RendererCommand.Init(TE_IDX_RENDERPASS::RENDERBOUNDINGBOX, TE_IDX_SHADERCLASS::RENDERBOUNDINGBOX);
}

void TruthEngine::RenderPass_RenderBoundingBoxes::InitTextures()
{
}

void TruthEngine::RenderPass_RenderBoundingBoxes::InitBuffers()
{
	m_ConstantBufferDirect_PerBB = m_RendererCommand.CreateConstantBufferDirect<ConstantBuffer_Data_PerBoundingBox>(TE_IDX_GRESOURCES::Constant_RenderBoundingBox);
}

void TruthEngine::RenderPass_RenderBoundingBoxes::InitPipelines()
{
	RendererStateSet _States = InitRenderStates
	(
		TE_RENDERER_STATE_ENABLED_SHADER_HS_FALSE,
		TE_RENDERER_STATE_ENABLED_SHADER_DS_FALSE,
		TE_RENDERER_STATE_ENABLED_SHADER_GS_TRUE,
		TE_RENDERER_STATE_ENABLED_MAP_DIFFUSE_FALSE,
		TE_RENDERER_STATE_ENABLED_MAP_NORMAL_FALSE,
		TE_RENDERER_STATE_ENABLED_MAP_DISPLACEMENT_FALSE,
		TE_RENDERER_STATE_ENABLED_MAP_SPECULAR_FALSE,
		TE_RENDERER_STATE_ENABLED_MAP_ROUGHNESS_FALSE,
		TE_RENDERER_STATE_ENABLED_MAP_METALLIC_FALSE,
		TE_RENDERER_STATE_ENABLED_MAP_AMBIENTOCCLUSION_FALSE,
		TE_RENDERER_STATE_FRONTCOUNTERCLOCKWISE_FALSE,
		TE_RENDERER_STATE_ENABLED_DEPTH_TRUE,
		TE_RENDERER_STATE_ENABLED_STENCIL_FALSE,
		TE_RENDERER_STATE_FILL_MODE_SOLID,
		TE_RENDERER_STATE_CULL_MODE_BACK,
		TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_POINTLIST
	);

	const auto _ShaderHandle = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::RENDERBOUNDINGBOX, 0, "Assets/Shaders/RenderBoundingBox.hlsl", "vs", "ps", "", "", "", "gs");

	TE_RESOURCE_FORMAT rtvFormats[] =
	{
		m_RendererLayer->GetFormatRenderTargetSceneSDR()
	};

	PipelineDepthStencilDesc _PipelineDSDesc{TE_DEPTH_WRITE_MASK::ZERO, TE_COMPARISON_FUNC::LESS};

	PipelineGraphics::Factory(&m_Pipeline, _States, _ShaderHandle, _countof(rtvFormats), rtvFormats, m_RendererLayer->GetFormatDepthStencilSceneDSV(), {}, true, PipelineBlendDesc{}, _PipelineDSDesc);
}

void TruthEngine::RenderPass_RenderBoundingBoxes::ReleaseRendererCommand()
{
	m_RendererCommand.Release();
}

void TruthEngine::RenderPass_RenderBoundingBoxes::ReleaseTextures()
{
}

void TruthEngine::RenderPass_RenderBoundingBoxes::ReleaseBuffers()
{
}

void TruthEngine::RenderPass_RenderBoundingBoxes::ReleasePipelines()
{
}

void TruthEngine::RenderPass_RenderBoundingBoxes::RegisterEventListeners()
{
}

void TruthEngine::RenderPass_RenderBoundingBoxes::UnRegisterEventListeners()
{
}

