#include "pch.h"
#include "RenderPass_Wireframe.h"

#include "Core/Renderer/ShaderManager.h"
#include "Core/Renderer/RendererLayer.h"
#include "Core/Entity/Scene.h"
#include "Core/Entity/Camera/Camera.h"
#include "Core/Entity/Components/LightComponent.h"
#include "Core/Entity/Components/TransformComponent.h"
#include "Core/Event/EventRenderer.h"

#include "Core/Entity/Light/LightPoint.h"

namespace TruthEngine
{



	RenderPass_Wireframe::RenderPass_Wireframe(RendererLayer* _RendererLayer)
		: RenderPass(TE_IDX_RENDERPASS::WIREFRAME, _RendererLayer)
	{}

	void RenderPass_Wireframe::OnImGuiRender()
	{
	}

	void RenderPass_Wireframe::OnUpdate(double _DeltaTime)
	{
	}

	void RenderPass_Wireframe::BeginScene()
	{
		if (m_Queue.size() == 0)
			return;

		m_RendererCommand.BeginGraphics(&m_Pipeline);

		m_RendererCommand.SetViewPort(&m_RendererLayer->GetViewportScene(), &m_RendererLayer->GetViewRectScene());
		m_RendererCommand.SetRenderTarget(m_RendererLayer->GetRenderTargetViewSceneSDR());
		m_RendererCommand.SetDepthStencil(m_RendererLayer->GetDepthStencilViewScene());
	}

	void RenderPass_Wireframe::EndScene()
	{
		if (m_Queue.size() == 0)
			return;

		m_RendererCommand.End();

		m_Queue.clear();
	}

	void RenderPass_Wireframe::Render()
	{
		if (m_Queue.size() == 0)
			return;

		m_RendererCommand.ExecutePendingCommands();

		/*if (Entity _Entity = _Scene->GetSelectedEntity(); _Entity && _Scene->HasComponent<LightComponent>(_Entity))
		{
			const ILight* _Light = _Scene->GetComponent<LightComponent>(_Entity).GetLight();

			if (_Light->GetLightType() == TE_LIGHT_TYPE::Point)
			{
				float4x4A _Transform = _Scene->GetComponent<TransformComponent>(_Entity).GetTransform();

				const LightPoint* _LightPoint = static_cast<const LightPoint*>(_Light);

				float _EstRadius = _LightPoint->GetAttenuationStartRadius();
				XMMatrix _ScaleTransform = Math::XMTransformMatrixScale(Math::ToXM(float3{ _EstRadius,_EstRadius ,_EstRadius }));
				const Mesh& _Mesh = TE_INSTANCE_MODELMANAGER->GetPrimitiveMeshInstances().Sphere.GetMesh();
				Render(&_Mesh, Math::Multiply(_ScaleTransform, Math::ToXM(_Transform)), float4{0.8, 1.0, 0.0f, 1.0f});

				_EstRadius = _LightPoint->GetAttenuationEndRadius();
				_ScaleTransform = Math::XMTransformMatrixScale(Math::ToXM(float3{ _EstRadius,_EstRadius ,_EstRadius }));
				Render(&_Mesh, Math::Multiply(_ScaleTransform, Math::ToXM(_Transform)), float4{1.0, 0.7, 0.0f, 1.0f});
			}
		}*/

		const Camera* _Camera = GetActiveScene()->GetActiveCamera();
		XMMatrix _XMViewProj = Math::ToXM(_Camera->GetViewProj());

		for (auto& _QueueItem : m_Queue)
		{

			auto _CBData = m_ConstantBuffer->GetData();

			*_CBData = ConstantBufferData_Wireframe(Math::Multiply(Math::ToXM(_QueueItem.Transform), _XMViewProj), _QueueItem.Color);

			m_RendererCommand.SetDirectConstantGraphics(m_ConstantBuffer);

			m_RendererCommand.DrawIndexed(_QueueItem.Mesh);
		}


	}

	void RenderPass_Wireframe::Queue(const RenderPass_Wireframe::QueueItem& _QueueItem)
	{
		m_Queue.push_back(_QueueItem);
	}

	void RenderPass_Wireframe::InitRendererCommand()
	{
		m_RendererCommand.Init(TE_IDX_RENDERPASS::WIREFRAME, TE_IDX_SHADERCLASS::WIREFRAME);
	}

	void RenderPass_Wireframe::InitTextures()
	{
	}

	void RenderPass_Wireframe::InitBuffers()
	{
		m_ConstantBuffer = m_RendererCommand.CreateConstantBufferDirect<ConstantBufferData_Wireframe>(TE_IDX_GRESOURCES::Constant_Wireframe);
	}

	void RenderPass_Wireframe::InitPipelines()
	{
		const RendererStateSet _States = InitRenderStates
		(
			TE_RENDERER_STATE_ENABLED_SHADER_HS_FALSE,
			TE_RENDERER_STATE_ENABLED_SHADER_DS_FALSE,
			TE_RENDERER_STATE_ENABLED_SHADER_GS_FALSE,
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
			TE_RENDERER_STATE_FILL_MODE_WIREFRAME,
			TE_RENDERER_STATE_CULL_MODE_NONE,
			TE_RENDERER_STATE_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			TE_RENDERER_STATE_ENABLED_HDR_FALSE,
			TE_RENDERER_STATE_SHADING_MODEL_NONE,
			TE_RENDERER_STATE_ENABLED_BLEND_FALSE
		);

		const auto _ShaderHandle = TE_INSTANCE_SHADERMANAGER->AddShader(TE_IDX_SHADERCLASS::WIREFRAME, 0, "Assets/Shaders/RenderWireframe.hlsl", "vs", "ps");

		TE_RESOURCE_FORMAT rtvFormats[] = { m_RendererLayer->GetFormatRenderTargetSceneSDR() };

		const std::vector<ShaderInputElement> _InputElements{ ("POSITION", 0, TE_RESOURCE_FORMAT::R32G32B32_FLOAT, 0, 0, TE_RENDERER_SHADER_INPUT_CLASSIFICATION::PER_VERTEX, 0) };

		PipelineGraphics::Factory(&m_Pipeline, _States, _ShaderHandle, _countof(rtvFormats), rtvFormats, m_RendererLayer->GetFormatDepthStencilSceneDSV(), _InputElements, false);
	}

	void RenderPass_Wireframe::ReleaseTextures()
	{
	}

	void RenderPass_Wireframe::ReleaseBuffers()
	{
	}

	void RenderPass_Wireframe::ReleasePipelines()
	{
	}

	void RenderPass_Wireframe::RegisterEventListeners()
	{
	}

	void RenderPass_Wireframe::UnRegisterEventListeners()
	{
	}

	void RenderPass_Wireframe::ReleaseRendererCommand()
	{
		m_RendererCommand.Release();
	}

}